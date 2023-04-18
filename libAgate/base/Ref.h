#pragma once
#if defined(_USRDLL)
#define AgateAPI     __declspec(dllexport)
#else        
#define AgateAPI     __declspec(dllimport)
#endif

#include <atomic>
#include <future>
#include <assert.h>

namespace agate
{
    class Ref;

    class Reference
    {
    private:
        Reference(Ref* obj)
        {
            _obj = obj;
        }

        void retain()
        {
            _weakReference++;
        }

        void release()
        {
            auto count = _weakReference--;
            if (count == 1)
            {
                delete this;
            }
        }

        ~Reference()
        {
            assert(_weakReference == 0);
        }
    private:
        friend class Ref;
        friend class BasePtr;
        std::atomic_uint _reference{0};
        std::atomic_uint _weakReference{1};
        Ref* _obj;
    };

	class AgateAPI Ref
	{
	public:

        void retain()
        {
            _ref->_reference++;
        }

		unsigned int release()
        {
            auto refcount = _ref->_reference--;
            if (refcount == 1)
            {
                delete this;
            }
            return refcount;
        }

	protected: 
        Ref()
        {
            _ref = new Reference(this);
        }
        
        virtual ~Ref();

	private:
        friend class BasePtr;
        Reference* _ref;
	};

    template <class T>
    class Share_Ptr
    {
    public:
        Share_Ptr() :_p{}
        {
        }

        Share_Ptr(T* lp): _p{lp}
        {
            if (_p)
            {
                _p->retain();
            }
        }

        Share_Ptr(Share_Ptr<T>& lp)
        {
            _p = lp->_p;
            if (_p)
            {
                _p->retain();
            }
        }

        Share_Ptr(Share_Ptr<T>&& lp)
        {
            _p = lp._p;
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

        bool operator==(T* pT)
        {
            return _p == pT;
        }

        T* operator=(T* lp)
        {
            if (_p != lp)
            {
                if (_p)
                {
                    _p->release();
                }
                _p = lp;
                if (_p)
                {
                    _p->retain();
                }
            }
            return *this;
        }

        T* operator=(Share_Ptr<T>&& lp)
        {
            if (_p != lp._p)
            {
                if (_p)
                {
                    _p->release();
                }
                _p = lp._p;
                lp._p = nullptr;
            }
            else if(_p)
            {
                lp._p->release();
                lp._p = nullptr;
            }
            return lp._p;
        }

        T** operator&() throw()
        {
            assert(_p == nullptr);
            return &_p;
        }

        T* operator=(Share_Ptr<T>& lp)
        {
            if (_p != lp._p)
            {
                if (_p)
                {
                    _p->release();
                }
                _p = lp._p;
                if (_p)
                {
                    _p->retain();
                }
            }
            return lp._p;
        }
        
        void reset() 
        {
            if (_p)
            {
                _p->release();
                _p = nullptr;
            }
        }

        ~Share_Ptr()
        {
            if (_p)
            {
                _p->release();
            }
        }

        T* _p;
    };
    
    class BasePtr 
    {
    public:
        ~BasePtr()
        {
            reset();
        }
        
        void reset()
        {
            if (_ref)
            {
                _ref->release();
                _ref = nullptr;
            }
        }
    protected:
        BasePtr() :_ref{}
        {

        }

        BasePtr(Ref* obj) 
        {
            if (obj)
            {
                _ref = obj->_ref;
                _ref->retain();
            }
            else
            {
                _ref = nullptr;
            }
        }

        BasePtr(BasePtr& ptr)
        {
            _ref = ptr._ref;
            if (_ref)
            {
                _ref->retain();
            }
        }

        BasePtr(BasePtr&& ptr) noexcept
        {
            _ref = ptr._ref;
            ptr._ref = nullptr;
        }

        void assign(Ref* obj)
        {
            if (obj)
            {
                if (obj->_ref != _ref)
                {
                    reset();
                    _ref = obj->_ref;
                    _ref->retain();
                }
            }
            else
            {
                reset();
            }
        }

        Ref* _lock()
        {
            auto count = _ref->_reference++;
            if (count > 0)
            {
                return _ref->_obj;
            }
            else
            {
                _ref->_reference--;
                return nullptr;
            }
        }
    protected:
        Reference* _ref;
    };

    template<class T>
    class Weak_Ptr: public BasePtr
    {
    public:
        Weak_Ptr() :BasePtr()
        {
        }

        Weak_Ptr(T* lp): BasePtr(lp)
        {

        }

        Weak_Ptr(WeakRefPtr<T>& lp)
        {
            _ref = lp._ref;
        }

        Weak_Ptr(WeakRefPtr<T>&& lp)
        {
            _ref = lp._ref;
            lp._ref = nullptr;
        }

        T* operator=(T* lp)
        {
            assign(lp);
            return lp;
        }

        Share_Ptr<T> lock()
        {
            Share_Ptr<T> ptr;
            ptr.attach(static_cast<T*>(_lock()));
            return ptr;
        }
    };

};