
using namespace EngineBase;

template <class PodObject>
CPodObjectPool<PodObject, CHECK_POD_OBJECT_TYPE>::CPodObjectPool(ERROR_CODE& errorCode)
{
	_pPool = NULL;
	_dwPoolSize = 0;
	_dwObjectSize = 0;
	_dwObjectCount = 0;
	_pUsablePodObject = NULL;

	// 전용 힙을 생성, 일단 최대 크기 제한 없이 생성
	_hHeap = ::HeapCreate(0, 0, 0);
	if (NULL == _hHeap)
	{
		errorCode = (ERROR_CODE)::GetLastError();
	}
}

template <class PodObject>
CPodObjectPool<PodObject, CHECK_POD_OBJECT_TYPE>::~CPodObjectPool()
{
	if (NULL != _hHeap && NULL != _pPool)
	{
		// 메모리 블럭을 해제하고
		::HeapFree(_hHeap, 0, _pPool);
	}

	if (NULL != _hHeap)
	{
		// 힙 영역을 해제
		::HeapDestroy(_hHeap);

		// 주의할 점은 소멸자가 호출되는 시점이 프로그램 종료 시점이기를 (매우) 권장한다는 것
		// 소멸자가 호출되었을 때도 사용중인 메모리에 대한 어떠한 케어도 없음

		_hHeap = NULL;
		_pPool = NULL;
		_dwPoolSize = 0;
		_dwObjectSize = 0;
		_dwObjectCount = 0;
		_pUsablePodObject = NULL;
	}
}

template <class PodObject>
BOOL CPodObjectPool<PodObject, CHECK_POD_OBJECT_TYPE>::CreatePodObjectPool(SIZE_T dwPodObjectAllocationCount, ERROR_CODE& errorCode)
{
	// 힙 생성을 실패함
	if (NULL == _hHeap)
	{
		errorCode = ERROR_CODE_HEAP_CREATION_FAILED;
		return FALSE;
	}

	// 메모리 블럭을 이미 할당함
	if (NULL != _pPool)
	{
		errorCode = ERROR_CODE_MEMORY_BLOCK_ALREADY_ALLOCATED;
		return FALSE;
	}

	// 최소한 1개는 할당해야 함
	if (0 == dwPodObjectAllocationCount)
	{
		errorCode = ERROR_CODE_INVALID_POD_OBJECT_COUNT;
		return FALSE;
	}

	// 객체 크기
	_dwObjectSize = sizeof(CPodObjectHeader<PodObject>);

	// 객체 개수
	_dwObjectCount = dwPodObjectAllocationCount;

	// 객체 크기 x 개수
	_dwPoolSize = _dwObjectSize * _dwObjectCount;

	// 객체풀을 위한 힙 영역에 메모리 블럭을 할당 및 초기화
	_pPool = ::HeapAlloc(_hHeap, HEAP_ZERO_MEMORY, _dwPoolSize);
	if (NULL == _pPool)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	// 각 객체들을 체이닝하기 위한 작업
	CPodObjectHeader<PodObject>* pPodObjectHeader = (CPodObjectHeader<PodObject>*)_pPool;
	pPodObjectHeader->_podObjectUseStatus = POD_OBJECT_USE_STATUS_USABLE; // 사용 가능 상태
	_pUsablePodObject = pPodObjectHeader; // 사용 가능한 객체를 검색할 때 사용하기 위해 세팅

	// 할당되지 않은 메모리 접근을 피하기 위해 크기 단위로 접근
	CPodObjectHeader<PodObject>* pPodObjectPreviousHeader = pPodObjectHeader;
	SIZE_T dwObjectCursor = _dwObjectSize;
	while (dwObjectCursor < _dwPoolSize)
	{
		pPodObjectHeader++;
		pPodObjectHeader->_podObjectUseStatus = POD_OBJECT_USE_STATUS_USABLE;
		pPodObjectPreviousHeader->_pNextUsablePodObject = pPodObjectHeader;
		pPodObjectPreviousHeader = pPodObjectHeader;
		dwObjectCursor += _dwObjectSize;
	}

	return TRUE;
}

