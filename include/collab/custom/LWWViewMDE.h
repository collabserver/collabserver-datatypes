#pragma once

#include "collab/CmRDT/LWWMap.h"
#include "collab/CmRDT/LWWGraph.h"

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
        typedef CmRDT::LWWMap<std::string, std::string, Timestamp> Attribute;

    private:
        CmRDT::LWWGraph<UUID, Attribute, Timestamp> _modelMDE;


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


