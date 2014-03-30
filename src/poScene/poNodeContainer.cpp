//
//  poNodeContainer.cpp
//  BasicScene
//
//  Created by Stephen Varga on 3/22/14.
//
//

#include "poNodeContainer.h"

namespace po {
    NodeContainerRef NodeContainer::create()
    {
        return std::shared_ptr<NodeContainer>(new NodeContainer);
    }

    NodeContainer::NodeContainer()
    {
    }

    int NodeContainer::getNumChildren()
    {
        return mChildren.size();
    }
    
    #pragma mark Set and Remove Scene
    void NodeContainer::setScene(SceneRef scene)
    {
        Node::setScene(scene);
        for(NodeRef &childNode : mChildren) {
            childNode->setScene(scene);
        }
    }
    
    void NodeContainer::removeScene()
    {
        Node::removeScene();
        for(NodeRef &childNode : mChildren) {
            childNode->removeScene();
        }
    }

    void NodeContainer::addChild(NodeRef node)
    {
        //See if the node is already a child of another node.
        if(node->getParent()) {
            node->getParent()->removeChild(node);
        }
        
        //Assign ourselves as the parent
        #pragma message "This is ugly...if this needs to be done a lot its gonna get messy with inheritance and shared_from_this"
        //node->setParent(enable_shared_from_this<NodeContainer>::shared_from_this());
        node->setParent(std::dynamic_pointer_cast<NodeContainer>(shared_from_this()));
        node->setScene(mScene.lock());
        
        //Track Node
        mChildren.push_back(node);
    }

    bool NodeContainer::removeChild(NodeRef node)
    {
        std::vector<NodeRef>::iterator iter = std::find(mChildren.begin(), mChildren.end(), node);
        
        if(iter != mChildren.end()) {
            //Remove reference to this node in child
            node->removeParent();
            node->removeScene();
            
            //Erase node
            mChildren.erase(iter);
            
            return true;
        }
        
        return false;
    }

    void NodeContainer::updateTree()
    {
        update();
        for(NodeRef &childNode : mChildren)
            childNode->updateTree();
    }

    void NodeContainer::drawTree()
    {
        #pragma message "Need to implement matrix order"
        ci::gl::pushMatrices();
        setTransformation();
        
        draw();
        for(NodeRef &childNode : mChildren) {
            childNode->drawTree();
            
            #pragma message "For testing, should be removed"
            ci::gl::color(0,255,0);
            ci::gl::drawStrokedRect(childNode->getFrame());
        }
        
        if(mDrawBounds)
            drawBounds();
        
        ci::gl::popMatrices();
    }

    ci::Rectf NodeContainer::getBounds()
    {
        //Reset Bounds
        ci::Rectf bounds = ci::Rectf(0,0,0,0);
        
        for(NodeRef &childNode : mChildren)
            bounds.include(childNode->getFrame());
        
        return bounds;
    }
    
    bool NodeContainer::pointInside(const ci::Vec2f &point)
    {
        for (NodeRef node : mChildren) {
            if (node->pointInside(point)) {
                return true;
            }
        }
        
        return false;
    }
}