#pragma once

#include "Common.h"
#include "Singleton.h"
#include "ObjectHeader.h"
#include "Lock.h"

namespace EngineBase
{
	template <class Object>
	class CObjectHeader;

	// 각 객체(Object)별 메모리 관리를 별도로 해야 하므로 싱글톤을 적용
	template <class Object>
	class CObjectPool final : public CSingletonTemplate<Object, CObjectPool>
	{
	private:
		// chunk (CObjectHeader + Object) 
		typedef CObjectHeader<Object> Chunk;

		// 싱글톤 처리
		friend CSingletonTemplate;
		CObjectPool();

		// 메모리 블럭이 할당될 힙 영역
		HANDLE _hHeap;

		// 다중 스레드의 접근으로부터 힙 영역 보호를 위한 critical section
		CRITICAL_SECTION _criticalSection;

		// 에러 코드
		ERROR_CODE _errorCode;

		// 객체들이 할당될 메모리 블럭
		PVOID _pBlock;

		// 메모리 블럭 크기
		SIZE_T _dwBlockSize;

		// chunk 크기
		SIZE_T _dwChunkSize;

		// 현재 할당된 객체 개수
		SIZE_T _dwObjectCount;

		// 현재 사용 중인 객체 개수
		LONG _lCurrentUsedObjectCount;

		// 메모리 블럭 크기를 확장할 때 추가로 할당할 객체 개수
		SIZE_T _dwAdditoryObjectCount;

		// chunk 리스트에서 사용 가능한 object가 있는 다음 chunk
		Chunk* _pNextUsableChunk;

		// chunk들을 체이닝하기 위한 함수
		void ChainingAllocatedObjects(PVOID pBlock, SIZE_T dwChunkSize, SIZE_T dwBlockSize);

		// 힙에서 직접 객체를 할당하는 인라인 함수
		// 이 클래스 멤버 함수에서만 사용되기 때문에 별다른 예외 검사 없이 수행함
		inline BOOL AllocateObject(Object** ppAllocatedObject, SIZE_T dwAllocationCount, ERROR_CODE& errorCode);

		// 힙에서 직접 객체를 해제하는 인라인 함수
		// 이 클래스 멤버 함수에서만 사용되기 때문에 별다른 예외 검사 없이 수행함
		inline BOOL DeallocateObject(Object* pReturningObject, ERROR_CODE& errorCode);

	public:
		~CObjectPool();

		// 최초 메모리 블럭 할당 함수
		BOOL AllocateInitialBlock(SIZE_T dwInitialObjectCount, SIZE_T dwAdditoryObjectCount, ERROR_CODE& errorCode);

		// 객체 획득 함수 (1개만 획득)
		BOOL AcquireAllocatedObject(Object** ppAllocatedObject, ERROR_CODE& errorCode);

		// 객체 반환 함수 (1개만 반환)
		BOOL ReturnAllocatedObject(Object* pReturningObject, ERROR_CODE& errorCode);

		// 객체 획득 함수 (2개 이상 획득)
		BOOL AcquireAllocatedMultipleObjects(Object** ppAllocatedMultipleObjects, SIZE_T dwAllocationCount, ERROR_CODE& errorCode);

		// 객체 반환 함수 (2개 이상 반환)
		BOOL ReturnAllocatedMultipleObjects(Object* pReturningMultipleObjects, ERROR_CODE& errorCode);
	};
}

#include "ObjectPool.inl"