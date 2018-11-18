
using namespace EngineBase;

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
		return pObjectPool->AllocateInitialBlock(dwInitialObjectCount, dwAdditoryObjectCount, errorCode);
	}

	return FALSE;
}

template <class ChildClass>
void* CObjectInPool<ChildClass>::operator new(size_t dwObjectSize, ERROR_CODE& errorCode)
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

	// 객체풀 사용에 실패했다면
	// 디폴트 힙에서 해당 크기의 메모리 영역을 할당
	return new BYTE[dwObjectSize];
}

template <class ChildClass>
void CObjectInPool<ChildClass>::operator delete(void* pObjectAddress, ERROR_CODE& errorCode)
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

	// 객체풀로 반납을 실패한 경우면
	// 애초에 객체풀 자체에 문제가 있으면 디폴트 힙을 사용하므로 디폴트 힙에서 해제하는 것은 문제되지 않을 것이지만
	// 위 함수 내부에서 ::HeapFree 에서 실패한 경우라면 디폴트 힙에서 해제하면 안 됨
	// 메모리 누수가 있을 수 있으므로 각별히 유의해야 함
	if (ERROR_CODE_FAILURE_IN_HEAP_FREE_FUNCTION == errorCode)
	{
		// 여기서 GetLastError를 호출하여 에러 코드를 반환
		errorCode = (ERROR_CODE)::GetLastError();
		return;
	}
	
	// 그 외의 경우는 디폴트 힙에서 할당한 경우이므로 디폴트 힙에서 해제
	delete static_cast<BYTE*>(pObjectAddress);
}

template <class ChildClass>
void* CObjectInPool<ChildClass>::operator new[](size_t dwAllocationSize, ERROR_CODE& errorCode)
{
	CObjectPool<ChildClass>* pObjectPool = CObjectPool<ChildClass>::GetInstance(errorCode);
	if (NULL != pObjectPool && ERROR_CODE_NONE == errorCode)
	{
		// 메모리 영역 획득 요청
		ChildClass* pAllocationObject = NULL;
		if (FALSE != pObjectPool->AcquireAllocatedMultipleObjects(&pAllocationObject, dwAllocationSize, errorCode))
		{
			return static_cast<void*>(pAllocationObject);
		}
	}

	// 객체풀 사용에 실패했다면
	// 디폴트 힙에서 해당 크기의 메모리 영역을 할당
	return new BYTE[dwAllocationSize * sizeof(ChildClass)];
}

template <class ChildClass>
void CObjectInPool<ChildClass>::operator delete[](void* pObjectAddress, ERROR_CODE& errorCode)
{
	CObjectPool<ChildClass>* pObjectPool = CObjectPool<ChildClass>::GetInstance(errorCode);
	if (NULL != pObjectPool && ERROR_CODE_NONE == errorCode)
	{
		// 메모리 영역 반납 요청
		if (FALSE != pObjectPool->ReturnAllocatedMultipleObjects(static_cast<ChildClass*>(pObjectAddress), errorCode))
		{
			// 정상 반납 완료
			return;
		}
	}

	// 객체풀로 반납을 실패한 경우면
	// 애초에 객체풀 자체에 문제가 있으면 디폴트 힙을 사용하므로 디폴트 힙에서 해제하는 것은 문제되지 않을 것이지만
	// 위 함수 내부에서 ::HeapFree 에서 실패한 경우라면 디폴트 힙에서 해제하면 안 됨
	// 메모리 누수가 있을 수 있으므로 각별히 유의해야 함
	if (ERROR_CODE_FAILURE_IN_HEAP_FREE_FUNCTION == errorCode)
	{
		// 여기서 GetLastError를 호출하여 에러 코드를 반환
		errorCode = (ERROR_CODE)::GetLastError();
		return;
	}

	// 아직 객체풀 할당이 되지 않았거나 객체풀로 반납을 실패한 경우면
	// 디폴트 힙을 사용하는 것이므로 디폴트 힙으로부터 영역 해제
	delete[] static_cast<BYTE*>(pObjectAddress);
}