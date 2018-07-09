#include "Thunder.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Thunder::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Thunder::init() {
	if (!Layer::init()) {
		return false;
	}
	stoneType = 0;
	isMove = false;  // 是否点击飞船
	visibleSize = Director::getInstance()->getVisibleSize();

	// 创建背景
	auto bgsprite = Sprite::create("bg.jpg");
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
		visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);

	// 创建飞船
	player = Sprite::create("player.png");
	player->setAnchorPoint(Vec2(0.5, 0.5));
	player->setPosition(visibleSize.width / 2, player->getContentSize().height);
	player->setName("player");
	this->addChild(player, 1);

	// 显示陨石和子弹数量
	enemysNum = Label::createWithTTF("enemys: 0", "fonts/arial.TTF", 20);
	enemysNum->setColor(Color3B(255, 255, 255));
	enemysNum->setPosition(50, 60);
	this->addChild(enemysNum, 3);
	bulletsNum = Label::createWithTTF("bullets: 0", "fonts/arial.TTF", 20);
	bulletsNum->setColor(Color3B(255, 255, 255));
	bulletsNum->setPosition(50, 30);
	this->addChild(bulletsNum, 3);

	addEnemy(5);        // 初始化陨石
	preloadMusic();     // 预加载音乐
	playBgm();          // 播放背景音乐
	explosion();        // 创建爆炸帧动画

	// 添加监听器
	addTouchListener();
	addKeyboardListener();
	addCustomListener();

	// 调度器
	schedule(schedule_selector(Thunder::update), 0.04f, kRepeatForever, 0);

	return true;
}

//预加载音乐文件
void Thunder::preloadMusic() {
	// Todo
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music/explore.wav");
	audio->preloadEffect("music/fire.wav");
}

//播放背景音乐
void Thunder::playBgm() {
	// Todo
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm.mp3", true);
}

//初始化陨石
void Thunder::addEnemy(int n) {
	enemys.clear();
	for (unsigned i = 0; i < 3; ++i) {
		char enemyPath[20];
		sprintf(enemyPath, "stone%d.png", 3 - i);
		double width = visibleSize.width / (n + 1.0),
			height = visibleSize.height - (50 * (i + 1));
		for (int j = 0; j < n; ++j) {
			auto enemy = Sprite::create(enemyPath);
			enemy->setAnchorPoint(Vec2(0.5, 0.5));
			enemy->setScale(0.5, 0.5);
			enemy->setPosition(width * (j + 1), height);
			enemys.push_back(enemy);
			addChild(enemy, 1);
		}
	}
}

// 陨石向下移动并生成新的一行(加分项)
void Thunder::newEnemy() {
	for (auto enemy : enemys) {
		enemy->runAction(MoveBy::create(0.04, Vec2(0, -50)));
	}
	char enemyPath[20];
	double width = visibleSize.width / 6,
		height = visibleSize.height - 50;
	for (int j = 0; j < 5; ++j) {
		int i = random() % 3 + 1;
		sprintf(enemyPath, "stone%d.png", i);
		auto enemy = Sprite::create(enemyPath);
		enemy->setAnchorPoint(Vec2(0.5, 0.5));
		enemy->setScale(0.5, 0.5);
		enemy->setPosition(width * (j + 0.5), height);
		enemys.push_back(enemy);
		addChild(enemy, 1);
	}
}

// 移动飞船
void Thunder::movePlane(char c) {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto tmp = player->getPosition();
	switch (c) {
	case 'A':
		if (player->getPosition().x - 60 < 0) {
			player->runAction(MoveTo::create(0.04f, Vec2(0, player->getPosition().y)));
		} else {
			player->runAction(MoveBy::create(0.04f, Vec2(-30, 0)));
		}
		break;
	case 'D':
		if ((player->getPosition()).x + 60 > visibleSize.width) {
 			player->runAction(MoveTo::create(0.04f, Vec2(visibleSize.width, player->getPosition().y)));
		} else {
			player->runAction(MoveBy::create(0.04f, Vec2(30, 0)));
		}
		break;
	default:
		break;
	}
}

//发射子弹
void Thunder::fire() {
	auto bullet = Sprite::create("bullet.png");
	bullet->setAnchorPoint(Vec2(0.5, 0.5));
	bullets.push_back(bullet);
	bullet->setPosition(player->getPosition());
	addChild(bullet, 1);
	SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);
}

// 切割爆炸动画帧
void Thunder::explosion() {
	// Todo
	auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
	explore.reserve(10);
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(193*i + 1, 193*j, 142, 142)));
			explore.pushBack(frame);
		}
	}
}

void Thunder::autoFire(float f) {
	if (isClick) fire();
}

