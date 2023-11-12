#include "WorkshopPopup.h"
#include "CustomObjectCard.h"
#include "CustomObjectView.h"
#include "json.hpp"
#include <Geode/utils/web.hpp>
#include <fmt/format.h>

using namespace cocos2d;

// clang-format off
bool WorkshopPopup::addCard(const json::Value& j)
{
	if (!_cardMenu) return false;
	
	auto card = CustomObjectCard::create(j.as<CustomObjectData>(), this, menu_selector(WorkshopPopup::onCard));
	if(!card) return false;

	_cardMenu->addChild(card);
	return true;
}
// clang-format on

bool WorkshopPopup::setup()
{
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	m_closeBtn->setPosition(m_size.width / 2 + 3.f, m_size.height / 2 - 3.f);

	this->setTitle("Custom Object Workshop");

	auto buttonMenu = CCMenu::create();
	buttonMenu->setLayout(ColumnLayout::create());
	m_mainLayer->addChild(buttonMenu);

	auto getBtn = [this](const char* title) {
		auto on	 = SearchButton::create("GJ_longBtn02_001.png", title, 0.5f, nullptr);
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

	_cardMenu = CCMenu::create();
	_cardMenu->setContentSize({350, 260});
	_cardMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setGap(10.0f));

	this->openPage(0, 6);

	m_mainLayer->addChild(_cardMenu);
	_cardMenu->setPositionX(_cardMenu->getPositionX() + 35.0f);
	_cardMenu->setPositionY(_cardMenu->getPositionY() + 10.0f);

	return true;
}

void WorkshopPopup::updatePageButtons()
{
	constexpr float buttonScale = 0.6f;

	if (!_selectPageMenu)
	{
		_selectPageMenu = CCMenu::create();
		_selectPageMenu->setLayout(RowLayout::create());
		CCPoint pos = _cardMenu->getPosition();
		pos.y -= _cardMenu->getContentSize().height / 2 + 17;

		_selectPageMenu->setPosition(pos);
		m_mainLayer->addChild(_selectPageMenu);
	}

	auto addButton =
		[this](CCMenuItemSpriteExtra** btn, CCSprite* spr, SEL_MenuHandler callback, bool updateLayout = false) {
		spr->setScale(buttonScale);
		*btn = CCMenuItemSpriteExtra::create(spr, nullptr, this, callback);
		_selectPageMenu->addChild(*btn);
		if (updateLayout)
		{
			_selectPageMenu->updateLayout();
		}
	};

	if (!_prevBtn)
	{
		addButton(
			&_prevBtn, CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
			menu_selector(WorkshopPopup::onPrevious));
	}
	_prevBtn->setVisible(_currentPage != 1);

	if (!_currentPageBtn)
	{
		auto spr = ButtonSprite::create(fmt::format("{}", _currentPage).c_str(), "bigFont.fnt", "GJ_button_02.png");
		addButton(&_currentPageBtn, spr, menu_selector(WorkshopPopup::onCard));
	}
	else
	{
		auto spr = geode::cast::safe_cast<ButtonSprite*>(_currentPageBtn->getChildren()->objectAtIndex(0));
		if (spr) spr->setString(fmt::format("{}", _currentPage).c_str());
	}

	if (!_nextBtn)
	{
		auto spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
		spr->setFlipX(true);
		addButton(&_nextBtn, spr, menu_selector(WorkshopPopup::onNext), true);
	}
	else
	{
		_nextBtn->setVisible(_currentPage != _maxPage);
	}
}

void WorkshopPopup::onNext(cocos2d::CCObject*)
{
	int nextPage = _currentPage + 1;
	openPage(nextPage, 6);
}

void WorkshopPopup::onPrevious(cocos2d::CCObject*) {}

void WorkshopPopup::onCard(cocos2d::CCObject* sender)
{
	if (auto card = geode::cast::safe_cast<CustomObjectCard*>(sender))
	{
		CustomObjectView::create(&card->_objectData)->show();
	}
}

void WorkshopPopup::openPage(int page, int perPage)
{
	_cardMenu->removeAllChildrenWithCleanup(true);

	geode::utils::web::AsyncWebRequest()
		.fetch(fmt::format("https://hyperbolus.net/api/stencils?page={}&perPage={}", page, perPage))
		.text()
		.then([this](const std::string& r) { handleResponse(r); });
}

void WorkshopPopup::handleResponse(std::string_view resp)
{
	geode::log::info("resp: {}", resp);
	if (!_cardMenu) return;

	try
	{
		json::Value jsonResp = json::parse(resp);
		for (const json::Value& j : jsonResp["data"].as_array())
		{
			this->addCard(j);
		}
		_currentPage = jsonResp["current_page"].as_int();
		_maxPage     = jsonResp["last_page"].as_int();
	}
	catch (std::exception& e)
	{
		geode::log::error("CATCHED: {}", e.what());
		return;
	}

	_cardMenu->updateLayout();
	updatePageButtons();
}

void WorkshopPopup::fillEmpty()
{
	if (!_cardMenu) return;
	while (_cardMenu->getChildrenCount() < 5)
	{
		addEmptyCard(false);
	}
}

bool WorkshopPopup::addEmptyCard(bool visible)
{
	if (!_cardMenu) return false;
	auto card = CustomObjectCard::create({}, this, menu_selector(WorkshopPopup::onCard));
	if (!card) return false;

	card->setVisible(visible);
	_cardMenu->addChild(card);
	return true;
}

WorkshopPopup* WorkshopPopup::create()
{
	auto ret = new WorkshopPopup();
	if (ret && ret->init(430.0f, 295.0f))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}
