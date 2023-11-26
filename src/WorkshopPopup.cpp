#include "WorkshopPopup.h"
#include "CustomObjectCard.h"
#include "CustomObjectView.h"
#include "json.hpp"
#include <Geode/utils/web.hpp>
#include <fmt/format.h>
#include <utility>


using namespace cocos2d;

bool WorkshopPopup::addCard(const json::Value& j)
{
	auto card = CustomObjectCard::create(j.as<CustomObjectData>(), this, menu_selector(WorkshopPopup::onCard));
	_cardMenu->addChild(card);
	return true;
}

bool WorkshopPopup::setup()
{
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	m_closeBtn->setPosition(m_size.width / 2 + 3.f, m_size.height / 2 - 3.f);

	// m_title->setVisible(false);
	// this->setTitle("Custom Object Workshop");

	_buttonMenu = CCMenu::create();
	_buttonMenu->setLayout(ColumnLayout::create());
	m_mainLayer->addChild(_buttonMenu);

	auto getBtn = [this](const char* title, const char* title2, float scale, SEL_MenuHandler callback = nullptr) {
		auto on	 = SearchButton::create("GJ_longBtn02_001.png", title, scale, nullptr);
		auto off = SearchButton::create("GJ_longBtn01_001.png", title2 ? title2 : title, scale, nullptr);
		on->setScale(0.7f);
		off->setScale(0.7f);
		return CCMenuItemToggler::create(off, on, this, callback);
	};

	// buttonMenu->addChild(getBtn("trending"));
	// buttonMenu->addChild(getBtn("recent"));
	// buttonMenu->addChild(getBtn("favorite"));

	_browseBtn = getBtn("Browse", nullptr, 0.6f, menu_selector(WorkshopPopup::onUpload));
	_myObjectsBtn = getBtn("My Objects", nullptr, 0.4f, menu_selector(WorkshopPopup::onLocalObject));

	_buttonMenu->addChild(_browseBtn);
	_buttonMenu->addChild(_myObjectsBtn);

	auto alignLeft = [](CCNode* node, float posX, float offset) {
		node->setPositionX(node->getPositionX() - (posX / 2) + (node->getContentSize().width / 2) + offset);
	};

	_buttonMenu->updateLayout();

	constexpr float btnOffsetX = 10.0f;
	alignLeft(_buttonMenu, m_size.width, btnOffsetX);

	_cardMenu = CCMenu::create();
	_cardMenu->setContentSize({350, 260});
	_cardMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setGap(10.0f)->setAxisAlignment(AxisAlignment::Start));
	//_cardMenu->setPositionX(_cardMenu->getPositionX() + 35.0f);
	//_cardMenu->setPositionY(_cardMenu->getPositionY() + 10.0f);
	_cardMenu->setPosition(winSize / 2 + CCPoint(48.0f, 10.0f));
	m_mainLayer->addChild(_cardMenu);

	_loadingCircle = LoadingCircle::create();
	_loadingCircle->setParentLayer(m_mainLayer);
	_loadingCircle->setPositionX(_loadingCircle->getPositionX() + 35.0f);
	_loadingCircle->show();

	this->openPageHyperBolus(getMainApiEndpoint(0));

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

		_selectPageMenu->setPosition(pos - CCPoint(13.0f, _cardMenu->getContentSize().height / 2 + 17));
		m_mainLayer->addChild(_selectPageMenu);
	}
	else
	{
		_selectPageMenu->setVisible(!_makingRequest);
	}


	auto addButton = [this](CCMenuItemSpriteExtra** btn, CCSprite* spr, SEL_MenuHandler callback, bool updateLayout = false)
	{
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
		auto spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
		addButton(&_prevBtn, spr, menu_selector(WorkshopPopup::onPrevious));
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
		_nextBtn->setVisible(_currentPage < _maxPage);
}

std::string WorkshopPopup::getMainApiEndpoint(int page)
{
	if (_uploadPage) page = 0;

	constexpr const char* endpoint = "https://hyperbolus.net/api/stencils?page={}&perPage={}";
	constexpr int perPage		   = 6;

	return fmt::format(endpoint, page, perPage);
}

void WorkshopPopup::onNext(cocos2d::CCObject*)
{
	if (_uploadPage)
	{
		openPageLocalObjects(_currentPage + 1);
	}
	else
	{
		openPageHyperBolus(getMainApiEndpoint(_currentPage + 1));
	}
}

void WorkshopPopup::onPrevious(cocos2d::CCObject*)
{
	if (_uploadPage)
	{
		openPageLocalObjects(_currentPage - 1);
	}
	else
	{
		openPageHyperBolus(getMainApiEndpoint(_currentPage - 1));
	}
}

void WorkshopPopup::onCard(cocos2d::CCObject* sender)
{
	if (auto card = geode::cast::safe_cast<CustomObjectCard*>(sender))
	{
		CustomObjectView::create(&card->_objectData)->show();
	}
}

