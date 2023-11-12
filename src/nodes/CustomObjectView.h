#include <Geode/Geode.hpp>
#include "../CustomObjectData.h"

class CustomObjectView : public geode::Popup<CustomObjectData*>
{
public:
	CustomObjectData* _data = nullptr;

	bool setup(CustomObjectData*) override;
	static CustomObjectView* create(CustomObjectData*);
};
