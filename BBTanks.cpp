#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <iostream>
#include <OgreCameraMan.h>
#include "ImguiManager.h"
#include <OgreRenderQueueListener.h>

// render queues
#define RENDER_QUEUE_OUTLINE_GLOW_OBJECTS	RENDER_QUEUE_MAIN + 1
#define RENDER_QUEUE_OUTLINE_GLOW_GLOWS		RENDER_QUEUE_MAIN + 2
#define RENDER_QUEUE_FULL_GLOW_ALPHA_GLOW	RENDER_QUEUE_MAIN + 3
#define RENDER_QUEUE_FULL_GLOW_GLOW			RENDER_QUEUE_MAIN + 4
#define LAST_STENCIL_OP_RENDER_QUEUE		RENDER_QUEUE_FULL_GLOW_GLOW

// stencil values
#define STENCIL_VALUE_FOR_OUTLINE_GLOW 1
#define STENCIL_VALUE_FOR_FULL_GLOW 2
#define STENCIL_FULL_MASK 0xFFFFFFFF

class BBTanks : public OgreBites::ApplicationContext, public OgreBites::InputListener, public Ogre::RenderQueueListener
{
public:
    BBTanks() : OgreBites::ApplicationContext( "BBTanks" )
    {
    }

    ~BBTanks()
    {
        delete cameraMan;
        cameraMan = 0;
    }

    bool frameStarted(const Ogre::FrameEvent& evt)
    {
        OgreBites::ApplicationContext::frameStarted(evt);

        Ogre::ImguiManager::getSingleton().newFrame(
            evt.timeSinceLastFrame,
            Ogre::Rect(0, 0, getRenderWindow()->getWidth(), getRenderWindow()->getHeight()));

        ImGui::ShowDemoWindow();

//        Ogre::Vector3 transVector = Ogre::Vector3::ZERO;
//        int mMoveScale = 250;
//
//
//        if( evt.x.abs > (1920 - 20))
//        {
//            transVector.z -= mMoveScale;
//        }
//
//        scnMgr->getSceneNode("myCam" )->translate(transVector *evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);

        cameraMan->frameRendered( evt );
        return true;
    }

    virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation)
    {
        if (queueGroupId == Ogre::RENDER_QUEUE_OUTLINE_GLOW_OBJECTS) // outline glow object
        {
            Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem();

            // PORTABILIDAD: OGRE 1.11 se agrego el parametro: STENCIL_FULL_MASK
            // https://www.ogre3d.org/docs/api/1.8/class_ogre_1_1_render_system.html
            // https://ogrecave.github.io/ogre/api/1.11/class_ogre_1_1_render_system.html
            rendersys->clearFrameBuffer(Ogre::FBT_STENCIL);
            rendersys->setStencilCheckEnabled(true);
            rendersys->setStencilBufferParams(Ogre::CompareFunction::CMPF_ALWAYS_PASS,
                                              STENCIL_VALUE_FOR_OUTLINE_GLOW, STENCIL_FULL_MASK, STENCIL_FULL_MASK,
                                              Ogre::StencilOperation::SOP_KEEP,Ogre::StencilOperation::SOP_KEEP,Ogre::StencilOperation::SOP_REPLACE, false);
        }
        if (queueGroupId == Ogre::RENDER_QUEUE_OUTLINE_GLOW_GLOWS)  // outline glow
        {
            Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem();
            rendersys->setStencilCheckEnabled(true);
            rendersys->setStencilBufferParams(Ogre::CompareFunction::CMPF_NOT_EQUAL,
                                              STENCIL_VALUE_FOR_OUTLINE_GLOW, STENCIL_FULL_MASK, STENCIL_FULL_MASK,
                                              Ogre::StencilOperation::SOP_KEEP,Ogre::StencilOperation::SOP_KEEP,Ogre::StencilOperation::SOP_REPLACE,false);
        }
        if (queueGroupId == Ogre::RENDER_QUEUE_FULL_GLOW_ALPHA_GLOW)  // full glow - alpha glow
        {
            Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem();
            rendersys->setStencilCheckEnabled(true);
            rendersys->setStencilBufferParams(Ogre::CompareFunction::CMPF_ALWAYS_PASS,
                                              STENCIL_VALUE_FOR_FULL_GLOW,STENCIL_FULL_MASK, STENCIL_FULL_MASK,
                                              Ogre::StencilOperation::SOP_KEEP,Ogre::StencilOperation::SOP_KEEP,Ogre::StencilOperation::SOP_REPLACE,false);
        }

        if (queueGroupId == Ogre::RENDER_QUEUE_FULL_GLOW_GLOW)  // full glow - glow
        {
            Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem();
            rendersys->setStencilCheckEnabled(true);
            rendersys->setStencilBufferParams(Ogre::CompareFunction::CMPF_EQUAL,
                                              STENCIL_VALUE_FOR_FULL_GLOW,STENCIL_FULL_MASK, STENCIL_FULL_MASK,
                                              Ogre::StencilOperation::SOP_KEEP,Ogre::StencilOperation::SOP_KEEP,Ogre::StencilOperation::SOP_ZERO,false);
        }
    }

    virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation)
    {
        if ( queueGroupId == Ogre::LAST_STENCIL_OP_RENDER_QUEUE )
        {
            Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem();
            rendersys->setStencilCheckEnabled(false);
            rendersys->setStencilBufferParams();
        }
    }

