#include "CustomObjectView.h"
#include <array>
#include <utility>

using namespace cocos2d;

bool CustomObjectView::setup(CustomObjectData* data)
{
	auto winSize = CCDirector::sharedDirector()->getWinSize();

	// convenience function provided by Popup
	// for adding/setting a title to the popup
	this->setTitle(data->name.c_str(), "bigFont.fnt");

	auto authorLabel = CCLabelBMFont::create(data->author.name.c_str(), "goldFont.fnt");
	authorLabel->setScale(m_title->getScale());
	authorLabel->setPosition(m_title->getPosition() - CCPoint(0.0f, 20.0f));
	m_mainLayer->addChild(authorLabel);

	CCPoint center{winSize.width / 2, winSize.height / 2 - 50.0f};

	auto floor = CCSprite::createWithSpriteFrameName("floorLine_001.png");
	floor->setPosition(center);
	floor->setScale(0.8f);
	m_mainLayer->addChild(floor);

	auto preview = CCLabelBMFont::create("Object preview here", "bigFont.fnt");
	preview->setScale(.6f);
	preview->setPosition(center + CCPoint(0.0f, 65.0f));
	m_mainLayer->addChild(preview);

	auto btnMenu = CCMenu::create();
	btnMenu->setContentSize({325.0f, 75.0f});
	btnMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setAutoScale(false));
	btnMenu->setPosition(center - CCPoint(0.0f, 35.0f));
	m_mainLayer->addChild(btnMenu);

	constexpr auto buttons = std::to_array<std::pair<const char*, SEL_MenuHandler>>
	({
		{"Add Editor", nullptr},
		{"Add Custom Objects", nullptr},
		{"Favorites", nullptr},
		{"Website", nullptr},
		{"Rate", nullptr},
		{"Save as file", nullptr},
		{"Copy json", nullptr}
	});

	for (const auto& [text, callback] : buttons)
	{
		auto spr = ButtonSprite::create(text);
		spr->setScale(.5f);
		auto btn = CCMenuItemSpriteExtra::create(spr, nullptr, this, callback);
		btnMenu->addChild(btn);
	}

	btnMenu->updateLayout();





	return true;
}


CustomObjectView* CustomObjectView::create(CustomObjectData* data)
{
	auto ret = new CustomObjectView();
	if(ret && ret->init(350.0f, 250.0f, data, "GJ_square02.png"))
	{
		ret->autorelease();
		return ret;
	}

	ret = nullptr;
	return nullptr;
}
