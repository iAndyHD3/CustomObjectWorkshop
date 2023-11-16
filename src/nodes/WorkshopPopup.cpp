#include "WorkshopPopup.h"
#include "CustomObjectCard.h"
#include "CustomObjectView.h"
#include "json.hpp"
#include <Geode/utils/web.hpp>
#include <fmt/format.h>

using namespace cocos2d;

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

	auto getBtn = [this](const char* title, SEL_MenuHandler callback = nullptr) {
		auto on	 = SearchButton::create("GJ_longBtn02_001.png", title, 0.5f, nullptr);
		auto off = SearchButton::create("GJ_longBtn01_001.png", title, 0.5f, nullptr);
		on->setScale(0.7f);
		off->setScale(0.7f);
		return CCMenuItemToggler::create(off, on, this, callback);
	};

	// buttonMenu->addChild(getBtn("trending"));
	// buttonMenu->addChild(getBtn("recent"));
	// buttonMenu->addChild(getBtn("favorite"));
	buttonMenu->addChild(getBtn("Upload", menu_selector(WorkshopPopup::onUpload)));

	auto alignLeft = [](CCNode* node, float posX, float offset) {
		node->setPositionX(node->getPositionX() - (posX / 2) + (node->getContentSize().width / 2) + offset);
	};

	buttonMenu->updateLayout();

	constexpr float btnOffsetX = 10.0f;
	alignLeft(buttonMenu, m_size.width, btnOffsetX);

	_cardMenu = CCMenu::create();
	_cardMenu->setContentSize({350, 260});
	_cardMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setGap(10.0f));
	//_cardMenu->setPositionX(_cardMenu->getPositionX() + 35.0f);
	//_cardMenu->setPositionY(_cardMenu->getPositionY() + 10.0f);
	_cardMenu->setPosition(winSize / 2 + CCPoint(35.0f, 10.0f));
	m_mainLayer->addChild(_cardMenu);

	_loadingCircle = LoadingCircle::create();
	_loadingCircle->setParentLayer(m_mainLayer);
	_loadingCircle->setPositionX(_loadingCircle->getPositionX() + 35.0f);
	_loadingCircle->show();

	this->openPage(getMainApiEndpoint(0));

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
	constexpr const char* endpoint = "https://hyperbolus.net/api/stencils?page={}&perPage={}";
	constexpr int perPage		   = 6;

	return fmt::format(endpoint, page, perPage);
}

void WorkshopPopup::onNext(cocos2d::CCObject*)
{
	openPage(getMainApiEndpoint(_currentPage + 1));
}

void WorkshopPopup::onPrevious(cocos2d::CCObject*)
{
	openPage(getMainApiEndpoint(_currentPage - 1));
}

void WorkshopPopup::onCard(cocos2d::CCObject* sender)
{
	if (auto card = geode::cast::safe_cast<CustomObjectCard*>(sender))
	{
		CustomObjectView::create(&card->_objectData)->show();
	}
}

std::vector<std::string> getCustomObjectStrings()
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
	// !: state it will switch to
	bool on = !reinterpret_cast<CCMenuItemToggler*>(sender)->isOn();

	_uploadPage = on;

	_cardMenu->removeAllChildrenWithCleanup(true);
	for (const std::string& objectString : getCustomObjectStrings())
	{
		auto card = CustomObjectCard::create({.object_string = objectString}, nullptr, nullptr);
		_cardMenu->addChild(card);
	}
	_cardMenu->updateLayout();
}

void WorkshopPopup::openPage(std::string_view apiurl)
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

	_currentPage = resp["current_page"].as_int();
	_maxPage	 = resp["last_page"].as_int();

	if (auto url = resp["next_page_url"]; url.is_string()) _nextPageUrl = url.as_string();
	else _nextPageUrl.clear();

	if (auto url = resp["prev_page_url"]; url.is_string()) _prevPageUrl = url.as_string();
	else _nextPageUrl.clear();
}

void WorkshopPopup::handleResponse(std::string_view resp)
{
	_makingRequest = false;

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
		geode::createQuickPopup(
			"Error", fmt::format("<cr>{}</c>", e.what()), "OK", nullptr, [](FLAlertLayer*, bool) {}, true);

		return;
	}

	_cardMenu->updateLayout();
	updatePageButtons();
	_loadingCircle->setVisible(false);
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
