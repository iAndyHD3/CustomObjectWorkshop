#pragma once

#include <Geode/Geode.hpp>
#include <string_view>

class WorkshopPopup : public geode::Popup<>
{
public:
	static WorkshopPopup* create();

private:
	cocos2d::CCMenu* _cardMenu		 = nullptr;
	cocos2d::CCMenu* _selectPageMenu = nullptr;

	CCMenuItemSpriteExtra* _prevBtn		   = nullptr;
	CCMenuItemSpriteExtra* _currentPageBtn = nullptr;
	CCMenuItemSpriteExtra* _nextBtn		   = nullptr;

	int _currentPage = 1;
	int _maxPage	 = 0;

	std::string _currentPageUrl	 = {};
	std::string _nextPageUrl	 = {};
	std::string _previousPageUrl = {};

	// devtools crashes for nullptr menu_selector so use this for empty callbacks

	bool setup() override;

	void updatePageButtons();

	void onNext(cocos2d::CCObject*);
	void onPrevious(cocos2d::CCObject*);

	void onCard(cocos2d::CCObject*);

protected:
	// returns false if it couldnt't add card, feel free to ignore
	bool addCard(const json::Value& j);

	// returns false if it couldnt't add card, feel free to ignore
	bool addEmptyCard(bool visible = true);

	void handleResponse(std::string_view resp);
	void openPage(int page, int perPage);

	// fills the cardMenu with empty cards until there are 6
	// so it aligns correctly if there are less than 6 cards in a page
	//(bad workaround)
	void fillEmpty();
};
