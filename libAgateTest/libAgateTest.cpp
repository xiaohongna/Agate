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
			Assert::AreEqual(1, objectCount, L"��������");
			agate::Share_Ptr<DemoRef> ptr(obj);
			ptr = new DemoRef();
			ptr->func();
			Assert::AreEqual(1, objectCount, L"���� = ");
			ptr = nullptr;
			Assert::AreEqual(0, objectCount, L"���� = ");
			{
				agate::Share_Ptr<DemoRef> ptr2;
				CreateObj(&ptr2);
				Assert::AreEqual(1, objectCount, L"���� & ");
			}
			Assert::AreEqual(0, objectCount, L"���� 3 ");
		}

		TEST_METHOD(TestWeakRefPtr)
		{
			agate::Share_Ptr<DemoRef> ptr;
			CreateObj(&ptr);
			{
				agate::Weak_Ptr<DemoRef> weakptr(ptr);
				//ptr = nullptr;
				auto ptr2 = weakptr.lock();
				Assert::AreEqual(1, objectCount, L"���� weak ");
			}
			Assert::AreEqual(1, objectCount, L"���� weak 2");
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
				Assert::AreEqual(2, objectCount, L"ѭ������");
			}
			parent.reset();
			Assert::AreEqual(0, objectCount, L"ѭ������2");
		}

	};
}
