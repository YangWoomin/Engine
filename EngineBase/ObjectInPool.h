#pragma once

#include "Common.h"
#include "ObjectPool.h"

namespace EngineBase
{
	// 객체풀에서 사용될 객체 최상위 base 클래스
	// ChildClass 객체가 객체풀의 객체로 적용됨
	template <class ChildClass>
	class CObjectInPool
	{
	public:
		// 객체풀 사용 여부
		// AllocateObjectPool를 통해 객체풀 생성 시 TRUE 세팅
		static BOOL _bUseObjectPool;

	protected:
		CObjectInPool();
		virtual ~CObjectInPool();

	public:
		// 객체풀 생성 함수
		static BOOL AllocateObjectPool(SIZE_T dwInitialObjectCount, SIZE_T dwAdditoryObjectCount, ERROR_CODE& errorCode);

		// 객체풀로부터 메모리 영역 할당받는 operator new
		void* operator new(size_t dwObjectSize, ERROR_CODE& errorCode);

		// 객체풀로 메모리 영역 반납하는 operator delete
		void operator delete(void* pObjectAddress, ERROR_CODE& errorCode);

		// 객체풀로부터 메모리 영역 할당받는 operator new[]
		void* operator new[](size_t dwAllocationSize, ERROR_CODE& errorCode);

		// 객체풀로 메모리 영역 반납하는 operator delete[]
		void operator delete[](void* pObjectAddress, ERROR_CODE& errorCode);
	};
}

#include "ObjectInPool.inl"