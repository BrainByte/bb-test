// outline glow entity
Ogre::Entity *outlineGlowEntity = mSceneMgr->createEntity("outlineGlow", "ogrehead.mesh");
outlineGlowEntity->setRenderQueueGroup(RENDER_QUEUE_OUTLINE_GLOW_OBJECTS);
mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(outlineGlowEntity);

    // outline glow entity actual glow
    Ogre::Entity* actualOutlineGlowEntity = outlineGlowEntity->clone(outlineGlowEntity->getName() + "_glow");
    actualOutlineGlowEntity->setRenderQueueGroup(RENDER_QUEUE_OUTLINE_GLOW_GLOWS);
    actualOutlineGlowEntity->setMaterialName("cg/glow");
    if(outlineGlowEntity->hasSkeleton())    // To make the glow animate with it's parent
        actualOutlineGlowEntity->shareSkeletonInstanceWith(outlineGlowEntity);
    Ogre::SceneNode* actualOutlineGlowNode = outlineGlowEntity->getParentSceneNode()->createChildSceneNode("outlineGlowNode");
    actualOutlineGlowNode->attachObject(actualOutlineGlowEntity);

    // normal entity
    Ogre::Entity *normalOgreEntity = mSceneMgr->createEntity("normalOgreEntity", "ogrehead.mesh");
    Ogre::SceneNode* normalOgreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    normalOgreNode->attachObject(normalOgreEntity);
    normalOgreNode->setPosition(80, 0, 0);

    // full glow entity
    Ogre::Entity *fullGlowEntity = mSceneMgr->createEntity("fullGlowEntity", "ogrehead.mesh");
    Ogre::SceneNode* fullGlowNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    fullGlowNode->attachObject(fullGlowEntity);
    fullGlowNode->setPosition(-80, 0, 0);

    // full glow alpha glow
    Ogre::Entity* alphaFullGlowEntity = fullGlowEntity->clone(fullGlowEntity->getName() + "_alphaGlow");
    alphaFullGlowEntity->setRenderQueueGroup(RENDER_QUEUE_FULL_GLOW_ALPHA_GLOW);
    alphaFullGlowEntity->setMaterialName("cg/alpha_glow");
    if(fullGlowEntity->hasSkeleton())    // To make the glow animate with it's parent
        alphaFullGlowEntity->shareSkeletonInstanceWith(fullGlowEntity);
    Ogre::SceneNode* alphaFullGlowNode = fullGlowEntity->getParentSceneNode()->createChildSceneNode("fullGlowAlphaNode");
    alphaFullGlowNode->attachObject(alphaFullGlowEntity);

    // full glow alpha glow
    Ogre::Entity* glowFullGlowEntity = fullGlowEntity->clone(fullGlowEntity->getName() + "_glow");
    glowFullGlowEntity->setRenderQueueGroup(RENDER_QUEUE_FULL_GLOW_GLOW);
    glowFullGlowEntity->setMaterialName("cg/no_depth_check_glow");
    if(fullGlowEntity->hasSkeleton())    // To make the glow animate with it's parent
        glowFullGlowEntity->shareSkeletonInstanceWith(fullGlowEntity);
    Ogre::SceneNode* glowFullGlowNode = fullGlowEntity->getParentSceneNode()->createChildSceneNode("fullGlowGlowNode");
    glowFullGlowNode->attachObject(glowFullGlowEntity);

To disable, just remove the glow entity from the SceneNode:

actualOutlineGlowNode->detachObject(actualOutlineGlowEntity);
mSceneMgr->destroyEntity(actualOutlineGlowEntity);