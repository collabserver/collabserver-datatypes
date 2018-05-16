#pragma once

#include "CmRDT/LWWMap.h"
#include "CmRDT/LWWGraph.h"
#include <string>
#include <chrono>

namespace collab {


/**
 * TODO
 */
class LWWViewMDE {
    public:
        typedef std::string UUID;

    private:
        typedef std::chrono::steady_clock::time_point Timestamp;
        typedef CRDT::CmRDT::LWWMap<std::string, std::string, Timestamp> Attribute;

    private:
        CRDT::CmRDT::LWWGraph<UUID, Attribute, Timestamp> _modelMDE;


    public:

        void addElement(const UUID& id) {
            auto tnow = std::chrono::steady_clock::now();
            _modelMDE.addVertex(id, tnow);
        }

        void removeElement(const UUID& id) {
            auto tnow = std::chrono::steady_clock::now();
            _modelMDE.removeVertex(id, tnow);
        }

        void addAttribute() {
            // TODO
        }

        void removeAttribute() {
            // TODO
        }

        void updateAttribute() {
            // TODO
        }

        void addLinkElement() {
            // TODO
        }

        void removeLinkElement() {
            // TODO
        }
};


} // End namespaces


