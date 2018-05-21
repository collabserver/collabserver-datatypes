#pragma once

#include <unordered_map>
#include <cassert>
#include <ostream>

namespace CRDT {
namespace CmRDT {


/**
 * Last-Writer-Wins Map (LWW Map).
 * CmRDT (Operation-based)
 *
 * Associative container that contains key-value pairs with unique keys.
 * Timestamps is assigned to each add / remove operation to create total order
 * of operations. Internally, remove operation does not actually remove 
 * the key-value but only mark it as deleted.
 *
 * \note
 * CRDT Map only deals with concurrent add / remove of keys. The content for 
 * a key is not CRDT by itself. It is on your call to deal with the content.
 * You may for instance use a LWWRegister. (In such case, after any add,
 * call register.update function to also update, if necessary, the content).
 *
 * \warning
 * Timestamps are strictly unique with total order.
 * If (t1 == t2) is true, replicates may diverge.
 * (See quote and implementation).
 *
 *
 * \note
 * Quote from the CRDT article "A comprehensive study of CRDT":
 * "
 *  A Last-Writer-Wins [...] creates a total order of
 *  assignments by associating a timestamp with each update.
 *  Timestamps are assumed unique, totally ordered, and consistent with causal
 *  order; i.e., if assignment 1 happened-before assignment 2,
 *  the former's timestamp is less than the latter's. This may be implemented as
 *  a per-replicate counter concatenated with a unique replica identifier,
 *  such as its MAC address.
 * "
 *
 *
 * \tparam Key  Type of key.
 * \tparam T    Type of element.
 * \tparam U    Type of timestamps (Implements operators > and <).
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
template<typename Key, typename T, typename U>
class LWWMap {
    public:
        class Element;

        typedef typename std::unordered_map<Key, Element>::iterator load_iterator;

    private:
        std::unordered_map<Key, Element> _map;


    // -------------------------------------------------------------------------
    // CRDT methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Query a key-element and its internal CRDT metadata.
         * This means, query on a removed key will return this key with 
         * removed metadata to true. This may be useful to have CRDT update
         * (Query will return the key-elt, whereas it has been deleted or not).
         *
         * If this key has never been added in set, returns past-the-end
         * (See end()) iterator.
         *
         * \param key The key to find.
         * \return Iterator to the key-element or end() if not found.
         */
        load_iterator query(const Key& key) {
            return _map.find(key);
        }

        /**
         * Inserts new key in the container.
         *
         * Add a new key into the container.
         * If key already exists, update timestamps if was smaller than this one.
         * This is required for CRDT properties and commutativity.
         *
         * \param key   Key of the element to add.
         * \param stamp Timestamps of this operation.
         */
        void add(const Key& key, const U& stamp) {
            Element elt; // Content is not set here
            elt._timestamp  = stamp;
            elt._isRemoved  = false;

            auto res        = _map.insert(std::make_pair(key, elt));
            bool keyAdded   = res.second;
            Element& keyElt = res.first->second;
            U keyStamp      = keyElt._timestamp;

            if(!keyAdded) {
                if(keyStamp < stamp) {
                    keyElt._timestamp = stamp;
                    keyElt._isRemoved = false;
                }
            }
        }

        /**
         * Remove a key from the container.
         *
         * If key doesn't exists, add it first with removed state true.
         * This is because remove / add are commutative and remove may be
         * received before add.
         *
         * \param key   Key of the element to add.
         * \param stamp Timestamps of this operation.
         */
        void remove(const Key& key, const U& stamp) {
            Element elt; // Content is not set here
            elt._timestamp  = stamp;
            elt._isRemoved  = true;

            auto res        = _map.insert(std::make_pair(key, elt));
            bool keyAdded   = res.second;
            Element& keyElt = res.first->second;
            U keyStamp      = keyElt._timestamp;

            if(!keyAdded) {
                if(keyStamp < stamp) {
                    keyElt._timestamp = stamp;
                    keyElt._isRemoved = true;
                }
            }
        }


    // -------------------------------------------------------------------------
    // Iterator
    // -------------------------------------------------------------------------

    public:

        /**
         * Returns a load iterator to the beginning.
         */
        load_iterator lbegin() {
            return _map.begin();
        }

        /**
         * Returns a load iterator to the end.
         */
        load_iterator lend() {
            return _map.end();
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

        /**
         * Check if lhs and rhs are equals.
         * Two LWWMap are equal if their map of 'living' keys are equal.
         *
         * \return True if equal, otherwise, return false.
         */
        friend bool operator==(const LWWMap& lhs, const LWWMap& rhs) {
            // TODO: should only test for living elements.
            // atm: doesn't do the job specified by the doc.
            return lhs._map == rhs._map;
        }

        /**
         * Check if lhs and rhs are not equals.
         * See operator == for further information about equality meaning.
         *
         * \return True if not equal, otherwise, return false.
         */
        friend bool operator!=(const LWWMap& lhs, const LWWMap& rhs) {
            return !(lhs == rhs);
        }

        /**
         * Display the internal map content.
         * This is mainly for debug print purpose.
         */
        friend std::ostream& operator<<(std::ostream& out,
                                        const LWWMap<Key,T,U>& o) {
            out << "CmRDT::LWWMap = ";
            for(const auto& elt : o._map) {
                out << "(K=" << elt.first
                    << ",T=" << elt.second._content
                    << ",U=" << elt.second._timestamp;
                if(elt.second._isRemoved == true) {
                    out << ",removed) ";
                }
                else {
                    out << ",alive) ";
                }
            }
            return out;
        }
};


// *****************************************************************************
// Nested classes
// *****************************************************************************

/**
 * Map cell component.
 */
template<typename Key, typename T, typename U>
class LWWMap<Key, T, U>::Element {
    public:
        T       _content;
        U       _timestamp;
        bool    _isRemoved;

    public:
        friend bool operator==(const Element& lhs, const Element& rhs) {
            return (lhs._content == rhs._content)
                    && (lhs._timestamp == rhs._timestamp)
                    && (lhs._isRemoved == rhs._isRemoved);
        }
        friend bool operator!=(const Element& lhs, const Element& rhs) {
            return !(lhs == rhs);
        }
};


}} // End namespace


