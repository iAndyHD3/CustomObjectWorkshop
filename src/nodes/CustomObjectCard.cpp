#include "CustomObjectCard.h"
#include "cocos2d.h"

using namespace cocos2d;
using geode::cocos::CCArrayExt;

CustomObjectCard*
CustomObjectCard::create(const CustomObjectCardMembers& data, CCObject* target, SEL_MenuHandler callback)
{
	auto* ret = new CustomObjectCard(data);
	if (ret && ret->init(target, callback))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool CustomObjectCard::init(CCObject* target, SEL_MenuHandler callback)
{
	// CCRect bgRect {0, 0, 80, 80};
	CCSize bgSize{100, 115};

	auto* m_bgSprite = cocos2d::extension::CCScale9Sprite::create("GJ_square05.png");
	m_bgSprite->setContentSize(bgSize);
	if (!m_bgSprite) return false;
	if (!CCMenuItemSpriteExtra::init(m_bgSprite, nullptr, target, callback)) return false;

	m_bgSprite->setAnchorPoint({0.5f, 0.5f});

	icons_bg = CCSprite::create("square02_001.png");
	icons_bg->setPosition({bgSize.width / 2, bgSize.width - 26});
	icons_bg->setScaleY(0.775f);
	this->addChild(icons_bg);
	icons_bg->setOpacity(50);

	geode::log::info("name: {}, author: {}", m.objectData.name, m.objectData.author.name);

	return true;
}

// static void splitByDelimStringView(std::string_view str, char delim, auto callback)
//{
//	std::size_t len = str.length();
//
//	for (std::size_t pos = 0, end = 0; pos < len; pos = end + 1)
//	{
//		end = str.find(delim, pos);
//		if (end == std::string_view::npos)
//		{
//			callback(str.substr(pos));
//			break;
//		}
//		callback(str.substr(pos, end - pos));
//	}
// }

void CustomObjectCard::createObjects()
{
	// auto objContainer = CCSprite::create();

	// splitByDelimStringView(m.objectData.object_string, ';', [&](std::string_view str) {
	//     auto obj = GameObject::objectFromString(std::format("{}", str), false);
	//     if (obj)
	//     {
	//         objContainer->addChild(obj);
	//     }
	//     });
	// addChild(objContainer);
}
