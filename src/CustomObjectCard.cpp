#include "CustomObjectCard.h"
#include "cocos2d.h"

using namespace cocos2d;
using geode::cocos::CCArrayExt;

CCSprite* EditorUI_menuItemFromObjectString(std::string_view objectString);

CustomObjectCard* CustomObjectCard::create(const CustomObjectData& data, CCObject* target, SEL_MenuHandler callback)
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
	this->addChild(icons_bg);
	icons_bg->setPosition({bgSize.width / 2, bgSize.width - 26});
	icons_bg->setScaleY(0.775f);
	icons_bg->setOpacity(50);

	if (LevelEditorLayer::get())
	{
		auto spr = EditorUI_menuItemFromObjectString(_objectData.object_string);
		spr->setPosition(icons_bg->getPosition());
		this->addChild(spr);
	}


	geode::log::info("name: {}, author: {}", _objectData.name, _objectData.author.name);


	const char* label = _objectData.name.empty() && _objectData.local ? "Text" : _objectData.name.c_str();
	auto nameLabel = CCLabelBMFont::create(label, "bigFont.fnt");
	nameLabel->limitLabelWidth(90.0f, 0.5f, 0.0f);
	nameLabel->setPosition(icons_bg->getPosition() - CCPoint(0.0f, 40.0f));
	addChild(nameLabel);


	const char* label2 = _objectData.author.name.empty() && _objectData.local ? "Text" : _objectData.author.name.c_str();
	auto authorLabel = CCLabelBMFont::create(label2, "goldFont.fnt");
	authorLabel->limitLabelWidth(90.0f, 0.5f, 0.0f);
	authorLabel->setPosition(icons_bg->getPosition() - CCPoint(0.0f, 55.0f));
	addChild(authorLabel);

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
