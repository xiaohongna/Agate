#include "pch.h" 
#include "Ref.h"
#include <new>
#include <memory>
#include <set>
#include <unordered_map>
namespace agate
{
	Ref::~Ref()
	{
		_ref->release();
	}

};
