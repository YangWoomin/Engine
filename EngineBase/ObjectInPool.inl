
using namespace EngineBase;

template <class ChildClass> BOOL CObjectInPool<ChildClass>::_bUseObjectPool = FALSE;

template <class ChildClass>
CObjectInPool<ChildClass>::CObjectInPool()
{

}

template <class ChildClass>
CObjectInPool<ChildClass>::~CObjectInPool()
{

}

template <class ChildClass>
BOOL CObjectInPool<ChildClass>::AllocateObjectPool(SIZE_T dwInitialObjectCount, SIZE_T dwAdditoryObjectCount, ERROR_CODE& errorCode)
{
	CObjectPool<ChildClass>* pObjectPool = CObjectPool<ChildClass>::GetInstance(errorCode);

	if (NULL != pObjectPool && ERROR_CODE_NONE == errorCode)
	{
		_bUseObjectPool = pObjectPool->AllocateInitialBlock(dwInitialObjectCount, dwAdditoryObjectCount, errorCode);
	}

	return _bUseObjectPool;
}

template <class ChildClass>
void* CObjectInPool<ChildClass>::operator new(size_t dwObjectSize, ERROR_CODE& errorCode)
{
	// 객체 풀을 사용한다면
	if (FALSE != _bUseObjectPool)
	{
		CObjectPool<ChildClass>* pObjectPool = CObjectPool<ChildClass>::GetInstance(errorCode);
		if (NULL != pObjectPool && ERROR_CODE_NONE == errorCode)
		{
			// 메모리 영역 획득 요청
			ChildClass* pAllocationObject = NULL;
			if (FALSE != pObjectPool->AcquireAllocatedObject(&pAllocationObject, errorCode))
			{
				return static_cast<void*>(pAllocationObject);
			}
		}
	}

	// 아직 객체풀 할당이 되지 않았거나 객체풀 사용에 실패했다면
	// 디폴트 힙에서 해당 크기의 메모리 영역을 할당
	return new BYTE[dwObjectSize];
}

template <class ChildClass>
void CObjectInPool<ChildClass>::operator delete(void* pObjectAddress, ERROR_CODE& errorCode)
{
	// 객체 풀을 사용한다면
	if (FALSE != _bUseObjectPool)
	{
		CObjectPool<ChildClass>* pObjectPool = CObjectPool<ChildClass>::GetInstance(errorCode);
		if (NULL != pObjectPool && ERROR_CODE_NONE == errorCode)
		{
			// 메모리 영역 반납 요청
			if (FALSE != pObjectPool->ReturnAllocatedObject(static_cast<ChildClass*>(pObjectAddress), errorCode))
			{
				// 정상 반납 완료
				return;
			}
		}
	}

	// 아직 객체풀 할당이 되지 않았거나 객체풀로 반납을 실패한 경우면
	// 디폴트 힙을 사용하는 것이므로 디폴트 힙으로부터 영역 해제
	delete static_cast<BYTE*>(pObjectAddress);
}

template <class ChildClass>
void* CObjectInPool<ChildClass>::operator new[](size_t dwAllocationSize, ERROR_CODE& errorCode)
{
	// 객체 풀을 사용한다면
	if (FALSE != _bUseObjectPool)
	{
		// 객체풀에서 AcquireAllocatedObject를 반복 호출하면 되지 않을까 생각했는데
		// linear한 메모리 영역이어야 하니 chunk 리스트 구조인 현재 객체풀을 사용하기 어려운 상황
		// 좋은 방법이 있는지 연구중..
	}

	// 아직 객체풀 할당이 되지 않았거나 객체풀 사용에 실패했다면
	// 디폴트 힙에서 해당 크기의 메모리 영역을 할당
	return new BYTE[dwAllocationSize];
}

template <class ChildClass>
void CObjectInPool<ChildClass>::operator delete[](void* pObjectAddress, ERROR_CODE& errorCode)
{
	// 객체 풀을 사용한다면
	if (FALSE != _bUseObjectPool)
	{
		// 좋은 방법이 있는지 연구중..
	}

	// 아직 객체풀 할당이 되지 않았거나 객체풀로 반납을 실패한 경우면
	// 디폴트 힙을 사용하는 것이므로 디폴트 힙으로부터 영역 해제
	delete static_cast<BYTE*>(pObjectAddress);
}