std::vector<CCString*> getLocalCustomObjectStrings()
{
	std::vector<CCString*> ret;
	auto dict = geode::cocos::CCDictionaryExt<int, CCString>(GameManager::sharedState()->m_customObjectDict);
	for (const auto& [id, str] : dict)
	{
		ret.emplace_back(str);
	}

	return ret;
}

void WorkshopPopup::onUpload(cocos2d::CCObject* sender)
{
	if (_makingRequest) return;

	bool on = reinterpret_cast<CCMenuItemToggler*>(sender)->isOn();

	if (!on)
	{
		_selectPageMenu->setVisible(false);
		openPageHyperBolus(getMainApiEndpoint(0));
	}
	else
	{
		geode::Loader::get()->queueInMainThread([this] {
			reinterpret_cast<CCMenuItemToggler*>(_browseBtn)->toggle(true);
		});
	}
}

void WorkshopPopup::openPageHyperBolus(std::string_view apiurl)
{
	if (_makingRequest) return;

	_buttonMenu->setVisible(false);
	_makingRequest = true;

	_cardMenu->removeAllChildrenWithCleanup(true);
	_loadingCircle->setVisible(true);
	geode::log::info("Making api request: {}", apiurl);
	geode::utils::web::AsyncWebRequest()
		.fetch(std::string(apiurl))
		.text()
		.then([this](const std::string& r) {
		handleResponse(r);
	}).cancelled([this](auto)
	{
		_makingRequest = false;
		_buttonMenu->setVisible(true);
	});
}

void WorkshopPopup::updateMembers(const json::Value& resp)
{

	setCurrentPage(resp["current_page"].as_int());
	_maxPage	 = resp["last_page"].as_int();

	if (auto url = resp["next_page_url"]; url.is_string()) _nextPageUrl = url.as_string();
	else _nextPageUrl.clear();

	if (auto url = resp["prev_page_url"]; url.is_string()) _prevPageUrl = url.as_string();
	else _nextPageUrl.clear();
}

void WorkshopPopup::handleResponse(std::string_view resp)
{
	_makingRequest = false;
	_uploadPage = false;
	_buttonMenu->setVisible(true);
	_browseBtn->toggle(true);
	_myObjectsBtn->toggle(false);

	geode::log::info("resp: {}", resp);
	if (!_cardMenu) return;

	try
	{
		json::Value jsonResp = json::parse(resp);
		for (const json::Value& j : jsonResp["data"].as_array())
		{
			this->addCard(j);
		}

		updateMembers(jsonResp);
	}
	catch (std::exception& e)
	{

		geode::log::error("CATCHED: {}", e.what());
		this->onClose(nullptr);
		geode::createQuickPopup
		(
			"Error", fmt::format("<cr>{}</c>", e.what()), "OK", nullptr, [](FLAlertLayer*, bool) {}, true
		);
		return;
	}
	_cardMenu->updateLayout();
	updatePageButtons();
	_loadingCircle->setVisible(false);
}

void WorkshopPopup::fillEmpty()
{
//	if (!_cardMenu)
//		return;
//
//	if (_cardMenu->getChildrenCount() > 5) return;
//
//	while (_cardMenu->getChildrenCount() < 5)
//	{
//		addEmptyCard(false);
//	}
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


void WorkshopPopup::openPageLocalObjects(int page)
{
	_uploadPage = true;
	_browseBtn->toggle(false);
	std::vector<CCString*> objs = getLocalCustomObjectStrings();
	if (objs.empty())
	{
		//TODO
		return;
	}
	constexpr int perPage = 6;

	_cardMenu->removeAllChildrenWithCleanup(true);

	int pageFix      = page <= 0 ? 0 : page - 1; //first page index must be 0 instead of 1
	int pageIndexEnd = pageFix * perPage + perPage;

	try
	{
		for (int i = pageFix * perPage; i < pageIndexEnd && static_cast<size_t>(i) < objs.size(); i++)
		{
			CCString* str = objs.at(i); //this first so it can catch properly
			CustomObjectCard* card = CustomObjectCard::create(
				{ .object_string = str->getCString(), .local = true},
				nullptr,
				nullptr
			);

			card->setUserObject(str);
			_cardMenu->addChild(card);
		}
	}
	catch (std::out_of_range&)
	{
		geode::log::error("out of bounds");
	}
	setCurrentPage(page);

	_maxPage = objs.size() / 6;

	if (objs.size() % 6 != 0) //if there are enough for a no full page
	{
		_maxPage += 1;
	}

	updatePageButtons();
	_cardMenu->updateLayout();
}

void WorkshopPopup::onLocalObject(CCObject* sender)
{
	if (_makingRequest) return;

	bool on = reinterpret_cast<CCMenuItemToggler*>(sender)->isOn();
	if (!on)
	{
		openPageLocalObjects(0);
	}
	else
	{
		geode::Loader::get()->queueInMainThread([this]
		{
			reinterpret_cast<CCMenuItemToggler*>(_myObjectsBtn)->toggle(true);
		});
	}
}

void WorkshopPopup::setCurrentPage(int page)
{
	_currentPage = page <= 0 ? 1 : page;
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
