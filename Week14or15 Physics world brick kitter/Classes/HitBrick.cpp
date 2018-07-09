#pragma execution_character_set("utf-8")
#include "HitBrick.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace CocosDenshion;

void HitBrick::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* HitBrick::createScene() {
	srand((unsigned)time(NULL));
	auto scene = Scene::createWithPhysics();

	scene->getPhysicsWorld()->setAutoStep(true);

	// Debug 模式
	// scene->getPhysicsWorld()->setDebugrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
	auto layer = HitBrick::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
	layer->setJoint();
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool HitBrick::init() {
	//////////////////////////////
	// 1. super init first
	if (!Layer::init()) {
		return false;
	}
	visibleSize = Director::getInstance()->getVisibleSize();


	auto edgeSp = Sprite::create();  //创建一个精灵
	auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 0.0f), 3);  //edgebox是不受刚体碰撞影响的一种刚体，我们用它来设置物理世界的边界
	edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2);  //位置设置在屏幕中央
	boundBody->setGroup(2);
	boundBody->setCategoryBitmask(0xFFFFFFFF);
	boundBody->setCollisionBitmask(0xFFFFFFFF);
	boundBody->setContactTestBitmask(0xFFFFFFFF);
	edgeSp->setPhysicsBody(boundBody);	
	edgeSp->setTag(5);
	addChild(edgeSp);


	preloadMusic(); // 预加载音效

	addSprite();    // 添加背景和各种精灵
	addListener();  // 添加监听器 
	addPlayer();    // 添加球与板
	BrickGeneraetd();  // 生成砖块


	schedule(schedule_selector(HitBrick::update), 0.01f, kRepeatForever, 0.1f);

	onBall = true;

	spFactor = 0;
	return true;
}

// 关节连接，固定球与板子
// Todo
void HitBrick::setJoint() {
	joint1 = PhysicsJointPin::construct(
		player->getPhysicsBody(), ball->getPhysicsBody(),
		Vec2(ball->getPosition().x, ball->getPosition().y)
	);
	m_world->addJoint(joint1);
}

// 预加载音效
void HitBrick::preloadMusic() {
	auto sae = SimpleAudioEngine::getInstance();
	sae->preloadEffect("gameover.mp3");
	sae->preloadBackgroundMusic("bgm.mp3");
	sae->playBackgroundMusic("bgm.mp3", true);
}

// 添加背景和各种精灵
void HitBrick::addSprite() {
	// add background
	auto bgSprite = Sprite::create("bg.png");
	bgSprite->setPosition(visibleSize / 2);
	bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
	this->addChild(bgSprite, 0);


	// add ship
	ship = Sprite::create("ship.png");
	ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
	ship->setPosition(visibleSize.width / 2, 0);
	auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
	shipbody->setCategoryBitmask(0xFFFFFFFF);
	shipbody->setCollisionBitmask(0xFFFFFFFF);
	shipbody->setContactTestBitmask(0xFFFFFFFF);
	shipbody->setTag(1);
	shipbody->setDynamic(false);
	ship->setPhysicsBody(shipbody);
	ship->setTag(1);
	this->addChild(ship, 1);

	// add sun and cloud
	auto sunSprite = Sprite::create("sun.png");
	sunSprite->setPosition(rand() % (int)(visibleSize.width - 200) + 100, 550);
	this->addChild(sunSprite);
	auto cloudSprite1 = Sprite::create("cloud.png");
	cloudSprite1->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
	this->addChild(cloudSprite1);
	auto cloudSprite2 = Sprite::create("cloud.png");
	cloudSprite2->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
	this->addChild(cloudSprite2);

	// add score recorder
	scoreRecord = Label::createWithTTF("SCORE: 0", "fonts/Marker Felt.ttf", 36);
	scoreRecord->setPosition(100, visibleSize.height - 150);
	this->addChild(scoreRecord);
}

