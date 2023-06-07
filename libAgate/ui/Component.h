#pragma once
#include "Ref.h"

namespace agate
{
	class Component: public Ref
	{
	public:
#pragma region ���ӹ���

		size_t getChildrenCount();

		void addChild(Component* child);

		Component* getChild(size_t index) const;

		Component* getChildByName(const std::string& name);

		void setName(const std::string& name);
#pragma endregion

	    void draw();

		virtual void selfDraw();
	private:
		std::vector <Share_Ptr<Component> > _children;
		std::string  _name;
		size_t _hashOfName;
	};
}
