#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#pragma execution_character_set("utf-8")

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

	isRunning = false;
	timeCount = 180;
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}

	// 可以仿照攻击动画
	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(9);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	run.pushBack(frame0);

	auto wLabel = MenuItemLabel::create(Label::createWithTTF("W", "fonts/Arial.ttf", 36), [=](cocos2d::Ref* pSender) {
		if (isRunning) return;
		isRunning = true;
		
		if (player->getPosition().y + 50 < visibleSize.height) {
			auto animation = Animation::createWithSpriteFrames(run, 0.04f);
			animation->setRestoreOriginalFrame(true);
			auto animate = Animate::create(animation);
			auto spawn = Spawn::createWithTwoActions(MoveBy::create(0.32, Vec2(0, 50)), animate);
			player->runAction(spawn);
		}
		scheduleOnce(schedule_selector(HelloWorld::updateOnce), 0.32f);
	});
	wLabel->setPosition(50, 50);

	auto aLabel = MenuItemLabel::create(Label::createWithTTF("A", "fonts/Arial.ttf", 36), [=](cocos2d::Ref* pSender) {
		if (isRunning) return;
		isRunning = true;
		
		if (player->getPosition().x - 50 > 3) {

			auto animation = Animation::createWithSpriteFrames(run, 0.04f);
			animation->setRestoreOriginalFrame(true);
			auto animate = Animate::create(animation);
			auto spawn = Spawn::createWithTwoActions(MoveBy::create(0.32, Vec2(-50, 0)), animate);
			player->runAction(spawn);
		}
		scheduleOnce(schedule_selector(HelloWorld::updateOnce), 0.32f);
	});
	aLabel->setPosition(10, 10);

	auto sLabel = MenuItemLabel::create(Label::createWithTTF("S", "fonts/Arial.ttf", 36), [=](cocos2d::Ref* pSender) {
		if (isRunning) return;
		isRunning = true;
		if (player->getPosition().y - 50 > 3) {

			auto animation = Animation::createWithSpriteFrames(run, 0.04f);
			animation->setRestoreOriginalFrame(true);
			auto animate = Animate::create(animation);
			auto spawn = Spawn::createWithTwoActions(MoveBy::create(0.32, Vec2(0, -50)), animate);
			player->runAction(spawn);
		}
		scheduleOnce(schedule_selector(HelloWorld::updateOnce), 0.32f);
	});
	sLabel->setPosition(50, 10);

	auto dLabel = MenuItemLabel::create(Label::createWithTTF("D", "fonts/Arial.ttf", 36), [=](cocos2d::Ref* pSender) {
		if (isRunning) return;
		isRunning = true;
		
		if (player->getPosition().x + 50 < visibleSize.width) {

			auto animation = Animation::createWithSpriteFrames(run, 0.04f);
			animation->setRestoreOriginalFrame(true);
			auto animate = Animate::create(animation);
			auto spawn = Spawn::createWithTwoActions(MoveBy::create(0.32, Vec2(50, 0)), animate);
			player->runAction(spawn);
		}
		scheduleOnce(schedule_selector(HelloWorld::updateOnce), 0.32f);
	});	
	dLabel->setPosition(80, 10);

	auto opMenu = Menu::create(wLabel, sLabel, aLabel, dLabel, NULL);
	opMenu->setPosition(Vec2(100, 50));
	this->addChild(opMenu, 1);

	auto xLabel = MenuItemLabel::create(Label::createWithTTF("X", "fonts/Arial.ttf", 36), [=](Object* sender){		
		if (isRunning) return;
		isRunning = true;
		schedule(schedule_selector(HelloWorld::updateHPup), 0.01f, 25, 0); 
		auto animation = Animation::createWithSpriteFrames(attack, 0.1f);
		animation->setRestoreOriginalFrame(true);
		auto animate = Animate::create(animation);		
		player->runAction(animate);
		scheduleOnce(schedule_selector(HelloWorld::updateOnce), 2.5f);
	});
	xLabel->setPosition(Vec2(30, 30));
	
	auto yLabel = MenuItemLabel::create(Label::create("Y", "Arial", 36), [=](Object* sender) {
		if (isRunning) return;
		isRunning = true;
		schedule(schedule_selector(HelloWorld::updateHP), 0.01f, 25, 0);
		auto animation = Animation::createWithSpriteFrames(dead, 0.1f);
		animation->setRestoreOriginalFrame(true);
		auto animate = Animate::create(animation);
		player->runAction(animate);
		scheduleOnce(schedule_selector(HelloWorld::updateOnce), 2.5f);
	});

	auto menu = Menu::create(yLabel, NULL);
	menu->addChild(xLabel);
	menu->setPosition(Vec2(visibleSize.width - 100, 50));
	this->addChild(menu, 1);

	time = Label::createWithTTF("180", "fonts/Arial.ttf", 36);
	time->setPosition(Vec2(visibleSize.width/2, visibleSize.height - 50));
	this->addChild(time);

	schedule(schedule_selector(HelloWorld::timeCounter), 1.0f, kRepeatForever, 0);
	return true;
}

void HelloWorld::updateOnce(float dt) {
	this->isRunning = false;
}

void HelloWorld::updateHP(float dt) {
	float hp = pT->getPercentage();
	if (hp <= 0) {
		return;
	}
	pT->setPercentage(pT->getPercentage() - 1);
}

void HelloWorld::updateHPup(float dt) {
	float hp = pT->getPercentage();
	if (hp >= 100) {
		return;
	}
	pT->setPercentage(pT->getPercentage() + 1);
}

void HelloWorld::timeCounter(float dt) {
	std::string num = StringUtils::toString(timeCount);
	time->setString(num);
	if (timeCount == 0) {
		timeCount = 180;
	}
	timeCount--;
}