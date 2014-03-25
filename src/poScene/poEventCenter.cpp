//
//  poEventCenter.cpp
//  BasicTest
//
//  Created by Stephen Varga on 3/17/14.
//
//
#include "cinder/app/Window.h"

#include "poEventCenter.h"
#include "poScene.h"


namespace po {
    //------------------------------------------------------------------
    EventCenterRef EventCenter::create() {
        return EventCenterRef(new EventCenter());
    }
    
    EventCenter::EventCenter()
    {
        //Connect mouse events
        ci::app::getWindow()->connectMouseDown(&EventCenter::mouseDown,   this);
        ci::app::getWindow()->connectMouseMove(&EventCenter::mouseMove,   this);
        ci::app::getWindow()->connectMouseDrag(&EventCenter::mouseDrag,   this);
        ci::app::getWindow()->connectMouseUp(&EventCenter::mouseUp,       this);
        ci::app::getWindow()->connectMouseWheel(&EventCenter::mouseWheel, this);
    }
    
    
    //------------------------------------------------------------------------
    //Sort nodes to be top down
    bool sortByDrawOrderFunc(NodeRef &a, NodeRef &b) {
        return a->getDrawOrder() > b->getDrawOrder();
    }
    
    //Process all the event queues for this scene
    void EventCenter::processEvents(std::vector<NodeRef> nodes)
    {
        std::sort(nodes.begin(), nodes.end(), sortByDrawOrderFunc);
        
        processMouseEvents(nodes);
    }
    
    
    #pragma mark - Mouse Events -
    void EventCenter::processMouseEvents(std::vector<NodeRef> &nodes)
    {
        //Go through the queue
        for(auto& queue : mouseEventQueues) {
            //Get the type for this item in the std::map
            po::MouseEvent::Type type = (po::MouseEvent::Type)queue.first;
            
            //Go through all the ci::MouseEvents for this type
            for(ci::app::MouseEvent ciEvent : queue.second) {
                //Create a po::MouseEvent
                po::MouseEvent poEvent(ciEvent, type);
                notifyAllNodes(nodes,   poEvent);
                notifyCallbacks(nodes,  poEvent);
            }
            
            //Clear out the events
            queue.second.clear();
        }
    }
    
    //Dispatch to the appropriate mouse event function for each node in the scene
    void EventCenter::notifyAllNodes(std::vector<NodeRef> &nodes, po::MouseEvent event) {
        for(NodeRef node : nodes) {
            //Check if it is valid (the item hasn't been deleted) and if it is enabled for events
            if(!node->hasScene() || !node->isInteractionEnabled()) continue;
            
            //Notify the node
            event.setShouldPropagate(true);
            node->notifyGlobal(event);
        }
    }
    
    #pragma message "I def think this could be done in a better way, too much code"
    //Dispatch callback to top item, going up through draw tree
    void EventCenter::notifyCallbacks(std::vector<NodeRef> &nodes, po::MouseEvent event)
    {
        switch (event.type) {
            case MouseEvent::Type::DOWN:
                event.type = MouseEvent::Type::DOWN_INSIDE;
                break;
                
            case MouseEvent::Type::MOVE:
                event.type = MouseEvent::Type::MOVE_INSIDE;
                break;
                
            case MouseEvent::Type::UP:
                event.type = MouseEvent::Type::UP_INSIDE;
                break;
        }
        
        for(NodeRef node : nodes) {
            if(node->hasScene() &&
               node->isInteractionEnabled() &&
               node->hasCallbacks(event.getType()) &&
               node->pointInside(event.getWindowPos())
            ) {
                node->notifyCallbacks(event);
                if(event.getShouldPropagate()) {
                    event.setShouldPropagate(false);
                } else {
                    return;
                }
            }
        }
    }
    
    #pragma mark - Keyboard Events -
    #pragma mark - Touch Events -
    
}
