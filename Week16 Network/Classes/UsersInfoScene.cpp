#include "UsersInfoScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"

using namespace cocos2d::network;
using namespace rapidjson;

cocos2d::Scene * UsersInfoScene::createScene() {
	return UsersInfoScene::create();
}

bool UsersInfoScene::init() {
	if (!Scene::init()) return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto getUserButton = MenuItemFont::create("Get User", CC_CALLBACK_1(UsersInfoScene::getUserButtonCallback, this));
	if (getUserButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + getUserButton->getContentSize().height / 2;
		getUserButton->setPosition(Vec2(x, y));
	}

	auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
		Director::getInstance()->popScene();
	});
	if (backButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
		backButton->setPosition(Vec2(x, y));
	}

	auto menu = Menu::create(getUserButton, backButton, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	limitInput = TextField::create("limit", "arial", 24);
	if (limitInput) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - 100.0f;
		limitInput->setPosition(Vec2(x, y));
		this->addChild(limitInput, 1);
	}

	messageBox = Label::create("", "arial", 30);
	if (messageBox) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height / 2;
		messageBox->setPosition(Vec2(x, y));
		this->addChild(messageBox, 1);
	}

	return true;
}

void UsersInfoScene::getUserButtonCallback(Ref * pSender) {
	// Your code here
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::GET);
	std::string url = "127.0.0.1:8000/users?limit=" + limitInput->getString();
	request->setUrl(url);
	request->setResponseCallback(
		CC_CALLBACK_2(UsersInfoScene::onQueryOK, this)
	);

	HttpClient::getInstance()->send(request);
	request->release();
}

void UsersInfoScene::onQueryOK(HttpClient* sender, HttpResponse* response) {
	if (!response) return;
	if (!response->isSucceed())
	{
		log("Error happen when \'GET\'");
		log("Error buffer : %s", response->getErrorBuffer());
		return;
	}
	log("Http response for \'GET\': \n");
	std::vector<char> *buffer = response->getResponseData();
	std::string res = vector2string(buffer);
	log(res.c_str());
	std::string resultStr = "";
	rapidjson::Document doc;
	doc.Parse(buffer->data(), buffer->size());
	if (doc["status"] == true) {
		rapidjson::Value & datanode = doc["data"];
		if (datanode.IsArray()) {
			for (int i = 0; i < datanode.Size(); ++i) {
				//messageBox->setString(res.c_str());
				resultStr = resultStr + "username: " + datanode[i]["username"].GetString() +
					'\n' + "deck: \n";
				rapidjson::Value & decknode = (doc["data"])[i]["deck"];
				for (int k = 0; k < decknode.Size(); k++)
				{
					for (rapidjson::Value::ConstMemberIterator itr = decknode[k].MemberBegin(); itr != decknode[k].MemberEnd(); itr++)
					{
						rapidjson::Value jKey;
						rapidjson::Value jValue;
						Document::AllocatorType allocator;
						jKey.CopyFrom(itr->name, allocator);
						jValue.CopyFrom(itr->value, allocator);
						if (jKey.IsString())
						{
							std::string name = jKey.GetString();
							int value = jValue.GetInt();
							std::stringstream stream;
							stream << value;
							std::string values = stream.str();
							resultStr = resultStr + "     " + name + ':' + values + '\n';
						}
					}
					resultStr += "     _ _ _\n";
				}
				resultStr += "_ _ _\n";
			}
		}
		messageBox->setString(resultStr);
	}
	else {
		this->messageBox->setString(std::string("Failure ") + doc["msg"].GetString());
	}
}
