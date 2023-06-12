#pragma once
#include "base/Ref.h"
#include "graphic/Canvas.h"

namespace agate
{
	class AgateAPI Component: public Ref
	{
	public:
#pragma region ¸¸×Ó¹ÜÀí

		size_t getChildrenCount();

		void addChild(Component* child);

		Component* getChild(size_t index) const;

		Component* getChildByName(const std::string& name);

		void setName(const std::string& name);
#pragma endregion

	    void draw(Canvas* canvas);
	protected:
		virtual void selfDraw(Canvas* canvas);
	private:
		std::vector <Share_Ptr<Component> > _children;
		std::string  _name;
		size_t _hashOfName;
	};
}
