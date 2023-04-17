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
    class Ref;

    class Reference
    {
    private:
        Reference(Ref* obj)
        {
            _obj = obj;
        }

        ~Reference()
        {

        }
        
    private:
        friend class Ref;
        friend class BasePtr;
        std::atomic_uint _reference;
        std::atomic_uint _weakReference;
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
    class RefPtr 
    {
    public:
        RefPtr() :_p{}
        {
        }

        RefPtr(T* lp): _p{lp}
        {
            if (_p)
            {
                _p->retain();
            }
        }

        RefPtr(RefPtr<T>& lp)
        {
            _p = lp->_p;
            if (_p)
            {
                _p->retain();
            }
        }

        RefPtr(RefPtr<T>&& lp)
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

        T* operator=(RefPtr<T>&& lp)
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
        }

        T* operator=(RefPtr<T>& lp)
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
    
    class BasePtr
    {
    protected:
        BasePtr() :_ref{}
        {

        }
        BasePtr(Reference* ref) :_ref{ ref }
        {
        }

        void release()
        {
            if (_ref)
            {
                auto count = _ref->_weakReference--;
                if (count == 1 && _ref->_obj == nullptr)
                {
                    delete this;
                }
            }
        }
    protected:
        Reference* _ref;
    };

    template<class T>
    class WeakRefPtr: public BasePtr
    {
    public:
        WeakRefPtr() :BasePtr()
        {
        }

        WeakRefPtr(T* lp)
        {
            if (lp != nullptr)
            {
                BasePtr(lp->_ref);
                _ref->_weakReference++;
            }
        }

        WeakRefPtr(WeakRefPtr<T>& lp)
        {
            _ref = lp._ref;
        }

        WeakRefPtr(WeakRefPtr<T>&& lp)
        {
            _ref = lp._ref;
            lp._ref = nullptr;
        }

        T* operator=(T* lp)
        {
            if (lp == nullptr)
            {
                release();
                _ref = nullptr;
            }
            else if (_ref != lp->_ref)
            {
                release();
                _ref = lp->_ref;
                _ref->_weakReference++;
            }
            return lp;
        }

        RefPtr<T> lock()
        {
            RefPtr<T> obj;
            if (_ref)
            {
                _ref->_reference++;
                if (_ref->_obj)
                {
                    obj.attach(_ref->_obj);
                }
                else
                {
                    _ref->_reference--;
                }
            }
            return std::move(obj);
        }

        ~WeakRefPtr()
        {
            release();
        }
    };

};