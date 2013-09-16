#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "../../VisibleRect.h"
#include "../../testBasic.h"

class ArmatureTestScene : public TestScene
{
public: 
	ArmatureTestScene(bool bPortrait = false);

	virtual void runThisTest();

	// The CallBack for back to the main menu scene
	virtual void MainMenuCallback(Object* sender);
};

enum {
	TEST_COCOSTUDIO_WITH_SKELETON = 0,
	TEST_COCOSTUDIO_WITHOUT_SKELETON,
	TEST_DRAGON_BONES_2_0,
	TEST_PERFORMANCE,
	TEST_CHANGE_ZORDER,
	TEST_ANIMATION_EVENT,
	TEST_PARTICLE_DISPLAY,
	TEST_USE_DIFFERENT_PICTURE,
	TEST_BOX2D_DETECTOR,
	TEST_BOUDINGBOX,
	TEST_ANCHORPOINT,
	TEST_ARMATURE_NESTING,

	TEST_LAYER_COUNT
};

class ArmatureTestLayer : public Layer
{
public:
	virtual void onEnter();
	virtual void onExit();

	virtual std::string title();
	virtual std::string subtitle();

	void restartCallback(Object* sender);
	void nextCallback(Object* sender);
	void backCallback(Object* sender);

	virtual void draw();
};



class TestDragonBones20 : public ArmatureTestLayer
{
public:
	virtual void onEnter();
	virtual std::string title();
};



class TestCSWithSkeleton : public ArmatureTestLayer
{
	virtual void onEnter();
	virtual std::string title();
};

class TestCSWithoutSkeleton : public ArmatureTestLayer
{
	virtual void onEnter();
	virtual std::string title();
};


class TestPerformance : public ArmatureTestLayer
{
public:
	~TestPerformance();

	virtual void onEnter();
	virtual std::string title();
	virtual std::string subtitle();
	virtual void addArmature(cocos2d::extension::armature::Armature *armature);
	void update(float delta);

	int armatureCount;

	int frames;
	float times;
	float lastTimes;
	bool generated;
};


class TestChangeZorder : public ArmatureTestLayer
{
	virtual void onEnter();
	virtual std::string title();
	void changeZorder(float dt);

	int currentTag;
};


class TestAnimationEvent : public ArmatureTestLayer, public sigslot::has_slots<>
{
public:

	virtual void onEnter();
	virtual std::string title();
	void animationEvent(cocos2d::extension::armature::Armature *armature, cocos2d::extension::armature::MovementEventType movementType, const char *movementID);
	void callback1();
	void callback2();

	cocos2d::extension::armature::Armature *armature;
};

class TestUseMutiplePicture : public ArmatureTestLayer
{
	virtual void onEnter();
	virtual std::string title();
	virtual std::string subtitle();
	virtual bool onTouchBegan(Touch  *touch, Event  *event);
//	virtual void registerWithTouchDispatcher();

	int displayIndex;
	cocos2d::extension::armature::Armature *armature;
};

class TestParticleDisplay : public ArmatureTestLayer
{
	virtual void onEnter();
	virtual std::string title();
	virtual std::string subtitle();
	virtual bool onTouchBegan(Touch  *touch, Event  *event);
//	virtual void registerWithTouchDispatcher();

	int animationID;
	cocos2d::extension::armature::Armature *armature;
};

class TestBox2DDetector : public ArmatureTestLayer, public sigslot::has_slots<>
{
public:
	virtual void onEnter();
	virtual std::string title();
	virtual void draw();
	virtual void update(float delta);

	void onHit(cocos2d::extension::armature::Bone *bone, cocos2d::extension::armature::Bone *bone2);

	cocos2d::extension::armature::Armature *armature;
	cocos2d::extension::armature::Armature *armature2;
};

class TestBoundingBox : public ArmatureTestLayer
{
public:
	virtual void onEnter();
	virtual std::string title();
	virtual void draw();

	cocos2d::extension::armature::Armature *armature;
	Rect rect;
};

class TestAnchorPoint : public ArmatureTestLayer
{
public:
	virtual void onEnter();
	virtual std::string title();
};

class TestArmatureNesting : public ArmatureTestLayer
{
public:
	virtual void onEnter();
	virtual std::string title();
	virtual bool onTouchBegan(Touch  *touch, Event  *event);
//	virtual void registerWithTouchDispatcher();

	cocos2d::extension::armature::Armature *armature;
	int weaponIndex;
};
#endif  // __HELLOWORLD_SCENE_H__