template <class PodObject>
BOOL CPodObjectPool<PodObject, CHECK_POD_OBJECT_TYPE>::AcquireAllocatedPodObject(PodObject** ppPodObject, ERROR_CODE& errorCode)
{
	// 힙 생성을 실패함
	if (NULL == _hHeap)
	{
		errorCode = ERROR_CODE_HEAP_CREATION_FAILED;
		return FALSE;
	}

	// 메모리 블럭이 할당되지 않음
	if (NULL == _pPool)
	{
		errorCode = ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED;
		return FALSE;
	}

	// 사용 가능한 객체가 없음
	if (NULL == _pUsablePodObject)
	{
		// 사용 가능한 객체가 없으면 메모리 블럭 크기를 재조절해봄
		PVOID pNewPool = ::HeapReAlloc(_hHeap, HEAP_REALLOC_IN_PLACE_ONLY | HEAP_ZERO_MEMORY, _pPool, _dwPoolSize + _dwObjectSize);
		if (NULL == pNewPool)
		{
			errorCode = ERROR_CODE_NO_USABLE_POD_OBJECT;
			*ppPodObject = NULL;
			return FALSE;
		}

		// HEAP_REALLOC_IN_PLACE_ONLY 옵션을 사용했으니 메모리 블럭 재할당이 성공이면 주소(pNewPool)가 기존(_pPool)과 동일함
		// 메모리 블럭의 마지막 객체를 찾고
		CPodObjectHeader<PodObject>* pEndPodObjectHeader = (CPodObjectHeader<PodObject>*)_pPool + _dwPoolSize;

		// 사용 중인 상태로 세팅
		pEndPodObjectHeader->_podObjectUseStatus = POD_OBJECT_USE_STATUS_USED;

		// pod 객체를 세팅
		*ppPodObject = &(pEndPodObjectHeader->_podObject);

		// 크기도 다시 세팅
		_dwPoolSize += _dwObjectSize;

		// 메모리 블럭에 객체가 가득 찼다는 것을 알려줌
		errorCode = ERROR_CODE_FULL_POD_OBJECTS_IN_MEMORY_BLOCK;

		return TRUE;
	}

	// 사용 가능한 객체가 사용 가능한 상태가 아니다?
	if (POD_OBJECT_USE_STATUS_USABLE != _pUsablePodObject->_podObjectUseStatus)
	{
		// 사용 가능한 리스트에서 제거하고 이 함수를 다시 호출
		_pUsablePodObject = _pUsablePodObject->_pNextUsablePodObject;
		return AcquireAllocatedPodObject(ppPodObject, errorCode);
	}

	// 사용 중으로 세팅
	_pUsablePodObject->_podObjectUseStatus = POD_OBJECT_USE_STATUS_USED;
	*ppPodObject = &(_pUsablePodObject->_podObject);

	_pUsablePodObject = _pUsablePodObject->_pNextUsablePodObject;

	return TRUE;
}

template <class PodObject>
BOOL CPodObjectPool<PodObject, CHECK_POD_OBJECT_TYPE>::ReleaseAllocatedPodObject(PodObject* pPodObject, ERROR_CODE& errorCode)
{
	// 힙 생성을 실패함
	if (NULL == _hHeap)
	{
		errorCode = ERROR_CODE_HEAP_CREATION_FAILED;
		return FALSE;
	}

	// 메모리 블럭이 할당되지 않음
	if (NULL == _pPool)
	{
		errorCode = ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED;
		return FALSE;
	}

	// 메모리 유효성 검사
	//if (FALSE == ::HeapValidate(_hHeap, 0, _pPool))
	//{
	//	errorCode = ERROR_CODE_INVALID_POD_OBJECT;
	//	return FALSE;
	//}

	// 위 함수 HeapValidate는 메모리 블럭 자체에 대한 유효성 검사를 하는거고 그 블럭의 일부를 검사하면 FALSE를 리턴;
	// 따라서 그냥 직접 범위 검사를 해주는 방법으로 변경
	CPodObjectHeader<PodObject>* pLastPodObject = (CPodObjectHeader<PodObject>*)_pPool + (_dwObjectCount - 1);
	if ((__int64)_pPool > (__int64)pPodObject || (__int64)(&(pLastPodObject->_podObject)) < (__int64)pPodObject)
	{
		errorCode = ERROR_CODE_INVALID_POD_OBJECT;
		return FALSE;
	}
	
	// 주소값 연산 때문에 전처리기로 플랫폼마다 계산을 나눌까 했는데 굳이 필요 없을듯
	// 일반적으로 PodObject 크기가 메모리 정렬 단위(32bit, 64bit 모두 4byte단위)에 맞춰서 정해지지만
	// pack을 할 수도 있으므로 비트 연산을 통해 라운드를 구하기 보다는 직접 계산하도록 함
	CPodObjectHeader<PodObject>* pReleasePodObject 
		= (CPodObjectHeader<PodObject>*)_pPool + (((__int64)pPodObject - (__int64)_pPool) / _dwObjectSize);

	// 사용 가능한 상태로 세팅
	pReleasePodObject->_podObjectUseStatus = POD_OBJECT_USE_STATUS_USABLE;

	// 사용 가능한 객체가 있으면 해제 되는 객체로 바꿔서 세팅
	// 반환하는 객체를 리스트의 제일 앞에 넣어야 locality 성질을 잘 받음
	if (NULL != _pUsablePodObject)
	{
		pReleasePodObject->_pNextUsablePodObject = _pUsablePodObject;
	}
	_pUsablePodObject = pReleasePodObject;

	return TRUE;
}