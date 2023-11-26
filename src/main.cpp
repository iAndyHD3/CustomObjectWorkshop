#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "CustomObjectCard.h"
#include "WorkshopPopup.h"
#include <json.hpp>

using namespace cocos2d;

struct MenuLayerExt : geode::Modify<MenuLayerExt, MenuLayer>
{
	void onMoreGames(CCObject*)
	{
		// CCDictionary* customObjects = GameManager::sharedState()->m_customObjectDict;
		// CCDictElement* pElement;
		// CCDICT_FOREACH(customObjects, pElement)
		//{
		//	const char* key = pElement->getStrKey();
		//	CCString* str = (CCString*)pElement->getObject();
		//	log::info("key: {}, str:{}\n", key, str->getCString());
		// }
		//WorkshopPopup::create()->show();
	}
};

ButtonSprite* createEditorButtonSprite(const char* top, const char* bg = "GJ_button_01.png") {
	return ButtonSprite::create(
		CCSprite::createWithSpriteFrameName(top),
		32, true, 32, bg, 1.f
	);
}

struct EditorUIExt : geode::Modify<EditorUIExt, EditorUI>
{
	void onCustomObjectWorkshop(CCObject*)
	{
		WorkshopPopup::create()->show();
	}
	CCArray* createCustomItems()
	{
		CCArray* ret = EditorUI::createCustomItems();
		auto btn = CCMenuItemSpriteExtra::create(createEditorButtonSprite("gj_folderBtn_001.png", "GJ_button_04.png"), nullptr, this, menu_selector(EditorUIExt::onCustomObjectWorkshop));
		ret->addObject(btn);
		return ret;
	}
};