#ifndef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
    void locateResources()
    {
        OgreBites::ApplicationContext::locateResources();
        // we have to manually specify the shaders
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
            "../resources", "FileSystem", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    }
#endif

    void setup()
    {
        OgreBites::ApplicationContext::setup();

        //getRenderWindow()->setFullscreen( false, 1920, 1080 );
        //OgreBites::ApplicationContextBase::setWindowGrab();

        addInputListener(this );

        Ogre::ImguiManager::createSingleton();

        addInputListener(Ogre::ImguiManager::getSingletonPtr()->getInputListener());

        // get a pointer to the already created root
        root = getRoot();

        scnMgr = root->createSceneManager();
        Ogre::ImguiManager::getSingleton().init(scnMgr);
        scnMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
        //scnMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

        // register our scene with the RTSS
        Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        shadergen->addSceneManager(scnMgr);

        Ogre::Light* light = scnMgr->createLight("MainLight" );
        Ogre::SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        //light->setType(Ogre::Light::LT_DIRECTIONAL);
        lightNode->setPosition(0, 10, 15);
        lightNode->attachObject(light);

        Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        camNode->setPosition(50, 50, 50 );
        camNode->lookAt(Ogre::Vector3(0, 0, 0 ), Ogre::Node::TS_PARENT);

        Ogre::Camera* cam = scnMgr->createCamera("myCam" );
        cam->setNearClipDistance(5); // specific to this sample
        cam->setAutoAspectRatio(true);
        camNode->attachObject(cam);
        getRenderWindow()->addViewport(cam);

        createGround();

        Ogre::SceneNode* firefly1Node = loadObject( "sherman_firefly.scene", Ogre::Vector3( 0, 0, 0 ) );

        dumpObject( firefly1Node );

        //Ogre::SceneNode* firefly2Node = scnMgr->getRootSceneNode()->createChildSceneNode();
        //cloneObject2( firefly1Node, firefly2Node, "Firefly2" );
        //firefly2Node->translate( Ogre::Vector3( 10, 0, 0 ), Ogre::Node::TS_WORLD );

        cameraMan = new OgreBites::CameraMan( camNode );
        cameraMan->setStyle( OgreBites::CameraStyle::CS_ORBIT );
        cameraMan->setTarget( selectedNode );
        cameraMan->setYawPitchDist( Ogre::Degree(180 ), Ogre::Degree(-40 ), 50 );
        cameraMan->setTopSpeed( 10 );

        addInputListener(cameraMan );

    }

    bool keyPressed(const OgreBites::KeyboardEvent& evt)
    {
        if (evt.keysym.sym == 27)
        {
            getRoot()->queueEndRendering();
        }

        //cameraMan->keyPressed( evt );
        return true;
    }

    bool mouseMoved(const OgreBites::MouseMotionEvent& evt)
    {
        cameraMan->mouseMoved( evt );
        return true;
    }

    bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
    {
        cameraMan->mouseWheelRolled( evt );
        return true;
    }

