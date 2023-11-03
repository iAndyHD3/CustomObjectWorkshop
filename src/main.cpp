#include <Geode/Geode.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "nodes/CustomObjectCard.h"
#include "nodes/WorkshopPopup.h"
#include <Geode/utils/web.hpp>
#include <json.hpp>

using namespace geode::prelude;
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
		WorkshopPopup::create()->show();
	}
};

struct EditorUIExt : geode::Modify<EditorUIExt, EditorUI>
{
	void onPlayback(CCObject*)
	{
		//
		WorkshopPopup::create()->show();
	}
};
