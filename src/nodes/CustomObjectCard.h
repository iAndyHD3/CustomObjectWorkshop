#include "../CustomObjectData.h"
#include <Geode/Geode.hpp>
#include "cocos2d.h"

struct CustomObjectCardMembers
{
	CustomObjectData objectData{};
	LevelEditorLayer* editor{nullptr};
};

class CustomObjectCard : public CCMenuItemSpriteExtra
{
public:
	cocos2d::CCSprite* icons_bg = nullptr;
	CustomObjectCardMembers m{};

	static CustomObjectCard* create(const CustomObjectCardMembers& data,
									cocos2d::CCObject* target,
									cocos2d::SEL_MenuHandler callback);
	bool init(cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback);
	void createObjects();

private:
	CustomObjectCard(const CustomObjectCardMembers& members) : m(members) {}
};
