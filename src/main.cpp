#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "nodes/CustomObjectCard.h"

using namespace geode::prelude;
using namespace cocos2d;

class MyPopup : public geode::Popup<>
{
	void nothing(CCObject*) {}

protected:
	bool setup() override
	{
		auto winSize = CCDirector::sharedDirector()->getWinSize();
		m_closeBtn->setPosition(m_size.width / 2 + 3.f, m_size.height / 2 - 3.f);

		// convenience function provided by Popup 
		// for adding/setting a title to the popup
		this->setTitle("Hi mom!");
		
		//esto esta mal por ahora
		//constexpr int sizeInBetween = 10;
		//constexpr int perRow = 3;

		//auto btn = CustomObjectCard::create({}, nullptr, nullptr);
		//m_buttonMenu->addChild(btn);

		auto buttonMenu = CCMenu::create();
		buttonMenu->setLayout(ColumnLayout::create());
		m_mainLayer->addChild(buttonMenu);

		for (int i = 0; i < 5; i++)
		{
			auto spr = SearchButton::create("GJ_longBtn04_001.png", "Trending", 0.5f, "GJ_sTrendingIcon_001.png");
			spr->setScale(0.7f);
			auto btn = CCMenuItemSpriteExtra::create(spr, nullptr, this, menu_selector(MyPopup::nothing));
			buttonMenu->addChild(btn);
		}

		buttonMenu->updateLayout();
		constexpr float spaceFromLeft = 10.0f;
		buttonMenu->setPositionX(buttonMenu->getPositionX() - (m_size.width / 2) + (buttonMenu->getContentSize().width / 2) + spaceFromLeft);

		log::info("x: {}", m_size.width);
		return true;
	}

public:
	static MyPopup* create()
	{
		auto ret = new MyPopup();
		if (ret && ret->init(400.f, 285.f))
		{
			ret->autorelease();
			return ret;
		}
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
};


class $modify(MenuLayer)
{
	void onMoreGames(CCObject*)
	{
		//CCDictionary* customObjects = GameManager::sharedState()->m_customObjectDict;
		//CCDictElement* pElement;
		//CCDICT_FOREACH(customObjects, pElement)
		//{
		//	const char* key = pElement->getStrKey();
		//	CCString* str = (CCString*)pElement->getObject();
		//	log::info("key: {}, str:{}\n", key, str->getCString());
		//}
		MyPopup::create()->show();
	} 
};
