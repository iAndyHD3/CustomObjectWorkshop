#pragma once

#include "CustomObjectData.h"
#include <Geode/Geode.hpp>
#include "cocos2d.h"
#include <string_view>

class CustomObjectCard : public CCMenuItemSpriteExtra
{
public:
	cocos2d::CCSprite* icons_bg = nullptr;
	CustomObjectData _objectData{};

	static CustomObjectCard* create(const CustomObjectData& data, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback);
	static cocos2d::CCSprite* getCustomObjectPreview(std::string_view str);
	void onUploadObject(cocos2d::CCObject*);
	bool init(cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback);

private:
	CustomObjectCard(const CustomObjectData& members) : _objectData(members) {}
};
