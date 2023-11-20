#pragma once

#include <Geode/Geode.hpp>
#include <string_view>

struct WorkshopPopup : public geode::Popup<>
{
	static WorkshopPopup* create();

	cocos2d::CCMenu* _cardMenu		 = nullptr;
	cocos2d::CCMenu* _selectPageMenu = nullptr;

	CCMenuItemSpriteExtra* _prevBtn		   = nullptr;
	CCMenuItemSpriteExtra* _currentPageBtn = nullptr;
	CCMenuItemSpriteExtra* _nextBtn		   = nullptr;

	LoadingCircle* _loadingCircle = nullptr;
	bool _makingRequest			  = false;

	int _currentPage = 1;
	int _maxPage	 = 0;
	bool _uploadPage = false;

	std::string _nextPageUrl = {};
	std::string _prevPageUrl = {};

	bool setup() override;

	void updatePageButtons();
	std::string getMainApiEndpoint(int page);
	void onNext(cocos2d::CCObject*);
	void onPrevious(cocos2d::CCObject*);
	void onCard(cocos2d::CCObject*);
	void onUpload(cocos2d::CCObject*);

	bool addCard(const json::Value& j);
	bool addEmptyCard(bool visible = true);
	void openPageLocalObjects(int page);
	void setCurrentPage(int page);

	// netwrok funcs

	void handleResponse(std::string_view resp);
	void openPageHyperBolus(std::string_view apiurl);
	void updateMembers(const json::Value& resp);

	// fills the cardMenu with empty cards until there are 6
	// so it aligns correctly if there are less than 6 cards in a page
	//(bad workaround)
	void fillEmpty();
};
