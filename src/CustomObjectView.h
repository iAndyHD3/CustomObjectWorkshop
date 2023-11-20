#include <Geode/Geode.hpp>
#include "CustomObjectData.h"

struct CustomObjectView : public geode::Popup<CustomObjectData*>
{
	CustomObjectData* _data = nullptr;

	bool setup(CustomObjectData*) override;
	static CustomObjectView* create(CustomObjectData*);

	void onWebsite(CCObject*);
	void onRate(CCObject*);
	void onFavorites(CCObject*);
	void onEditor(CCObject*);
	void onCustomObjects(CCObject*);
	void onCopyJson(CCObject*);
	void onSaveJson(CCObject*);
	void onComingSoon(CCObject*);
};
