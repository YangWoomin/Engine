#pragma once

#include "Common.h"
#include "ObjectPool.h"
#include "ObjectInPool.h"

namespace EngineBase
{
	// stl allocator
	// 참고 사이트
	// http://ozt88.tistory.com/31 
	// https://github.com/zeliard/GSP/blob/master/Homework3/EduServer_IOCP/XTL.h

	template <class _Ty>
	class STLAllocator
	{
	public:
		typedef _Ty value_type;

		typedef value_type *pointer;
		typedef const value_type *const_pointer;

		typedef value_type& reference;
		typedef const value_type& const_reference;

		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

		STLAllocator() = default;

		template <class _Other>
		STLAllocator(const STLAllocator<_Other>&)
		{
		}

		template <class _Other>
		struct rebind
		{
			typedef STLAllocator<_Other> other;
		};

		void construct(pointer _Ptr, const reference t)
		{
			new(_Ptr) value_type(t);
		}

		void destroy(pointer _Ptr)
		{
			_Ptr->~value_type();
		}

		pointer allocate(size_t _Count)
		{
			if (1 == _Count)
			{
				ERROR_CODE errorCode = ERROR_CODE_NONE;
				CObjectPool<value_type>* pObjectPool = CObjectPool<value_type>::GetInstance(errorCode);
				if (NULL != pObjectPool && ERROR_CODE_NONE == errorCode)
				{
					pointer pAllocatedObject = NULL;
					if (FALSE != pObjectPool->AcquireAllocatedObject(&pAllocatedObject, errorCode))
					{
						return static_cast<pointer>(pAllocatedObject);
					}
				}
			}

			return reinterpret_cast<pointer>(new BYTE[_Count * sizeof(value_type)]);
		}

		void deallocate(pointer _Ptr, size_t _Count)
		{
			ERROR_CODE errorCode = ERROR_CODE_NONE;
			CObjectPool<value_type>* pObjectPool = CObjectPool<value_type>::GetInstance(errorCode);
			if (NULL != pObjectPool && ERROR_CODE_NONE == errorCode)
			{
				if (FALSE != pObjectPool->ReturnAllocatedObject(_Ptr, errorCode))
				{
					return;
				}
			}

			delete reinterpret_cast<BYTE*>(_Ptr);
		}
	};

	template <class _Ty>
	struct xvector
	{
		typedef std::vector<_Ty, STLAllocator<_Ty>> type;
	};

	template <class _Ty>
	struct xdeque
	{
		typedef std::deque<_Ty, STLAllocator<_Ty>> type;
	};

	template <class _Ty>
	struct xlist
	{
		typedef std::list<_Ty, STLAllocator<_Ty>> type;
	};

	template <class _Key, class _Ty, class C = std::less<_Key>>
	struct xmap
	{
		typedef std::map<_Key, _Ty, C, STLAllocator<std::pair<_Key, _Ty>>> type;
	};

	template <class _Ty, class C = std::less<_Ty>>
	struct xset
	{
		typedef std::set<_Ty, C, STLAllocator<_Ty>> type;
	};
}