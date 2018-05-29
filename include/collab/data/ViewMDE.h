#pragma once

#include "CollabData.h"
#include "Timestamp.h"

#include "collab/CmRDT/LWWMap.h"
#include "collab/CmRDT/LWWGraph.h"
#include "collab/CmRDT/LWWRegister.h"

#include <string>

namespace collab {


/**
 * \brief
 * View data structure for Model Driven Engineering.
 *
 * ViewMDE is built on top of collab CRDTs.
 *
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
class ViewMDE : public CollabData {
    public:
        typedef std::string UUID;

    private:
        typedef CmRDT::LWWRegister<std::string, Timestamp>          attribute;
        typedef CmRDT::LWWMap<std::string, attribute, Timestamp>    attributesMap;

        CmRDT::LWWGraph<UUID, attributesMap, Timestamp> _modelMDE;


    public:

        /**
         * Add element in the View.
         *
         * \param id The element's ID.
         */
        void addElement(const UUID& id) {
            auto tnow = Timestamp::now();
            _modelMDE.addVertex(id, tnow);
        }

        /**
         * Remove elements from the view.
         *
         * \param id The element's ID.
         */
        void removeElement(const UUID& id) {
            auto tnow = Timestamp::now();
            _modelMDE.removeVertex(id, tnow);
        }

        /**
         * Add attribute inside specific element.
         *
         * \param eltID Element's ID.
         * \param name  Attribute's name.
         * \param value Attribute's value.
         */
        void addAttribute(const UUID& eltID,
                          const std::string& name,
                          const std::string& value) {
            auto tnow = Timestamp::now();
            auto v = _modelMDE.queryVertex(eltID);
            // TODO
        }

        /**
         * Remove attribute from the element.
         *
         * \param eltID Element's ID.
         * \param name  Attribute's name.
         */
        void removeAttribute(const UUID& eltID, const std::string& name) {
            auto tnow = Timestamp::now();
            // TODO
        }

        /**
         * Update attribute with new value.
         *
         * \param eltID Element's ID.
         * \param name  Attribute's name.
         * \param value Attribute's new value.
         */
        void updateAttribute(const UUID& eltID,
                             const std::string& name,
                             const std::string& value) {
            auto tnow = Timestamp::now();
            // TODO
        }

        /**
         * Add a directed link between two elements.
         *
         * \param from  ID of element link is starting from.
         * \param to    ID of element link is going to.
         */
        void addLink(const UUID& from, const UUID& to) {
            auto tnow = Timestamp::now();
            _modelMDE.addEdge(from, to, tnow);
        }

        void removeLink(const UUID& from, const UUID& to) {
            auto tnow = Timestamp::now();
            _modelMDE.removeEdge(from, to, tnow);
        }
};


} // End namespaces


