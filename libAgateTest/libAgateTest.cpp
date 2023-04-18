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

		agate::Share_Ptr<DemoRef> child;
		agate::Weak_Ptr<DemoRef> parent;
	};

	void CreateObj(DemoRef** obj)
	{
		*obj = new DemoRef();
		(*obj)->retain();
	}

	TEST_CLASS(ReferenceTest)
	{
	public:
		
		TEST_METHOD(TestRefPtr)
		{
			auto obj = new DemoRef();
			Assert::AreEqual(1, objectCount, L"创建对象");
			agate::Share_Ptr<DemoRef> ptr(obj);
			ptr = new DemoRef();
			ptr->func();
			Assert::AreEqual(1, objectCount, L"测试 = ");
			ptr = nullptr;
			Assert::AreEqual(0, objectCount, L"测试 = ");
			{
				agate::Share_Ptr<DemoRef> ptr2;
				CreateObj(&ptr2);
				Assert::AreEqual(1, objectCount, L"测试 & ");
			}
			Assert::AreEqual(0, objectCount, L"测试 3 ");
		}

		TEST_METHOD(TestWeakRefPtr)
		{
			agate::Share_Ptr<DemoRef> ptr;
			CreateObj(&ptr);
			{
				agate::Weak_Ptr<DemoRef> weakptr(ptr);
				//ptr = nullptr;
				auto ptr2 = weakptr.lock();
				Assert::AreEqual(1, objectCount, L"测试 weak ");
			}
			Assert::AreEqual(1, objectCount, L"测试 weak 2");
		}

		TEST_METHOD(CircleTest)
		{
			agate::Share_Ptr<DemoRef> parent;
			CreateObj(&parent);
			{
				agate::Share_Ptr<DemoRef> child;
				CreateObj(&child);
				parent->child = child;
				child->parent = parent;
				Assert::AreEqual(2, objectCount, L"循环引用");
			}
			parent.reset();
			Assert::AreEqual(0, objectCount, L"循环引用2");
		}

	};
}
