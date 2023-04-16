#include "pch.h" 
#include "Ref.h"
#include <new>
#include <assert.h>
#include <memory>
#include <set>
#include <unordered_map>
namespace agate
{
	static std::atomic_uint s_refObjectID{1};

	static std::mutex s_weakMapLocker;
	static std::unordered_map<unsigned int, Ref*> s_weakObjectMap;

	Ref::Ref():_id{ s_refObjectID ++ }
	{

	}

	void Ref::lockWeakObject(unsigned int id, Ref** ppobj)
	{
		std::lock_guard<std::mutex> lock(s_weakMapLocker);
		auto found = s_weakObjectMap.find(id);
		if (found != s_weakObjectMap.end())
		{
			found->second->retain();
			*ppobj = found->second;
		}
	}

	Ref::~Ref()
	{
		std::lock_guard<std::mutex> lock(s_weakMapLocker);
		s_weakObjectMap.erase(_id);
	}

	unsigned int Ref::weakRetain()
	{
		std::lock_guard<std::mutex> lock(s_weakMapLocker);
		s_weakObjectMap[_id] = this;
		return _id;
	}

	void Ref::weakRelease()
	{
		//
	}

};
