#include "CustomObjectView.h"
#include <array>
#include <utility>
#include <Geode/modify/EditorUI.hpp>

using namespace cocos2d;

bool CustomObjectView::setup(CustomObjectData* data)
{
	_data = data;

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

	constexpr auto buttons = std::to_array<std::pair<const char*, SEL_MenuHandler>>(
		{
			{"Add Editor", menu_selector(CustomObjectView::onEditor)},
			{"Add Custom Objects", menu_selector(CustomObjectView::onCustomObjects)},
			//{"Favorites", menu_selector(CustomObjectView::onComingSoon)},
			{"Website", menu_selector(CustomObjectView::onWebsite)},
			//{"Rate", menu_selector(CustomObjectView::onComingSoon)},
			//{"Save as file", menu_selector(CustomObjectView::onSaveJson)},
			//{"Copy json", menu_selector(CustomObjectView::onCopyJson)}
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

void CustomObjectView::onWebsite(CCObject*)
{
	CCApplication::sharedApplication()->openURL(
		fmt::format("{}/{}", "https://hyperbolus.net/stencil", _data->object_id).c_str());
}
void CustomObjectView::onRate(CCObject*) {}
void CustomObjectView::onFavorites(CCObject*) {}
void CustomObjectView::onEditor(CCObject*) {}
void CustomObjectView::onCustomObjects(CCObject*)
{
	auto gm = GameManager::get();

	CCDictionary* customObjects = gm->m_customObjectDict;
	unsigned int count			= customObjects->count();

	gm->addNewCustomObject(_data->object_string);

	if (auto* ui = EditorUI::get())
	{
		ui->reloadCustomItems();
	}

	if (count + 1 == customObjects->count())
	{
		auto notification = geode::Notification::create(fmt::format("Added {} by {}", _data->name, _data->author.name));
		notification->setIcon(geode::NotificationIcon::Success);
		notification->show();
	}
}

void CustomObjectView::onCopyJson(CCObject*) {}
void CustomObjectView::onSaveJson(CCObject*) {}

void CustomObjectView::onComingSoon(CCObject*)
{
	auto n = geode::Notification::create("Action coming soon");
	n->setIcon(geode::NotificationIcon::Error);
	n->show();
}

CustomObjectView* CustomObjectView::create(CustomObjectData* data)
{
	auto ret = new CustomObjectView();
	if (ret && ret->init(350.0f, 250.0f, data, "GJ_square02.png"))
	{
		ret->autorelease();
		return ret;
	}

	ret = nullptr;
	return nullptr;
}
