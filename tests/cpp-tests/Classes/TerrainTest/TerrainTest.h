#ifndef TERRAIN_TESH_H
#include "../testBasic.h"
#include "../BaseTest.h"

#include "3d/CCSprite3D.h"
#include "3d/CCTerrain.h"
#include "2d/CCCamera.h"
#include "2d/CCAction.h"
class TerrainTestScene : public TestScene
{
public:
    virtual void runThisTest();
};

class TerrainTestDemo : public BaseTest
{
public:
    TerrainTestDemo(void);
    virtual ~TerrainTestDemo(void);

    void restartCallback(Ref* sender);
    void nextCallback(Ref* sender);
    void backCallback(Ref* sender);

    // overrides
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void onEnter() override;
};


class TerrainSimple : public TerrainTestDemo
{
public:
    CREATE_FUNC(TerrainSimple);
    TerrainSimple();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
protected:
};

#define PLAYER_STATE_LEFT 0 
#define PLAYER_STATE_RIGHT 1
#define PLAYER_STATE_IDLE 2
#define PLAYER_STATE_FORWARD 3
#define PLAYER_STATE_BACKWARD 4

class PlayerAction : public Action
{
public:
    PlayerAction(Camera * cam,Terrain * terrain);
    virtual bool isDone() const;
    virtual void step(float dt);

    void turnLeft();
    void turnRight();
    void forward();
    void backward();
    void idle();
    cocos2d::Vec3 _targetPos;
    void updateState();
private:
Terrain * _terrain;
Camera * _cam;
int _playerState;
};


class TerrainWalkThru : public TerrainTestDemo
{
public:
    CREATE_FUNC(TerrainWalkThru);
    TerrainWalkThru();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesEnd(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
protected:
    PlayerAction * _action;
    Label * forward;
    Label * backward;
    Label * left;
    Label * right;
    Camera * _camera;
    Terrain * _terrain;
    Sprite3D * _player;
};
#endif // !TERRAIN_TESH_H