void Thunder::update(float f) {
	// 实时更新页面内陨石和子弹数量(不得删除)
	// 要求数量显示正确(加分项)
	char str[15];
	sprintf(str, "enemys: %d", enemys.size());
	enemysNum->setString(str);
	sprintf(str, "bullets: %d", bullets.size());
	bulletsNum->setString(str);

	// 飞船移动
	if (isMove)
		this->movePlane(movekey);

	static int ct = 0;
	static int dir = 4;
	++ct;
	if (ct == 120)
		ct = 40, dir = -dir;
	else if (ct == 80) {
		dir = -dir;
		newEnemy();  // 陨石向下移动并生成新的一行(加分项)
	}
	else if (ct == 20)
		ct = 40, dir = -dir;

	//陨石左右移动
	for (Sprite* s : enemys) {
		if (s != NULL) {
			s->setPosition(s->getPosition() + Vec2(dir, 0));
		}
	}

	// 子弹向上飞行
	for (Sprite* s : bullets) {
		if (s != NULL) {
			s->runAction(MoveBy::create(0.04f, Vec2(0, 30)));
		}
	}

	// 移除飞出屏幕外的子弹
	auto visibleSize = Director::getInstance()->getVisibleSize();
	for (auto it = bullets.begin(); it != bullets.end(); ) {
		auto bullet = *it;
		if ((bullet->getPosition()).x > visibleSize.width ||
			(bullet->getPosition()).y > visibleSize.height ||
			(bullet->getPosition()).x < 0 ||
			(bullet->getPosition()).y < 0)
		{
			this->removeChild(bullet);
			it = bullets.erase(it);
		}
		else
		{
			it++;
		}
	}
	// 测试游戏结束
	for (auto it = enemys.begin(); it != enemys.end(); it++) {
		auto t = (*it)->getPosition().y;
		if ((*it)->getPosition().y < 30) {
			stopAc();
		}
	}

	// 分发自定义事件
	EventCustom e("meet");
	_eventDispatcher->dispatchEvent(&e);
}

// 自定义碰撞事件
void Thunder::meet(EventCustom * event) {
	// 判断子弹是否打中陨石并执行对应操作
	/*for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); ) {
		auto Erased = false;
		for (auto enemyIt = enemys.begin(); enemyIt != enemys.end(); ) {
			auto distance = ((*bulletIt)->getPosition()).getDistance((*enemyIt)->getPosition());
			if (distance < 50) {
				auto texture = Director::getInstance()->getTextureCache()->getTextureForKey("explosion.png");
				auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 188.8, 212.5)));
				
				Sprite* sp = Sprite::createWithSpriteFrame(frame);
				sp->setPosition((*enemyIt)->getPosition());
				this->addChild(sp, 1); 
				
				this->removeChild((*bulletIt));
				this->removeChild((*enemyIt));
				bulletIt = bullets.erase(bulletIt);
				enemyIt = enemys.erase(enemyIt);
				Erased = true;

				auto animation = Animation::createWithSpriteFrames(explore, 0.04f);
				auto animate = Animate::create(animation);
				sp->runAction(animate);

				SimpleAudioEngine::getInstance()->playEffect("music/explore.wav", false);
			}
			if (!Erased) {
				enemyIt++;
			} else {
				break;
			}
		}
		if (!Erased) {
			bulletIt++;
		}
	}
*/
	for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); ) {
		Sprite* shootedEnemy = nullptr;
		float minDistance = 50;
		for (auto enemyIt = enemys.begin(); enemyIt != enemys.end(); enemyIt++) {
			auto distance = (*bulletIt)->getPosition().getDistance((*enemyIt)->getPosition());
			if (distance < minDistance) {
				shootedEnemy = *enemyIt;
			}
		}
		if (shootedEnemy != nullptr) {
			auto texture = Director::getInstance()->getTextureCache()->getTextureForKey("explosion.png");
			auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 188.8, 212.5)));

			auto temp = shootedEnemy;
			shootedEnemy->runAction(
				Sequence::create(
					Animate::create(
						Animation::createWithSpriteFrames(explore, 0.04f, 1)
					),
					CallFunc::create([temp, this] {
						temp->removeFromParentAndCleanup(true);
						enemys.remove(temp);
					}),
					nullptr
				)
			);

			SimpleAudioEngine::getInstance()->playEffect("music/explore.wav", false);
			(*bulletIt)->removeFromParentAndCleanup(true);
			bulletIt = bullets.erase(bulletIt);			
		} else {
			bulletIt++;
		}
	}
}

void Thunder::stopAc() {
	auto temp = player;
	player->runAction(
		Sequence::create(
			Animate::create(
				Animation::createWithSpriteFrames(explore, 0.05f, 1)
			),
			CallFunc::create([temp] {
				temp->removeFromParentAndCleanup(true);
			}),
			nullptr
		)
	);
	SimpleAudioEngine::getInstance()->playEffect("music/explore.wav", false);

	auto gameover = Sprite::create("gameOver.png");
	gameover->setPosition(Director::getInstance()->getVisibleSize().width / 2,
		Director::getInstance()->getVisibleSize().height / 2);
	this->addChild(gameover, 1);
	
	unscheduleAllSelectors();
	_eventDispatcher->removeAllEventListeners();
}

void Thunder::updateOnce(float f) {
	
}

// 添加自定义监听器
void Thunder::addCustomListener() {
	// Todo
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

// 添加键盘事件监听器
void Thunder::addKeyboardListener() {
	// Todo
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey = 'A';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey = 'D';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		fire();
		break;
	}
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
		isMove = false;
		break;
	}
}

// 添加触摸事件监听器
void Thunder::addTouchListener() {
	// Todo
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// 鼠标点击发射炮弹
bool Thunder::onTouchBegan(Touch *touch, Event *event) {
	if (touch->getLocation().getDistance(player->getPosition()) <= 30)
	{
		isClick = true;
	}
	if (isClick) 
	{
		fire();
		schedule(schedule_selector(Thunder::autoFire), 0.2f, kRepeatForever, 0);
	}
	return true;
}

void Thunder::onTouchEnded(Touch *touch, Event *event) {
	isClick = false;
	unschedule(schedule_selector(Thunder::autoFire));
}

// 当鼠标按住飞船后可控制飞船移动 (加分项)
void Thunder::onTouchMoved(Touch *touch, Event *event) {
	if (isClick) {
		player->setPosition(touch->getLocation().x, player->getPosition().y);
	}
}