// 添加监听器
void HitBrick::addListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(HitBrick::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(HitBrick::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	auto contactListener = EventListenerPhysicsContact::create();	
	contactListener->onContactBegin = CC_CALLBACK_1(HitBrick::onConcactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// 创建角色
void HitBrick::addPlayer() {

	player = Sprite::create("bar.png");
	int xpos = visibleSize.width / 2;

	player->setScale(0.1f, 0.1f);
	player->setPosition(Vec2(xpos, ship->getContentSize().height - player->getContentSize().height*0.1f));
	// 设置板的刚体属性
	auto physicsBody = PhysicsBody::createBox(player->getContentSize(),
		PhysicsMaterial(0.1f, 1.0f, 0.0f));
	physicsBody->setGravityEnable(false);
	physicsBody->setDynamic(false);
	physicsBody->setGroup(2);
	physicsBody->setCategoryBitmask(0xffffffff);
	physicsBody->setCollisionBitmask(0xffffffff);
	physicsBody->setContactTestBitmask(0xffffffff);

	player->setTag(10);
	player->setPhysicsBody(physicsBody);

	this->addChild(player, 2);

	ball = Sprite::create("ball.png");
	ball->setPosition(Vec2(xpos, player->getPosition().y + ball->getContentSize().height*0.1f/2));
	ball->setScale(0.1f, 0.1f);
	// 设置球的刚体属性
	physicsBody = PhysicsBody::createCircle(ball->getContentSize().width / 2,
		PhysicsMaterial(0.1f, 1.0f, 0.0f));
	
	physicsBody->setGravityEnable(false);
	physicsBody->setGroup(2);
	physicsBody->setCategoryBitmask(0xffffffff);
	physicsBody->setCollisionBitmask(0xffffffff);
	physicsBody->setContactTestBitmask(0xffffffff);
	physicsBody->setVelocityLimit(600);
	ball->setPhysicsBody(physicsBody);
	ball->setTag(2);
	addChild(ball, 3);

	// 光尾实现[2]
	lightTail = nullptr;
}

void HitBrick::update(float dt) {
	// 实现简单的蓄力效果
	if (spHolded) {
		spFactor++;
	}
	else {
		spFactor = 0;
	}
	// 计分
	char str[10];
	sprintf(str, "SCORE: %d", score);
	scoreRecord->setString(str);
	// 球的光尾效果
	//if ((abs(ball->getPhysicsBody()->getVelocity().x - 0) > 30 ||
	//	abs(ball->getPhysicsBody()->getVelocity().y - 0) > 30) &&
	//	ballTailCount == 6)
	//{
	//	auto particle = ParticleMeteor::create();
	//	particle->setPosition(ball->getPosition());
	//	particle->setDuration(0.04f);
	//	particle->setLifeVar(0.04f);
	//	particle->setScale(1.3f);
	//	particle->setAutoRemoveOnFinish(true);
	//	addChild(particle);
	//}
	//ballTailCount = (ballTailCount + 1) % 7;
	
	// 另一种实现
	if (abs(ball->getPhysicsBody()->getVelocity().y - 0) > 30) 
	{
		if (lightTail == nullptr) {
			lightTail = ParticleMeteor::create();
			lightTail->setPosition(ball->getPosition());
			lightTail->setDuration(-1);			
			lightTail->setEmitterMode(ParticleSystem::Mode::RADIUS);
			lightTail->setStartRadius(25);
			lightTail->setEndRadius(25);			
			lightTail->setAutoRemoveOnFinish(true);
			addChild(lightTail);
		}
		lightTail->setPosition(ball->getPosition());
	}
	else 
	{
		removeChild(lightTail, true);
		lightTail = nullptr;
	}

	
}

// Todo
void HitBrick::BrickGeneraetd() {

	for (int i = 0; i < 3; i++) {
		int cw = 0;
		while (cw <= visibleSize.width) {
			auto box = Sprite::create("box.png");

			auto physicsBody = PhysicsBody::createBox(box->getContentSize(),
				PhysicsMaterial(0.1f, 1.0f, 0.0f));

			physicsBody->setVelocity(Vec2(0, 0));
			physicsBody->setGravityEnable(false);
			physicsBody->setDynamic(false);
			physicsBody->setGroup(2);			
			physicsBody->setCategoryBitmask(0xffffffff);
			physicsBody->setCollisionBitmask(0xffffffff);
			physicsBody->setContactTestBitmask(0xffffffff);
			box->setPosition(Vec2(cw, visibleSize.height - i * box->getContentSize().height));
			box->setPhysicsBody(physicsBody);

			box->setTag(3);
			this->addChild(box);
			cw += box->getContentSize().width;
		}
	}
}


// 左右
void HitBrick::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {

	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		if (abs(player->getPosition().x - 0) > 30) 
			player->getPhysicsBody()->setVelocity(Vec2(-800, 0));
		else
			player->getPhysicsBody()->setVelocity(Vec2(0, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		// 左右移动
		if (abs(player->getPosition().x - visibleSize.width) > 30)
			player->getPhysicsBody()->setVelocity(Vec2(800, 0));
		else
			player->getPhysicsBody()->setVelocity(Vec2(0, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE: // 开始蓄力
		spHolded = true;
		break;
	default:
		break;
	}
}

// 释放按键
void HitBrick::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		// 停止运动
		player->getPhysicsBody()->setVelocity(Vec2(0, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		// 蓄力结束，小球发射
		m_world->removeJoint(joint1);
		spHolded = false;		
		ball->getPhysicsBody()->setVelocity(Vec2(ball->getPhysicsBody()->getVelocity().x, spFactor * 10 + 100));
		break;
	default:
		break;
	}
}

// 碰撞检测
// Todo
bool HitBrick::onConcactBegin(PhysicsContact & contact) 
{
	auto c1 = contact.getShapeA(), c2 = contact.getShapeB();
	auto node1 = c1->getBody()->getNode(),
		node2 = c2->getBody()->getNode();
	if (node1 && node2) 
	{
		log("tags are %d - %d", node1->getTag(), node2->getTag());
		if (node1->getTag() == 3 && node2->getTag() == 2) 
		{	
			score++;
			auto p = ParticleExplosion::create();
			p->setPosition(node2->getPosition());
			p->setDuration(0.1f);
			p->setLife(0.1f);
			p->setScale(0.6f);
			p->setAutoRemoveOnFinish(true);
			addChild(p);
			auto temp = node1;
			node1->runAction(
				CallFunc::create([temp] {
					temp->removeFromParentAndCleanup(true);
				})
			);
			node2->getPhysicsBody()->setVelocity(node2->getPhysicsBody()->getVelocity() * 1.1);
		}
		else if (node2->getTag() == 3 && node1->getTag() == 2)
		{
			score++;
			auto p = ParticleExplosion::create();
			p->setPosition(node2->getPosition());
			p->setDuration(0.1f);
			p->setLife(0.1f);
			p->setScale(0.6f);
			p->setAutoRemoveOnFinish(true);
			addChild(p);
			auto temp = node2;
			node2->runAction(
				CallFunc::create([temp] {
					temp->removeFromParentAndCleanup(true);
				})
			);
			node1->getPhysicsBody()->setVelocity(node1->getPhysicsBody()->getVelocity() * 1.1);
		}
		else if ((node2->getTag() == 1 && node1->getTag() == 2) ||
			(node1->getTag() == 1 && node2->getTag() == 2))
		{
			GameOver();
		}

	}
	return true;
}


void HitBrick::GameOver() {

	_eventDispatcher->removeAllEventListeners();
	ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
	player->getPhysicsBody()->setVelocity(Vec2(0, 0));
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
	SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

	auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
	label1->setColor(Color3B(0, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1);

	auto label2 = Label::createWithTTF("重玩", "fonts/STXINWEI.TTF", 40);
	label2->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HitBrick::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
	this->addChild(replay);

	auto label3 = Label::createWithTTF("退出", "fonts/STXINWEI.TTF", 40);
	label3->setColor(Color3B(0, 0, 0));
	auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HitBrick::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
	this->addChild(exit);
}

// 继续或重玩按钮响应函数
void HitBrick::replayCallback(Ref * pSender) {
	Director::getInstance()->replaceScene(HitBrick::createScene());
}

// 退出
void HitBrick::exitCallback(Ref * pSender) {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
