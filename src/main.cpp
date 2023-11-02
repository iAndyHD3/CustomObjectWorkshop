#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "nodes/CustomObjectCard.h"
#include <Geode/utils/web.hpp>
#include <json.hpp>

using namespace geode::prelude;
using namespace cocos2d;

class MyPopup : public geode::Popup<>
{
	CCMenu* cardMenu = nullptr;

	void nothing(CCObject* /*sender*/) {}

protected:
	void handleResponse(std::string_view resp)
	{
		geode::log::info("resp: {}", resp);
		if (!cardMenu) return;

		try
		{
			json::Value jsonResp = json::parse(resp);
			cardMenu->removeAllChildrenWithCleanup(true);
			for (const json::Value& j : jsonResp["data"].as_array())
			{
				cardMenu->addChild(CustomObjectCard::create(
					{.objectData = j.as<CustomObjectData>(), .editor = nullptr}, nullptr, nullptr));
			}
			cardMenu->updateLayout();
		}
		catch (std::exception& e)
		{
			geode::log::error("CATCHED: {}", e.what());
		}
	}

	void addCards(int page)
	{
		web::AsyncWebRequest()
			.fetch(std::format("https://hyperbolus.net/api/stencils?page={}", page))
			.text()
			.then([this](const std::string& r) { handleResponse(r); });
	}
	bool setup() override
	{
		auto winSize = CCDirector::sharedDirector()->getWinSize();
		m_closeBtn->setPosition(m_size.width / 2 + 3.f, m_size.height / 2 - 3.f);

		this->setTitle("Custom Object Workshop");

		auto buttonMenu = CCMenu::create();
		buttonMenu->setLayout(ColumnLayout::create());
		m_mainLayer->addChild(buttonMenu);

		auto getBtn = [this](const char* title) {
			auto on	 = SearchButton::create("GJ_longBtn02_001.png", title, 0.5f, nullptr);
			auto off = SearchButton::create("GJ_longBtn01_001.png", title, 0.5f, nullptr);
			on->setScale(0.7f);
			off->setScale(0.7f);
			return CCMenuItemToggler::create(off, on, nullptr, nullptr);
		};

		buttonMenu->addChild(getBtn("trending"));
		buttonMenu->addChild(getBtn("recent"));
		buttonMenu->addChild(getBtn("favorite"));

		auto alignLeft = [](CCNode* node, float posX, float offset) {
			node->setPositionX(node->getPositionX() - (posX / 2) + (node->getContentSize().width / 2) + offset);
		};

		buttonMenu->updateLayout();

		constexpr float btnOffsetX = 10.0f;
		alignLeft(buttonMenu, m_size.width, btnOffsetX);

		cardMenu = CCMenu::create();
		cardMenu->setContentSize({350, 80});
		cardMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setGap(10.0f));

		this->addCards(0);

		cardMenu->updateLayout();
		m_mainLayer->addChild(cardMenu);
		cardMenu->setPositionX(cardMenu->getPositionX() + 35.0f);
		cardMenu->setPositionY(cardMenu->getPositionY() - 5.0f);

		return true;
	}

public:
	static MyPopup* create()
	{
		auto ret = new MyPopup();
		if (ret && ret->init(430.0f, 310.0f))
		{
			ret->autorelease();
			return ret;
		}
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
};

struct MenuLayerExt : geode::Modify<MenuLayerExt, MenuLayer>
{
	void onMoreGames(CCObject*)
	{
		// std::exit(1);
		// CCDictionary* customObjects = GameManager::sharedState()->m_customObjectDict;
		// CCDictElement* pElement;
		// CCDICT_FOREACH(customObjects, pElement)
		//{
		//	const char* key = pElement->getStrKey();
		//	CCString* str = (CCString*)pElement->getObject();
		//	log::info("key: {}, str:{}\n", key, str->getCString());
		// }
		MyPopup::create()->show();
	}
};

struct EditorUIExt : geode::Modify<EditorUIExt, EditorUI>
{
	void onPlayback(CCObject*) { MyPopup::create()->show(); }
};
