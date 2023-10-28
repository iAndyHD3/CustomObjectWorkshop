#include "CustomObjectCard.h"

using namespace cocos2d;

CustomObjectCard* CustomObjectCard::create(const CustomObjectCardMembers& data)
{
	auto* ret = new CustomObjectCard(data);
	if(ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool CustomObjectCard::init()
{
	if(!CCNode::init()) return false;
	
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	
	CCRect bgRect {0, 0, 80, 80};
	CCSize bgSize {m.x, m.y};
	
	auto m_bgSprite = cocos2d::extension::CCScale9Sprite::create("GJ_square05.png", bgRect);
	m_bgSprite->setContentSize(bgSize);
	//m_bgSprite->setPosition(winSize.width / 2, winSize.height / 2);
	addChild(m_bgSprite);
			
	return true;
}

