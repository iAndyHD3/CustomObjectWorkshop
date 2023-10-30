#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "nodes/CustomObjectCard.h"

using namespace geode::prelude;
using namespace cocos2d;

class MyPopup : public geode::Popup<>
{
	void nothing(CCObject*)
	{
	}

protected:
	bool setup() override
	{
		auto winSize = CCDirector::sharedDirector()->getWinSize();
		m_closeBtn->setPosition(m_size.width / 2 + 3.f, m_size.height / 2 - 3.f);

		this->setTitle("Custom Object Workshop");

		auto buttonMenu = CCMenu::create();
		buttonMenu->setLayout(ColumnLayout::create());
		m_mainLayer->addChild(buttonMenu);

		auto getBtn = [this](const char* title) {
			auto on = SearchButton::create("GJ_longBtn02_001.png", title, 0.5f, nullptr);
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

		auto cardMenu = CCMenu::create();
		cardMenu->setContentSize({ 350, 80 });
		cardMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setGap(10.0f));

		for (int i = 0; i < 6; i++)
		{
			auto card = CustomObjectCard::create({}, this, menu_selector(MyPopup::nothing));
			cardMenu->addChild(card);
		}
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


class $modify(MenuLayer)
{
	void onMoreGames(CCObject*)
	{
		//std::exit(1);
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
