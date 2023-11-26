#include "CustomObjectCard.h"
#include "cocos2d.h"
#include <fmt/format.h>


using namespace cocos2d;
using geode::cocos::CCArrayExt;


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

CCSprite* CustomObjectCard::getCustomObjectPreview(std::string_view objectString)
{

	auto editor = LevelEditorLayer::get();
	CCArray* objects = editor->createObjectsFromString(std::string(objectString), true);
	auto v8 = CCSprite::create();

	CCPoint originPos{ 1000.0f, 1000.0f };

	EditorUI::get()->repositionObjectsToCenter(objects, originPos, true);

	int count = objects->count();

	float v14 = 0.0f;
	float v10 = 0.0f;
	float v12 = 0.0f;
	float v13 = 0.0f;

	for (int i = 0; i < count; i++)
	{
		auto obj = reinterpret_cast<GameObject*>(objects->objectAtIndex(i));
		editor->removeObject(obj, true);
		obj->setPosition(obj->getPosition() - originPos);
		v8->addChild(obj);

		auto& texture = obj->getObjectTextureRect();
		float minX = texture.getMinY();
		float maxX = texture.getMaxX();
		float minY = texture.getMinY();
		float maxY = texture.getMaxY();

		bool v28 = v14 == 0.0f;
		if (v14 == 0.0) v14 = minX;

		if (!v28 && minX < v14) v14 = minX;

		if (v13 == 0.0)
		{
			v13 = maxX;
		}
		else if (maxX > v13)
		{
			v13 = maxX;
		}
		if (v12 == 0.0)
		{
			v12 = minY;
		}
		else if (minY < v12)
		{
			v12 = minY;
		}
		if (v10 == 0.0)
		{
			v10 = maxY;
		}
		else if (maxY > v10)
		{
			v10 = maxY;
		}
	}

	v8->setContentSize({ v13 - v14, v10 - v12 });
	for (int i = 0; i < count; i++)
	{
		auto object = reinterpret_cast<GameObject*>(objects->objectAtIndex(i));
		object->setPosition(v8->convertToNodeSpace(object->getPosition()));
	}
	auto& contentSize = v8->getContentSize();

	float sizeDiv = contentSize.width <= contentSize.height ? contentSize.height : contentSize.width;
	v8->setScale(40.0f / sizeDiv);
	return v8;
}

void CustomObjectCard::onUploadObject(cocos2d::CCObject*)
{
	geode::createQuickPopup(
		"Upload object",
		"You need a <cr>Hyperbolus account</c> to upload an object, open the upload website?",
		"Open",
		"Cancel",
		[this](FLAlertLayer*, bool btn2)
		{
			if (!btn2)
			{
				std::string url = fmt::format("https://hyperbolus.net/stencils/new/interstitial?ref=2465#{}", _objectData.object_string);
				CCApplication::sharedApplication()->openURL(url.c_str());
			}
		},
		true);
}



bool CustomObjectCard::init(CCObject* target, SEL_MenuHandler callback)
{
	// CCRect bgRect {0, 0, 80, 80};
	CCSize bgSize{100, 115};

	auto* m_bgSprite = cocos2d::extension::CCScale9Sprite::create("GJ_square05.png");
	m_bgSprite->setContentSize(bgSize);
	if (!m_bgSprite) return false;

	if (_objectData.local)
	{
		if (!CCMenuItemSpriteExtra::init(m_bgSprite, nullptr, this, menu_selector(CustomObjectCard::onUploadObject)))
			return false;
	}
	else
	{
		if (!CCMenuItemSpriteExtra::init(m_bgSprite, nullptr, target, callback))
			return false;
	}

	m_bgSprite->setAnchorPoint({0.5f, 0.5f});

	icons_bg = CCSprite::create("square02_001.png");
	this->addChild(icons_bg);
	icons_bg->setPosition({bgSize.width / 2, bgSize.width - 26});
	icons_bg->setScaleY(0.775f);
	icons_bg->setOpacity(50);

	if (LevelEditorLayer::get())
	{
		auto spr = getCustomObjectPreview(_objectData.object_string);
		spr->setPosition(icons_bg->getPosition());
		this->addChild(spr);
	}

	if (!_objectData.name.empty())
	{
		auto nameLabel = CCLabelBMFont::create(_objectData.name.c_str(), "bigFont.fnt");
		nameLabel->limitLabelWidth(90.0f, 0.5f, 0.0f);
		nameLabel->setPosition(icons_bg->getPosition() - CCPoint(0.0f, 40.0f));
		addChild(nameLabel);
	}



	if (!_objectData.author.name.empty())
	{
		auto authorLabel = CCLabelBMFont::create(_objectData.author.name.c_str(), "goldFont.fnt");
		authorLabel->limitLabelWidth(90.0f, 0.5f, 0.0f);
		authorLabel->setPosition(icons_bg->getPosition() - CCPoint(0.0f, 55.0f));
		addChild(authorLabel);
	}

	return true;
}