#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <string_view>
#include "nodes/CustomObjectCard.h"


using namespace geode::prelude;
using namespace cocos2d;




class MyPopup : public geode::Popup<>
{
protected:
	bool setup() override
	{
		auto winSize = CCDirector::sharedDirector()->getWinSize();
	
		// convenience function provided by Popup 
		// for adding/setting a title to the popup
		this->setTitle("Hi mom!");
		
		//esto esta mal por ahora
		constexpr int sizeInBetween = 10;
		constexpr int perRow = 3;
		for(int i = 0; i < perRow; i++)
		{
			auto label = CustomObjectCard::create({});
			float necessaryX = (label->m.x + sizeInBetween) * perRow;
			float offsetX = m_size.width - necessaryX;
			
			CCPoint pos {(winSize.width / 2 - offsetX) + i * (label->m.x + sizeInBetween), winSize.height / 2};
			label->setPosition(pos);
			m_mainLayer->addChild(label);
		}
	
		//close button on the right side
		m_closeBtn->setPosition(m_size.width / 2 + 3.f, m_size.height / 2 - 3.f);
		
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
		CCDictionary* customObjects = GameManager::sharedState()->m_customObjectDict;
		CCDictElement* pElement;
		CCDICT_FOREACH(customObjects, pElement)
		{
			const char* key = pElement->getStrKey();
			CCString* str = (CCString*)pElement->getObject();
			log::info("key: {}, str:{}\n", key, str->getCString());
		}
		MyPopup::create()->show();
	} 
};
