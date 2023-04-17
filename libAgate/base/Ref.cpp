#include "pch.h" 
#include "Ref.h"
#include <new>
#include <assert.h>
#include <memory>
#include <set>
#include <unordered_map>
namespace agate
{
	Ref::~Ref()
	{
		assert(_ref->_reference == 0);
		_ref->_obj = nullptr;
		if (_ref->_weakReference == 0)
		{
			delete _ref;
		}
	}

};
