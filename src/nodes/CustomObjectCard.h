#pragma once

#include "../CustomObjectData.h"
#include <Geode/Geode.hpp>
#include "cocos2d.h"

class CustomObjectCard : public CCMenuItemSpriteExtra
{
public:
	cocos2d::CCSprite* icons_bg = nullptr;
	CustomObjectData _objectData{};

	static CustomObjectCard*
	create(const CustomObjectData& data, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback);
	bool init(cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback);
	void createObjects();

private:
	CustomObjectCard(const CustomObjectData& members) : _objectData(members) {}
};
