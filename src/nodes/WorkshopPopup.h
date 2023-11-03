#pragma once

#include <Geode/Geode.hpp>
#include <string_view>

class WorkshopPopup : public geode::Popup<>
{
public:
	static WorkshopPopup* create();
private:
	cocos2d::CCMenu* cardMenu = nullptr;

	//devtools crashes for nullptr menu_selector so use this for empty callbacks
	void nothing(cocos2d::CCObject*) {}
	
	bool setup() override;
protected:

	//returns false if it couldnt't add card, feel free to ignore
	bool addCard(const json::Value& j);
	
	//returns false if it couldnt't add card, feel free to ignore
	bool addEmptyCard(bool visible = true);
	
	void handleResponse(std::string_view resp);
	void makeRequest(int page);
	
	//fills the cardMenu with empty cards until there are 6
	//so it aligns correctly if there are less than 6 cards in a page
	//(bad workaround)
	void fillEmpty();

};
