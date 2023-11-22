#include "WorkshopPopup.h"
#include "CustomObjectCard.h"
#include "CustomObjectView.h"
#include "json.hpp"
#include <Geode/utils/web.hpp>
#include <fmt/format.h>

using namespace cocos2d;

//exact decompilation of rob function - do not modify
CCSprite* EditorUI_menuItemFromObjectString(std::string_view objectString)
{
	auto editor		 = LevelEditorLayer::get();
	CCArray* objects = editor->createObjectsFromString(std::string(objectString), true);
	auto v8			 = CCSprite::create();

	CCPoint originPos{1000.0f, 1000.0f};

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
		float minX	  = texture.getMinY();
		float maxX	  = texture.getMaxX();
		float minY	  = texture.getMinY();
		float maxY	  = texture.getMaxY();

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

	v8->setContentSize({v13 - v14, v10 - v12});
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

	auto buttonMenu = CCMenu::create();
	buttonMenu->setLayout(ColumnLayout::create());
	m_mainLayer->addChild(buttonMenu);

	auto getBtn = [this](const char* title, const char* title2, SEL_MenuHandler callback = nullptr) {
		auto on	 = SearchButton::create("GJ_longBtn02_001.png", title, 0.4f, nullptr);
		auto off = SearchButton::create("GJ_longBtn01_001.png", title2 ? title2 : title, 0.4f, nullptr);
		on->setScale(0.7f);
		off->setScale(0.7f);
		return CCMenuItemToggler::create(off, on, this, callback);
	};

	// buttonMenu->addChild(getBtn("trending"));
	// buttonMenu->addChild(getBtn("recent"));
	// buttonMenu->addChild(getBtn("favorite"));
	buttonMenu->addChild(getBtn("My Objects", "Browse", menu_selector(WorkshopPopup::onUpload)));

	auto alignLeft = [](CCNode* node, float posX, float offset) {
		node->setPositionX(node->getPositionX() - (posX / 2) + (node->getContentSize().width / 2) + offset);
	};

	buttonMenu->updateLayout();

	constexpr float btnOffsetX = 10.0f;
	alignLeft(buttonMenu, m_size.width, btnOffsetX);

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
	else
	{
		_nextBtn->setVisible(_currentPage < _maxPage);
	}
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

std::vector<std::string> getLocalCustomObjectStrings()
{
	std::vector<std::string> ret;
	auto dict = geode::cocos::CCDictionaryExt<int, CCString>(GameManager::sharedState()->m_customObjectDict);
	for (const auto& [id, str] : dict)
	{
		ret.emplace_back(str->getCString());
	}

	return ret;
}

void WorkshopPopup::onUpload(cocos2d::CCObject* sender)
{
	if (_makingRequest) return;

	bool on = !reinterpret_cast<CCMenuItemToggler*>(sender)->isOn();

	if (on)
	{
		openPageLocalObjects(0);
	}
	else
	{
		_selectPageMenu->setVisible(false);
		openPageHyperBolus(getMainApiEndpoint(0));
	}
}

void WorkshopPopup::openPageHyperBolus(std::string_view apiurl)
{
	if (_makingRequest) return;

	_makingRequest = true;

	_cardMenu->removeAllChildrenWithCleanup(true);
	_loadingCircle->setVisible(true);
	geode::log::info("Making api request: {}", apiurl);
	geode::utils::web::AsyncWebRequest()
		.fetch(std::string(apiurl))
		.text()
		.then([this](const std::string& r) {
		handleResponse(r);
	}).cancelled([this](auto) { _makingRequest = false; });
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

	std::vector<std::string> objs = getLocalCustomObjectStrings();
	if (objs.empty())
	{
		//TODO
		return;
	}
	constexpr int perPage = 6;

	_cardMenu->removeAllChildrenWithCleanup(true);

	int pageFix      = page <= 0 ? 0 : page - 1; //first page index must be 0 instead of 1
	int pageIndexEnd = pageFix * perPage + perPage;

	geode::log::info("pageFix: {}, pageIndexEnd: {}", pageFix, pageIndexEnd);
	try
	{
		for (int i = pageFix * perPage; i < pageIndexEnd && static_cast<size_t>(i) < objs.size(); i++)
		{
			std::string& str = objs.at(i); //this first so it can catch properly
			auto card = CustomObjectCard::create({ .object_string = str, .local = true }, this, menu_selector(WorkshopPopup::onCard));
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
