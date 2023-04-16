#include "pch.h"
#include "CppUnitTest.h"
#include "../libAgate/base/Ref.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace libAgateTest
{
	TEST_CLASS(libAgateTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			auto obj  = new agate::Ref();
			
			agate::RefPtr<agate::Ref> c(obj);
			agate::WeakRefPtr weak(c);
			auto b = weak.lock<agate::Ref>();

		}
	};
}