private:

    void createGround()
    {
        Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

        Ogre::MeshManager::getSingleton().createPlane(
                "ground", Ogre::RGN_DEFAULT,
                plane,
                1500, 1500, 20, 20,
                true,
                1, 5, 5,
                Ogre::Vector3::UNIT_Z);

        /*
         * Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -5);
            Ogre::MeshManager::getSingleton().createPlane("plane",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
            1500,1500,200,200,true,1,5,5,Ogre::Vector3::UNIT_Z);
         */

        Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("Examples/GrassFloor");
        material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureScale(0.1, 0.1 );

        Ogre::Entity* groundEntity = scnMgr->createEntity("ground" );

        groundEntity->setMaterial( material );

        scnMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
    }

    Ogre::SceneNode* cloneObject2( Ogre::SceneNode* fromSceneNode, Ogre::SceneNode* toSceneNode, char* name )
    {
        int index = 0;

        Ogre::Node*              childNode    = fromSceneNode->getChild(0);
        Ogre::Node::ChildNodeMap nodeChildren = childNode->getChildren();

        for ( auto & element : nodeChildren )
        {
            auto sceneNodeItem = static_cast<Ogre::SceneNode *>(element);

            Ogre::SceneNode::ObjectMap sceneNodeObjects = sceneNodeItem->getAttachedObjects();

            Ogre::Vector3    fromPos = sceneNodeItem->getPosition();
            Ogre::Quaternion fromRot = sceneNodeItem->getOrientation();

            for ( auto & attachedEntities : sceneNodeObjects )
            {
                auto fromEntity              = static_cast<Ogre::Entity *>(attachedEntities);
                auto newSceneNode = toSceneNode->createChildSceneNode(name + Ogre::StringConverter::toString(index ), fromPos, fromRot );
                auto toEntity         = fromEntity->clone( fromEntity->getName() + Ogre::StringConverter::toString(index ) );

                newSceneNode->attachObject(toEntity );

                index++;
            }
        }
    }

    Ogre::SceneNode* cloneObject( Ogre::SceneNode* sceneNode, char* name, Ogre::Vector3 pos )
    {
        Ogre::SceneNode* node = scnMgr->getRootSceneNode()->createChildSceneNode();

        Ogre::Node*              childNode    = sceneNode->getChild(0);
        Ogre::Node::ChildNodeMap nodeChildren = childNode->getChildren();

        //cloneNode( childNode, name, 0, node );

        int childCount = 1;

        for ( auto & element : nodeChildren )
        {
            /*
             Ogre::Entity* ent = scnMgr->createEntity("Sinbad.mesh");
             Ogre::SceneNode* node = scnMgr->getRootSceneNode()->createChildSceneNode();
             node->attachObject(ent);
            */

            auto sceneNodeItem = static_cast<Ogre::SceneNode *>(element);

            cloneNode( sceneNodeItem, name, childCount, node );

            childCount++;
        }

        node->translate( pos, Ogre::Node::TS_WORLD );

        return node;
    }

    void cloneNode( Ogre::SceneNode *sceneNode, char* name, int index, Ogre::SceneNode* parent )
    {
        auto sceneEntityItem = static_cast<Ogre::Entity *>(sceneNode->getAttachedObject( 0));

        std::string sceneObjectName = sceneNode->getName();

        std::string partName = name + std::string( "_" ) + sceneObjectName + std::string( "_" ) + std::to_string( index );

        Ogre::Entity*    clonedEntity = sceneEntityItem->clone( partName );
        Ogre::SceneNode* entityNode   = parent->createChildSceneNode( partName );

        entityNode->attachObject( clonedEntity );
    }

    Ogre::SceneNode* loadObject( char* objectName, Ogre::Vector3 pos )
    {
        Ogre::Entity* sceneEntity = nullptr;

        Ogre::SceneNode* node = scnMgr->getRootSceneNode()->createChildSceneNode();

        node->loadChildren(objectName );

        Ogre::SceneManager::MovableObjectIterator it = scnMgr->getMovableObjectIterator("Entity" );

        while ( it.hasMoreElements() )
        {
            Ogre::Entity* sceneEntityItem = static_cast<Ogre::Entity *>(it.getNext ());

            //selectedNode->attachObject( sceneEntityItem );

            std::string objectName = sceneEntityItem->getName();
            std::string parentName = sceneEntityItem->getParentNode()->getParent()->getName();
            std::cout << objectName << " - " << parentName << std::endl;

            if ( objectName.rfind("sherman_firefly", 0) == 0)
            {
                std::cout << "YEP" << std::endl;

                if ( objectName == "sherman_fireflySherman_Firefly" )
                {
                    sceneEntity = sceneEntityItem;

                    selectedNode = sceneEntityItem->getParentSceneNode();
                    selectedNode->rotate( Ogre::Vector3f (0.0f, 0.0f, 1.0f ), Ogre::Degree(180), Ogre::Node::TS_LOCAL );
                    selectedNode->translate( pos, Ogre::Node::TS_WORLD );
                    //sceneNode->pitch(Ogre::Degree(10), Ogre::Node::TS_LOCAL);
                    //sceneNode->roll(Ogre::Degree(-30), Ogre::Node::TS_LOCAL);

                    sceneEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_OUTLINE_GLOW_GLOWS);
                    sceneEntity->setMaterialName("cg/alpha_glow");
                }

                sceneEntityItem->getSubEntity( 0)->getMaterial()->getBestTechnique()->getPass( 0)->setSelfIllumination( 0.5, 0.5, 0.5 );
            }
        }

        return node;
    }

    void dumpObject( Ogre::SceneNode* node, int index = 0 )
    {
        index++;

        std::string name = node->getName();

        std::cout << "## DUMP : " << name << " : " << index << " ##" << std::endl;

        int numAttachedObjects = node->numAttachedObjects();

        if ( numAttachedObjects > 0 )
        {
            Ogre::SceneNode::ObjectMap subObjects = node->getAttachedObjects();

            if ( ! subObjects.empty() )
            {
                for ( auto & subObjectItem : subObjects )
                {
                    Ogre::MovableObject* movableObject = static_cast<Ogre::MovableObject*>( subObjectItem );

                    std::string objectType = movableObject->getMovableType();
                    std::string objectName = movableObject->getName();

                    std::cout << "    - E : " << objectName << " : " << objectType << std::endl;
                }
            }
        }

        Ogre::Node::ChildNodeMap nodeChildren = node->getChildren();

        for ( auto & element : nodeChildren )
        {
            std::string subNodeName = element->getName();

            std::cout << "    - SN : " << subNodeName << std::endl;

            if ( element->numChildren() > 0 )
            {
                Ogre::Node::ChildNodeMap subNodeChildren = element->getChildren();

                if ( ! subNodeChildren.empty() )
                {
                    for ( auto & subElement : subNodeChildren )
                    {
                        Ogre::SceneNode* subSceneNode = static_cast<Ogre::SceneNode*>( subElement );

                        dumpObject( subSceneNode, index );
                    }
                }
            }
        }
    }

    void highlight()
    {

    }

    Ogre::Root* root;
    Ogre::SceneManager* scnMgr;
    OgreBites::CameraMan* cameraMan;
    Ogre::SceneNode* selectedNode;
};


int main(int argc, char *argv[])
{
    BBTanks app;
    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();

    return 0;
}
