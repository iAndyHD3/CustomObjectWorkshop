#include "WorkshopPopup.h"
#include "CustomObjectCard.h"
#include "json.hpp"

using namespace cocos2d;

// clang-format off
bool WorkshopPopup::addCard(const json::Value& j)
{
	if (!cardMenu) return false;
	
	auto card = CustomObjectCard::create
	(
		{.objectData = j.as<CustomObjectData>(), .editor = nullptr},
		this,
		menu_selector(WorkshopPopup::nothing)
	);
	
	if(!card) return false;
	cardMenu->addChild(card);
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

	cardMenu = CCMenu::create();
	cardMenu->setContentSize({350, 260});
	cardMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setGap(10.0f));

	this->makeRequest(0);

	m_mainLayer->addChild(cardMenu);
	cardMenu->setPositionX(cardMenu->getPositionX() + 35.0f);
	cardMenu->setPositionY(cardMenu->getPositionY() - 5.0f);

	fillEmpty();
	cardMenu->updateLayout();

	return true;
}

void WorkshopPopup::makeRequest(int /*page*/)
{
	for (int i = 0; i < 2; i++)
	{
		addEmptyCard();
	}

	// web::AsyncWebRequest()
	//	.fetch(std::format("https://hyperbolus.net/api/stencils?page={}", page))
	//	.text()
	//	.then([this](const std::string& r) { handleResponse(r); });
}

void WorkshopPopup::handleResponse(std::string_view resp)
{
	geode::log::info("resp: {}", resp);
	if (!cardMenu) return;

	try
	{
		json::Value jsonResp = json::parse(resp);
		cardMenu->removeAllChildrenWithCleanup(true);
		for (const json::Value& j : jsonResp["data"].as_array())
		{
			this->addCard(j);
		}
	}
	catch (std::exception& e)
	{
		geode::log::error("CATCHED: {}", e.what());
	}
}

void WorkshopPopup::fillEmpty()
{
	if (!cardMenu) return;
	while (cardMenu->getChildrenCount() < 5)
	{
		addEmptyCard(false);
	}
}

bool WorkshopPopup::addEmptyCard(bool visible)
{
	if (!cardMenu) return false;
	auto card = CustomObjectCard::create({}, this, menu_selector(WorkshopPopup::nothing));
	if (!card) return false;

	card->setVisible(visible);
	cardMenu->addChild(card);
	return true;
}

WorkshopPopup* WorkshopPopup::create()
{
	auto ret = new WorkshopPopup();
	if (ret && ret->init(430.0f, 310.0f))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}
