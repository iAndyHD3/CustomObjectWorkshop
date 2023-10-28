#include "../CustomObjectData.h"
#include <Geode/Geode.hpp>

struct CustomObjectCardMembers
{
	LevelEditorLayer* editor {nullptr};
	CustomObjectData objectData {};
	float x = 80;
	float y = 100;
};

class CustomObjectCard : public cocos2d::CCNode
{
public:
	CustomObjectCardMembers m {};

	static CustomObjectCard* create(const CustomObjectCardMembers& data);
	bool init() override;
private:

	CustomObjectCard(const CustomObjectCardMembers& members) : m(members) {}
};