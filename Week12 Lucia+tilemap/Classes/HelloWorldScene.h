#pragma once

#include "cocos2d.h"
using namespace cocos2d;

#define database UserDefault::getInstance()

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
	bool isRunning;
	int timeCount;
	void timeCounter(float);
	void updateOnce(float);
	void updateHP(float);
	void updateHPup(float);
    virtual bool init();

	void addMonster(float);
	void monsterApproach(float);
	void checkHitAndDo(float);
	
	void updateMoveDiretion(char);
	void checkAttack();

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	cocos2d::Label* attackCountLabel;
	cocos2d::Size designResolutionSize;
	cocos2d::Size smallResolutionSize;
	cocos2d::Size mediumResolutionSize;
	cocos2d::Size largeResolutionSize;
	int dtime;
	cocos2d::ProgressTimer* pT;
	char dir;
};