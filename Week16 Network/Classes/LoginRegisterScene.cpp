#include "LoginRegisterScene.h"
#include "ui\CocosGUI.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "json\stringbuffer.h"
#include "json\writer.h"
#include "Utils.h"

USING_NS_CC;
using namespace cocos2d::network;
using namespace cocos2d::ui;
using namespace rapidjson;

cocos2d::Scene * LoginRegisterScene::createScene() {
	return LoginRegisterScene::create();
}

bool LoginRegisterScene::init() {
	if (!Scene::init()) {
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto loginButton = MenuItemFont::create("Login", CC_CALLBACK_1(LoginRegisterScene::loginButtonCallback, this));
	if (loginButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + loginButton->getContentSize().height / 2;
		loginButton->setPosition(Vec2(x, y));
	}

	auto registerButton = MenuItemFont::create("Register", CC_CALLBACK_1(LoginRegisterScene::registerButtonCallback, this));
	if (registerButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + registerButton->getContentSize().height / 2 + 100;
		registerButton->setPosition(Vec2(x, y));
	}

	auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
		Director::getInstance()->popScene();
	});
	if (backButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
		backButton->setPosition(Vec2(x, y));
	}

	auto menu = Menu::create(loginButton, registerButton, backButton, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	usernameInput = TextField::create("username", "arial", 24);
	if (usernameInput) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - 100.0f;
		usernameInput->setPosition(Vec2(x, y));
		this->addChild(usernameInput, 1);
	}

	passwordInput = TextField::create("password", "arial", 24);
	if (passwordInput) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - 130.0f;
		passwordInput->setPosition(Vec2(x, y));
		this->addChild(passwordInput, 1);
	}

	messageBox = Label::create("", "arial", 30);
	if (messageBox) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - 200.0f;
		messageBox->setPosition(Vec2(x, y));
		this->addChild(messageBox, 1);
	}

	return true;
}

void LoginRegisterScene::loginButtonCallback(cocos2d::Ref * pSender) {
	// Your code here
	std::string username = usernameInput->getStringValue();
	std::string password = passwordInput->getStringValue();

	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	std::string url = "127.0.0.1:8000/auth";
	request->setUrl(url);
	std::string postData = "{\"username\":\"" + username + "\",\"password\":\"" + password + "\"}";
	request->setRequestData(postData.c_str(), postData.size());
	request->setResponseCallback(
		CC_CALLBACK_2(LoginRegisterScene::onLoginRequestComplete1, this)
	);
	
	HttpClient::getInstance()->send(request);
	request->release();
}

void LoginRegisterScene::registerButtonCallback(Ref * pSender) {
	// Your code here
	std::string username = usernameInput->getString();
	std::string password = passwordInput->getString();
	
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	std::string url = "127.0.0.1:8000/users";
	request->setUrl(url);
	std::string postData = "{\"username\":\"" + username + "\",\"password\":\"" + password + "\"}";
	/*
	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
	document.AddMember("username", username, allocator);
	document.AddMember("password", password, allocator);
	
	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writer(buffer);
	document.Accept(writer);

	request->setRequestData(buffer.GetString(), strlen(buffer.GetString()));
	*/
	request->setRequestData(postData.c_str(), postData.size());
	request->setResponseCallback(
		CC_CALLBACK_2(LoginRegisterScene::onRegistRequestComplete, this)
	);

	HttpClient::getInstance()->send(request);
	request->release();

}

void LoginRegisterScene::onLoginRequestComplete1(HttpClient* sender, HttpResponse* response)
{
	if (!response) {
		messageBox->setString("Not ok on log in");
		return;
	}
	if (!response->isSucceed()) 
	{
		messageBox->setString("Not ok on log in");
		log("Error happen when \'GET\'");
		log("Error buffer : %s", response->getErrorBuffer());
		return;
	}
	
	log("Http response for \'GET\': \n");
	std::vector<char> *buffer = response->getResponseData();
	std::string res = vector2string(buffer);
	log(res.c_str());

	auto resHeader = response->getResponseHeader();
	std::string resh = vector2string(resHeader);
	log("\n\n\nshit!!!!!!!!!!!!!!!!!!");
	log(resh.c_str());

	rapidjson::Document doc;
	doc.Parse(buffer->data(), buffer->size());

	messageBox->setString(doc["msg"].GetString());
	return;

}

void LoginRegisterScene::onRegistRequestComplete(HttpClient * sender, HttpResponse * response)
{
	if (!response) {		
		messageBox->setString("Not ok on regist");
		return;
	}
	if (!response->isSucceed())
	{
		messageBox->setString("Not ok on regist");
		log("Error happen when \'POST\'");
		log("Error buffer : %s", response->getErrorBuffer());
		return;
	}
	log("Http response for \'POST\': \n");
	std::vector<char> *buffer = response->getResponseData();
	std::string res = vector2string(buffer);
	log(res.c_str());
	HttpClient::getInstance()->enableCookies(nullptr);

	rapidjson::Document doc;
	doc.Parse(buffer->data(), buffer->size());

	messageBox->setString(doc["msg"].GetString());
	return;
}
