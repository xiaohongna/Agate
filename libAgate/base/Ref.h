#pragma once
#if defined(_USRDLL)
#define AgateAPI     __declspec(dllexport)
#else        
#define AgateAPI     __declspec(dllimport)
#endif

#include <atomic>
#include <future>

namespace agate
{
	class AgateAPI Ref
	{
	public:

        void retain()
        {
            _reference++;
        }

		unsigned int release()
        {
            auto refcount = _reference--;
            if (refcount == 1)
            {
                delete this;
            }
            return refcount;
        }

        unsigned int weakRetain();

        void weakRelease();

        Ref();

        static void lockWeakObject(unsigned int id, Ref** ppobj);
	protected: 

        virtual ~Ref();

	private:
		std::atomic_uint _reference;
        unsigned int _id;
	};

    template <class T>
    class RefPtr 
    {
    public:
        RefPtr() :_p{} 
        {
        }
        RefPtr(T* lp) :_p{lp}
        {
            _p = lp;
            if (_p != nullptr)
            {
                _p->retain();
            }
        }

        RefPtr(RefPtr<T>& lp): _p{lp._p}
        {
            lp._p = nullptr;
        }

        RefPtr(RefPtr<T>&& lp)
        {
            _p = lp;
            lp._p = nullptr;
        }

        void attach(T* lp)
        {
            if (_p)
            {
                _p->release();
            }
            _p = lp;
        }

        operator T* () const
        {
            return _p;
        }

        T* operator->() const
        {
            return _p;
        }

        T& operator*() const
        {
            return *_p;
        }

        ~RefPtr()
        {
            if (_p)
            {
                _p->release();
            }
        }
    private:
        T* _p;

    };
      

    template<class T>
    class WeakRefPtr
    {
        WeakRefPtr() :_id{}
        {
        }
        WeakRefPtr(T* lp)
        {
            if (lp != nullptr)
            {
                _id = lp->weakRetain;
            }
        }

        WeakRefPtr(WeakRefPtr<T>& lp)
        {
            _id = lp._id;
        }

        WeakRefPtr(WeakRefPtr<T>&& lp)
        {
            _id = lp._id;
        }

        RefPtr<T> lock()
        {
            RefPtr<T> obj;
            if (_id)
            {
                Ref* p;
                Ref::lockWeakObject(_id, &p);
                obj.attach(static_cast<T*>(p));
            }
            return std::move(obj);
        }
    private:
        unsigned int _id;
    };

};