#include "CustomObjectView.h"
#include <array>
#include <utility>
#include <Geode/modify/EditorUI.hpp>
#include <fmt/format.h>
#include "CustomObjectCard.h"


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

	//auto preview = CCLabelBMFont::create("Object preview here", "bigFont.fnt");
	//preview->setScale(.6f);
	//preview->setPosition(center + CCPoint(0.0f, 65.0f));
	//m_mainLayer->addChild(preview);

	if (LevelEditorLayer::get())
	{
		auto prev = CustomObjectCard::getCustomObjectPreview(_data->object_string);
		prev->setScale(prev->getScale() * 2);
		prev->setPosition(center + CCPoint(0.0f, 65.0f));
		m_mainLayer->addChild(prev);
	}

	auto btnMenu = CCMenu::create();
	btnMenu->setContentSize({325.0f, 75.0f});
	btnMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setAutoScale(false));
	btnMenu->setPosition(center - CCPoint(0.0f, 35.0f));
	m_mainLayer->addChild(btnMenu);


	auto buttons = [this]() -> std::vector<std::pair<const char*, SEL_MenuHandler>> {
			return
			{
				//{"Add Editor", menu_selector(CustomObjectView::onEditor)},
				{"Add to Custom Objects", menu_selector(CustomObjectView::onCustomObjects)},
				//{"Favorites", menu_selector(CustomObjectView::onComingSoon)},
				{"Website", menu_selector(CustomObjectView::onWebsite)},
				//{"Rate", menu_selector(CustomObjectView::onComingSoon)},
				//{"Save as file", menu_selector(CustomObjectView::onSaveJson)},
				{"Copy string to clipboard", menu_selector(CustomObjectView::onCopyStringClipboard)},
				{"Close", menu_selector(CustomObjectView::onClose)}
			};
	};


	for (const auto& [text, callback] : buttons())
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

static void CopyToClipboard(std::string_view text) {
	// Open the clipboard
	if (!OpenClipboard(nullptr)) {
		// Handle error if clipboard cannot be opened
		// You can add appropriate error handling here
		return;
	}

	// Empty the clipboard
	if (!EmptyClipboard()) {
		// Handle error if clipboard cannot be emptied
		// You can add appropriate error handling here
		CloseClipboard();
		return;
	}

	// Get the length of the text
	const size_t textLength = text.length();

	// Allocate global memory to hold the text
	HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, (textLength + 1) * sizeof(char));
	if (hClipboardData == nullptr) {
		// Handle error if memory allocation fails
		// You can add appropriate error handling here
		CloseClipboard();
		return;
	}

	// Lock the global memory handle and get a pointer to the memory
	char* pClipboardData = static_cast<char*>(GlobalLock(hClipboardData));
	if (pClipboardData == nullptr) {
		// Handle error if memory locking fails
		// You can add appropriate error handling here
		GlobalFree(hClipboardData);
		CloseClipboard();
		return;
	}

	// Copy the text to the global memory
	memcpy(pClipboardData, text.data(), textLength * sizeof(char));

	// Null-terminate the text
	pClipboardData[textLength] = '\0';

	// Unlock the global memory
	GlobalUnlock(hClipboardData);

	// Set the clipboard data
	if (!SetClipboardData(CF_TEXT, hClipboardData)) {
		// Handle error if clipboard data cannot be set
		// You can add appropriate error handling here
		GlobalFree(hClipboardData);
		CloseClipboard();
		return;
	}

	// Close the clipboard
	CloseClipboard();
}

void CustomObjectView::onCopyStringClipboard(CCObject*)
{
	CopyToClipboard(_data->object_string);
}
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
