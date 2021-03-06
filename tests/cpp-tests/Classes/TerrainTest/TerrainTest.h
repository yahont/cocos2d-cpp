#ifndef TERRAIN_TESH_H

#include "../BaseTest.h"

#include "3d/CCSprite3D.h"
#include "3d/CCTerrain.h"
#include "2d/CCCamera.h"
#include "2d/CCAction.h"

DEFINE_TEST_SUITE(TerrainTests);

class TerrainTestDemo : public TestCase
{
protected:
    std::string    _title;
};

class TerrainSimple : public TerrainTestDemo
{
public:
    static TerrainSimple* create()
    {
        auto ret = new TerrainSimple;
        ret->init();
        ret->autorelease();
        return ret;
    }
    TerrainSimple();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    cocos2d::Terrain* _terrain;
protected:
    cocos2d::Camera* _camera;
};

#define PLAYER_STATE_LEFT 0 
#define PLAYER_STATE_RIGHT 1
#define PLAYER_STATE_IDLE 2
#define PLAYER_STATE_FORWARD 3
#define PLAYER_STATE_BACKWARD 4

class Player : public cocos2d::Sprite3D
{
public:
    static Player * create(const char * file, cocos2d::Camera*  cam, cocos2d::Terrain* terrain);
    virtual bool isDone() const;
    virtual void update(float dt);

    void turnLeft();
    void turnRight();
    void forward();
    void backward();
    void idle();
    cocos2d::Vec3 _targetPos;
    void updateState();
    float _headingAngle;
    cocos2d::Vec3 _headingAxis;
private:
    cocos2d::Terrain* _terrain;
    cocos2d::Camera*  _cam;
    int _playerState;
};


class TerrainWalkThru : public TerrainTestDemo
{
public:
    static TerrainWalkThru* create()
    {
        auto ret = new TerrainWalkThru;
        ret->init();
        ret->autorelease();
        return ret;
    }
    TerrainWalkThru();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    void onTouchesEnd(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
protected:
    cocos2d::Camera*  _camera;
    cocos2d::Terrain* _terrain;
    Player * _player;
};

class TerrainWithLightMap : public TerrainTestDemo
{
public:
    static TerrainWithLightMap* create()
    {
        auto ret = new TerrainWithLightMap;
        ret->init();
        ret->autorelease();
        return ret;
    }
    TerrainWithLightMap();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
    
protected:
    cocos2d::Terrain* _terrain;
    cocos2d::Camera* _camera;
};

#endif // !TERRAIN_TESH_H
