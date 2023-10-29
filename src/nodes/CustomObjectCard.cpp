#include "CustomObjectCard.h"
#include "cocos2d.h"

using namespace cocos2d;

CustomObjectCard* CustomObjectCard::create(const CustomObjectCardMembers& data, CCObject* target, SEL_MenuHandler callback)
{
	auto* ret = new CustomObjectCard(data);
	if(ret && ret->init(target, callback))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool CustomObjectCard::init(CCObject* target, SEL_MenuHandler callback)
{
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	
	CCRect bgRect {0, 0, 80, 80};
	CCSize bgSize {m.x, m.y};
	
	auto* m_bgSprite = cocos2d::extension::CCScale9Sprite::create("GJ_square05.png", bgRect);
	if (!m_bgSprite) return false;


	if (!CCMenuItemSpriteExtra::init(m_bgSprite, nullptr, target, callback)) return false;
	
	m_bgSprite->setAnchorPoint({ 0.5f, 0.5f });
	return true;
}

