#include "Component.h"
namespace agate
{
	size_t Component::getChildrenCount()
	{
		return _children.size();
	}

	void Component::addChild(Component* child)
	{
		_children.emplace_back(child);
	}

	Component* Component::getChild(size_t index) const
	{
		return _children[index].p;
	}

	Component* Component::getChildByName(const std::string& name)
	{
		std::hash<std::string> h;
		auto hash = h(name);
		for (auto& item : _children)
		{
			if (item->_hashOfName == hash && item->_name.compare(name) == 0)
			{
				return item.p;
			}
		}
		return nullptr;
	}

	void Component::setName(const std::string& name)
	{
		_name = name;
		std::hash<std::string> h;
		_hashOfName = h(name);
	}

	void Component::draw(Canvas* canvas)
	{
		selfDraw(canvas);
		for (auto& item : _children)
		{
			item->selfDraw(canvas);
		}
	}

	void Component::selfDraw(Canvas* canvas)
	{
	}

}