#include "../CustomObjectData.h"
#include <Geode/Geode.hpp>
#include "cocos2d.h"

struct CustomObjectCardMembers
{
	LevelEditorLayer* editor {nullptr};
	CustomObjectData objectData {};
	float x = 80;
	float y = 100;
};

class CustomObjectCard : public CCMenuItemSpriteExtra
{
public:
	CustomObjectCardMembers m {};

	static CustomObjectCard* create(const CustomObjectCardMembers& data, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback);
	bool init(cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback);
private:

	CustomObjectCard(const CustomObjectCardMembers& members) : m(members) {}
};