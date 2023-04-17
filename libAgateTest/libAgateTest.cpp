#include "pch.h"
#include "CppUnitTest.h"
#include "../libAgate/base/Ref.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace libAgateTest
{
	static int objectCount = 0;

	class DemoRef : public agate::Ref
	{
	public:

		DemoRef():agate::Ref()
		{
			objectCount++;
		}
		~DemoRef()
		{
			objectCount--;
		}

		void func()
		{

		}
	};

	TEST_CLASS(ReferenceTest)
	{
	public:
		
		TEST_METHOD(TestRefPtr)
		{
			auto obj = new DemoRef();
			Assert::AreEqual(1, objectCount, L"创建对象");
			agate::RefPtr<DemoRef> ptr(obj);
			ptr = new DemoRef();
			ptr->func();
			Assert::AreEqual(1, objectCount, L"测试 = ");
			ptr = nullptr;
			Assert::AreEqual(0, objectCount, L"测试 = ");
		    
		}

		TEST_METHOD(TestWeakRefPtr)
		{
			agate::RefPtr<DemoRef> ptr = new DemoRef();
			agate::WeakRefPtr<DemoRef> weakptr(ptr);
			ptr = nullptr;
			ptr = weakptr.lock();
			Assert::AreEqual(0, objectCount, L"测试 weak ");
		}
	};
}
