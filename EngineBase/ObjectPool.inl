
using namespace EngineBase;

template <class Object>
CObjectPool<Object>::CObjectPool()
{
	::InitializeCriticalSection(&_criticalSection);
	_errorCode = ERROR_CODE_NONE;
	_pBlock = NULL;
	_dwBlockSize = 0;
	_dwChunkSize = 0;
	_dwObjectCount = 0;
	_lCurrentUsedObjectCount = 0;
	_dwAdditoryObjectCount = 0;
	_pNextUsableChunk = NULL;

	// 전용 힙을 생성, 일단 최대 크기 제한 없이 생성
	// 이 클래스의 사용은 스레드에 안전을 보장받아야 함 -> ㄴㄴ 안전을 보장해줌
	// 이 클래스로부터 할당받은 힙 영역의 메모리 블럭 일부인 Object 객체를 사용하는 여러 스레드들의 접근과 
	// HEAP_NO_SERIALIZE 옵션을 사용해서 힙 영역의 메모리 블럭을 할당/해제하는 접근은 별개의 동기화 작업임
	// HEAP_NO_SERIALIZE : 힙 영역의 메모리 블럭을 할당/해제하는 여러 스레드들의 동기적 접근을 보장하지 않음
	// _hHeap = ::HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	// 힙 영역에 대한 메모리 관련 작업에 대해서 스레드들 사이에 동기화를 보장하도록 수정
	_hHeap = ::HeapCreate(0, 0, 0);
	if (NULL == _hHeap)
	{
		_errorCode = (ERROR_CODE)::GetLastError();
	}
}

template <class Object>
CObjectPool<Object>::~CObjectPool()
{
	if (NULL != _hHeap && NULL != _pBlock)
	{
		// 메모리 블럭을 해제하고
		// ::HeapFree(_hHeap, HEAP_NO_SERIALIZE, _pBlock);
		// 힙 영역에 대한 메모리 관련 작업에 대해서 스레드들 사이에 동기화를 보장하도록 수정
		::HeapFree(_hHeap, 0, _pBlock);

		_pBlock = NULL;
	}

	if (NULL != _hHeap)
	{
		// 힙 영역을 해제
		::HeapDestroy(_hHeap);

		// 주의할 점은 소멸자가 호출되는 시점이 프로그램 종료 시점이기를 (매우) 권장한다는 것
		// 소멸자가 호출되었을 때도 사용중인 메모리에 대한 어떠한 케어도 없음
		_hHeap = NULL;
	}

	::DeleteCriticalSection(&_criticalSection);
	_errorCode = ERROR_CODE_NONE;
	_dwBlockSize = 0;
	_dwChunkSize = 0;
	_dwObjectCount = 0;
	_lCurrentUsedObjectCount = 0;
	_dwAdditoryObjectCount = 0;
	_pNextUsableChunk = NULL;
}

template <class Object>
void CObjectPool<Object>::ChainingAllocatedObjects(PVOID pBlock, SIZE_T dwChunkSize, SIZE_T dwBlockSize)
{
	// 이 함수는 멤버 함수에서만 호출되는 함수이므로 별다른 예외 처리 없음

	// chunk들을 체이닝을 위한 메모리 계산 작업
	// Chunk* pChunkCursor = static_cast<Chunk*>(pBlock);
	Chunk* pChunkCursor = (Chunk*)pBlock;
	// Chunk* pChunkEnd = reinterpret_cast<Chunk*>(reinterpret_cast<BYTE*>(pChunkCursor) + dwBlockSize);
	Chunk* pChunkEnd = (Chunk*)((BYTE*)pChunkCursor + dwBlockSize);

	// 사용 가능한 객체를 검색할 때 사용하기 위해 세팅
	_pNextUsableChunk = pChunkCursor;

	Chunk* pPreviousChunk = NULL;
	while (pChunkCursor < pChunkEnd)
	{
		// chunk 체이닝
		if (NULL != pPreviousChunk)
		{
			pPreviousChunk->_pNextUsableObjectHeader = pChunkCursor;
		}

		// 객체 헤더 멤버인 객체 포인터에 객체의 영역을 세팅 (헤더 바로 다음 영역)
		pChunkCursor->_pObject = (Object*)(pChunkCursor + 1);

		// 이전 chunk 세팅
		pPreviousChunk = pChunkCursor;

		// 다음 chunk
		// pChunkCursor = reinterpret_cast<Chunk*>(reinterpret_cast<BYTE*>(pChunkCursor) + dwChunkSize);
		pChunkCursor =(Chunk*)((BYTE*)pChunkCursor + dwChunkSize);
	}
}

template <class Object>
BOOL CObjectPool<Object>::AllocateObject(Object** ppAllocatedObject, SIZE_T dwAllocationCount, ERROR_CODE& errorCode)
{
	// 별다른 예외 처리가 없음

	// 힙에 직접 할당
	// 크기는 Object 사이즈만큼 할당
	// 이유는 이 클래스에서 따로 관리하지 않을 것이기 때문임
	(*ppAllocatedObject) = (Object*)::HeapAlloc(_hHeap, HEAP_ZERO_MEMORY, sizeof(Object) * dwAllocationCount);

	// 할당에 실패했다면
	if (NULL == (*ppAllocatedObject))
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	return TRUE;
}

template <class Object>
BOOL CObjectPool<Object>::DeallocateObject(Object* pReturningObject, ERROR_CODE& errorCode)
{
	// 반환하는 메모리가 유효한지 확인
	// 참고 사이트 : https://docs.microsoft.com/en-us/windows/desktop/api/HeapApi/nf-heapapi-heapvalidate
	// 일부 중요 내용
	// If the specified heap or memory block is invalid, the return value is zero.
	// On a system set up for debugging, the HeapValidate function then displays 
	// debugging messages that describe the part of the heap or memory block that is invalid, 
	// and stops at a hard - coded breakpoint so that you can examine the system 
	// to determine the source of the invalidity.
	// The HeapValidate function does not set the thread's last error value. 
	// There is no extended error information for this function.
	// do not call GetLastError
	// 이 함수 테스트하면서 자꾸 예외창 뜨길래 메모리 할당한 힙이 다르면 예외가 발생되는줄 알았는데
	// 디버깅 모드에서만 breakpoint 예외 올려주는거고 디버깅 모드가 아니면 조용히 TRUE/FALSE만 반환함
	// 정확히는 해당 힙에서 관리되는 메모리 관련 구조체(control structures)들을 통해 유효성 검사한다는 말이 있고 (이 줄은 official)
	// 아마 해당 힙을 통해서 할당하지 않은 메모리는 관련 구조체가 없기 때문에 FALSE를 리턴하는듯 (이 줄은 내 추측)
	// 위 내용에 추가 내용으로 스레드들 간에 이 힙 관련한 접근이 block되니 성능 감소가 언급되는데
	// 이 클래스 내부에서 일일히 메모리 주소들 관리하는 것보다 훨씬 이득일 것 같아서 그냥 쓰기로 함
	if (FALSE == ::HeapValidate(_hHeap, 0, (LPCVOID)pReturningObject))
	{
		// 위 내용대로 GetLastError를 쓰지 말 것
		errorCode = ERROR_CODE_INVALID_OBJECT_RETURN; 
		return FALSE;
	}

	// 힙에 직접 할당한 객체를 해제
	if (FALSE == ::HeapFree(_hHeap, 0, (LPVOID)pReturningObject))
	{
		// GetLastError는 이 객체 해제 함수 호출로부터 ::HeapFree 함수 실패를 알린 후 추출하도록 수정
		// errorCode = (ERROR_CODE)::GetLastError();
		errorCode = ERROR_CODE_FAILURE_IN_HEAP_FREE_FUNCTION;
		return FALSE;
	}

	return TRUE;
}

template <class Object>
BOOL CObjectPool<Object>::AllocateInitialBlock(SIZE_T dwInitialObjectCount, SIZE_T dwAdditoryObjectCount, ERROR_CODE& errorCode)
{
	CLock lock(_criticalSection);

	// 기존 에러가 있는지 검사
	if (ERROR_CODE_NONE != _errorCode)
	{
		errorCode = _errorCode;
		return FALSE;
	}

	// 힙 생성을 실패함
	if (NULL == _hHeap)
	{
		errorCode = ERROR_CODE_HEAP_CREATION_FAILED;
		return FALSE;
	}

	// 메모리 블럭을 이미 할당함
	if (NULL != _pBlock)
	{
		errorCode = ERROR_CODE_MEMORY_BLOCK_ALREADY_ALLOCATED;
		return FALSE;
	}
	
	// 최소한 1개(씩)는 할당해야 함
	if (0 == dwInitialObjectCount || 0 == dwAdditoryObjectCount)
	{
		errorCode = ERROR_CODE_INVALID_OBJECT_COUNT;
		return FALSE;
	}

	// 정렬된 chunk 크기
	/*
#ifdef _WIN64
	_dwChunkSize = (sizeof(Chunk) + sizeof(Object) + (8 - 1)) & ~(8 - 1);
#else
	_dwChunkSize = (sizeof(Chunk) + sizeof(Object) + (4 - 1)) & ~(4 - 1);
#endif
	*/
	_dwChunkSize = (sizeof(Chunk) + sizeof(Object) + (sizeof(PVOID) - 1)) & ~(sizeof(PVOID) - 1);

	// 메모리 블럭을 할당 및 초기화
	// HEAP_ZERO_MEMORY 옵션 : 메모리 블럭을 할당 받고 0으로 초기화
	// _pBlock = ::HeapAlloc(_hHeap, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, _dwBlockSize);
	// 힙 영역에 대한 메모리 관련 작업에 대해서 스레드들 사이에 동기화를 보장하도록 수정
	_pBlock = ::HeapAlloc(_hHeap, HEAP_ZERO_MEMORY, dwInitialObjectCount * _dwChunkSize);
	if (NULL == _pBlock)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	// 객체 개수
	_dwObjectCount = dwInitialObjectCount;

	// HeapReAlloc 호출해서 메모리 블럭 크기를 확장할 때 추가로 할당할 객체 개수
	// _dwAdditoryObjectCount = dwAdditoryObjectCount; // 사용하지 않음

	// 블럭 크기 = chunk 크기 x 개수
	_dwBlockSize = _dwChunkSize * _dwObjectCount;

	// chunk 리스트 체이닝
	ChainingAllocatedObjects(_pBlock, _dwChunkSize, _dwBlockSize);

	return TRUE;
}

template <class Object>
BOOL CObjectPool<Object>::AcquireAllocatedObject(Object** ppAllocatedObject, ERROR_CODE& errorCode)
{
	// 성능 문제로 Interlocked 시리즈로 대체
	//CLock lock(_criticalSection); 

	// 기존 에러가 있는지 검사
	if (ERROR_CODE_NONE != _errorCode)
	{
		errorCode = _errorCode;
		return FALSE;
	}

	// 메모리 블럭을 할당했는지 검사
	if (NULL == _pBlock)
	{
		// 메모리 블럭을 할당하지 않았어도 기본적으로 힙에서 할당하도록 수정
		// errorCode = ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED;
		return AllocateObject(ppAllocatedObject, 1, errorCode);
	}

	// 테스트2 시작
	// 사용 중인 객체 개수를 증가시켜 보고
	LONG lCurrentUsedObjectCount = ::InterlockedIncrement(&_lCurrentUsedObjectCount);

	// 할당된 개수를 초과하면 사용 못하는 걸로 처리
	// ㄴㄴ 직접 힙에 할당해보는 방법으로 처리
	if (lCurrentUsedObjectCount > _dwObjectCount)
	{
		// 1 증가시켰으니 1 감소는 해주고 가야지
		::InterlockedDecrement(&_lCurrentUsedObjectCount);

		// 힙에 직접 할당하도록 함
		if (FALSE != AllocateObject(ppAllocatedObject, 1, errorCode))
		{
			// 객체풀이 가득찬 상태인 것은 알리도록 함
			errorCode = ERROR_CODE_FULL_OBJECTS_IN_MEMORY_BLOCK;
			return TRUE;
		}
		
		return FALSE;
	}

	// _pNextUsableChunk->_pNextUsableObjectHeader가 NULL이어도 됨
	Chunk* pNextUsableChunk = _pNextUsableChunk;
	Chunk* pUsableChunk = NULL;

	while (pNextUsableChunk != pUsableChunk)
	{
		// 임시 저장용
		pNextUsableChunk = _pNextUsableChunk;

		// 사용 가능한 chunk를 빼온다
		pUsableChunk = (Chunk*)::InterlockedCompareExchangePointer((volatile PVOID*)&_pNextUsableChunk, 
			_pNextUsableChunk->_pNextUsableObjectHeader, pNextUsableChunk);
	} 

	// 빼오는 데 성공했다면
	*ppAllocatedObject = pUsableChunk->_pObject;

	// 사용될 객체의 chunk의 사용 가능한 chunk 해제
	pUsableChunk->_pNextUsableObjectHeader = NULL;

	return TRUE;
	// 테스트2 끝

	
	/*
	// 사용될 객체의 chunk
	Chunk* pAcquiredChunk = NULL;

	// 사용 가능한 객체가 있으면
	if (NULL != _pNextUsableChunk)
	{
		
		// 사용될 객체의 chunk로 세팅
		pAcquiredChunk = _pNextUsableChunk;

		// 사용 가능한 상태가 아니면 체이닝 리스트에서 제거하고 다시 호출
		// critical section은 동일 스레드에 대해 중첩 Enter/Leave가 가능하므로..
		//if (OBJECT_USE_STATUS_USABLE != pAcquiredChunk->_ObjectUseStatus)
		//{
		//	_pNextUsableChunk = _pNextUsableChunk->_pNextUsableObjectHeader;
		//	return AcquireAllocatedObject(ppAllocatedObject, errorCode);
		//}

		// 사용 가능한 객체를 세팅
		*ppAllocatedObject = pAcquiredChunk->_pObject;

		// 사용 중인 상태로 세팅
		pAcquiredChunk->_ObjectUseStatus = OBJECT_USE_STATUS_USED;

		// 다음에 있는 사용 가능한 객체로 세팅
		_pNextUsableChunk = pAcquiredChunk->_pNextUsableObjectHeader;

		// 사용될 객체의 chunk의 사용 가능한 chunk 해제
		pAcquiredChunk->_pNextUsableObjectHeader = NULL;

		return TRUE;
	}

	// 없으면 재할당 시도
	// HEAP_REALLOC_IN_PLACE_ONLY : 메모리 블럭의 크기 조정에 따른 메모리 블럭의 주소 이동을 하지 않음을 보장
	PVOID pNewPool = ::HeapReAlloc(_hHeap, HEAP_REALLOC_IN_PLACE_ONLY | HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, 
		_pBlock, _dwBlockSize + (_dwAdditoryObjectCount * _dwChunkSize));

	// 재할당에 실패하면 디폴트 힙에서 할당해 사용하도록 유도
	if (NULL == pNewPool)
	{
		errorCode = ERROR_CODE_NO_USABLE_OBJECT;
		return FALSE;
	}

	// 재할당에 성공 했으면 chunk 체이닝부터 작업
	// 새로 할당한 부분을 체이닝, 참고로 pNewPool과 _pBlock은 동일
	// ChainingAllocatedObjects(static_cast<PVOID>(static_cast<BYTE*>(pNewPool) + _dwBlockSize), _dwChunkSize, _dwAdditoryObjectCount * _dwChunkSize);
	ChainingAllocatedObjects((PVOID)((BYTE*)pNewPool + _dwBlockSize), _dwChunkSize, _dwAdditoryObjectCount * _dwChunkSize);

	// 사용될 객체의 chunk로 세팅
	pAcquiredChunk = _pNextUsableChunk;

	// 메모리 블럭 사이즈 재조정
	_dwBlockSize += (_dwAdditoryObjectCount * _dwChunkSize);

	// 할당된 객체 개수도 세팅
	_dwObjectCount += _dwAdditoryObjectCount;

	// 사용 가능한 객체를 세팅
	*ppAllocatedObject = pAcquiredChunk->_pObject;

	// 사용 중인 상태로 세팅
	pAcquiredChunk->_ObjectUseStatus = OBJECT_USE_STATUS_USED;

	// 다음에 있는 사용 가능한 객체로 세팅
	_pNextUsableChunk = pAcquiredChunk->_pNextUsableObjectHeader;

	// 사용될 객체의 chunk의 사용 가능한 chunk 해제
	pAcquiredChunk->_pNextUsableObjectHeader = NULL;

	return TRUE;
	*/
}

template <class Object>
BOOL CObjectPool<Object>::ReturnAllocatedObject(Object* pReturningObject, ERROR_CODE& errorCode)
{
	// 성능 문제로 Interlocked 시리즈로 대체
	//CLock lock(_criticalSection);

	// 기존 에러가 있는지 검사
	if (ERROR_CODE_NONE != _errorCode)
	{
		errorCode = _errorCode;
		return FALSE;
	}

	// 메모리 블럭을 할당했는지 검사
	if (NULL == _pBlock)
	{
		// 메모리 블럭을 할당하지 않았어도 기본적으로 힙에서 할당하도록 수정
		// errorCode = ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED;
		// 따라서 객체풀에서 사용하고 있지 않은 객체는 힙에서 직접 해제하도록 함
		return DeallocateObject(pReturningObject, errorCode);
	}

	// 메모리 유효성 검사
	//if (FALSE == ::HeapValidate(_hHeap, 0, _pBlock))
	//{
	//	errorCode = ERROR_CODE_INVALID_POD_OBJECT;
	//	return FALSE;
	//}

	// 위 함수 HeapValidate는 메모리 블럭 자체에 대한 유효성 검사를 하는거고 그 블럭의 일부를 검사하면 FALSE를 리턴;
	// 따라서 그냥 직접 범위 검사를 해주는 방법으로 변경
	// 메모리 주소값 연산에 대해 플랫폼별로 나누려 했으나 8바이트 타입이면 충분할 것 같아서 분리하지 않음
	
	// 마지막 chunk를 찾고
	// Chunk* pLastChunk = reinterpret_cast<Chunk*>(static_cast<BYTE*>(_pBlock) + (_dwChunkSize * (_dwObjectCount - 1)));
	Chunk* pLastChunk = (Chunk*)((BYTE*)_pBlock + (_dwChunkSize * (_dwObjectCount - 1)));
	
	// 첫 번째 객체의 시작 주소
	// ULONGLONG ullFirstObjectAddress = reinterpret_cast<ULONGLONG>((static_cast<Chunk*>(_pBlock))->_pObject);
	ULONGLONG ullFirstObjectAddress = (ULONGLONG)(((Chunk*)_pBlock)->_pObject);

	// 마지막 객체의 시작 주소
	// ULONGLONG ullLastObjectAddress = reinterpret_cast<ULONGLONG>(pLastChunk->_pObject);
	ULONGLONG ullLastObjectAddress = (ULONGLONG)(pLastChunk->_pObject);

	// 반환하려는 객체의 시작 주소
	// ULONGLONG ullReturningObjectAddress = reinterpret_cast<ULONGLONG>(pReturningObject);
	ULONGLONG ullReturningObjectAddress = (ULONGLONG)pReturningObject;

	// 유효 범위 안에 있는 메모리 주소인지 검사
	if (ullFirstObjectAddress > ullReturningObjectAddress || ullLastObjectAddress < ullReturningObjectAddress)
	{
		// 객체풀에서 사용하고 있지 않은 객체는 힙에서 직접 해제하도록 함
		return DeallocateObject(pReturningObject, errorCode);
	}

	// 일반적으로 Object 공간이 메모리 접근의 효율성을 위해 패딩 바이트가 추가되어 할당되겠지만
	// pack을 할 수도 있으므로 비트 연산을 통해 라운드를 구하기 보다는 직접 계산하도록 함

	// ((ullReturningObjectAddress - ullFirstObjectAddress) / _dwChunkSize) : 몇 번째 chunk인가?
	SIZE_T dwReturningChunkOffset = ((ullReturningObjectAddress - ullFirstObjectAddress) / _dwChunkSize) * _dwChunkSize;

	// 반환하는 chunk를 찾고
	// Chunk* pReturnChunk = reinterpret_cast<Chunk*>(static_cast<BYTE*>(_pBlock) + dwReturningChunkOffset);
	Chunk* pReturnChunk = (Chunk*)((BYTE*)_pBlock + dwReturningChunkOffset);


	// 테스트2 시작
	// _pNextUsableChunk가 NULL이어도 정상 동작함
	Chunk* pUsableChunk = NULL;
	do
	{
		// 임시 저장용
		pReturnChunk->_pNextUsableObjectHeader = _pNextUsableChunk;

		// 사용한 chunk를 넣어봄
		pUsableChunk = (Chunk*)::InterlockedCompareExchangePointer((volatile PVOID*)&_pNextUsableChunk,
			pReturnChunk, pReturnChunk->_pNextUsableObjectHeader);
	} while (pReturnChunk->_pNextUsableObjectHeader != pUsableChunk);

	// 반납했으니 사용 중인 개수 감소
	::InterlockedDecrement(&_lCurrentUsedObjectCount);

	return TRUE;
	// 테스트2 끝
	
	/*
	// 사용 가능한 객체가 있으면 해제 되는 객체로 바꿔서 세팅
	// 반환하는 객체를 리스트의 제일 앞에 넣어야 locality 성질을 잘 받음
	// chunk 리스트가 (논리적 말고 물리적으로) 꼬이면 locality 성질 받기가 어려워질 수 있으나.. 차후 개선하도록 함
	if (NULL != _pNextUsableChunk)
	{
		pReturnChunk->_pNextUsableObjectHeader = _pNextUsableChunk;
	}
	_pNextUsableChunk = pReturnChunk;

	return TRUE;
	*/
}

template <class Object>
BOOL CObjectPool<Object>::AcquireAllocatedMultipleObjects(Object** ppAllocatedMultipleObjects, SIZE_T dwAllocationCount, ERROR_CODE& errorCode)
{
	// 기존 에러가 있는지 검사
	if (ERROR_CODE_NONE != _errorCode)
	{
		errorCode = _errorCode;
		return FALSE;
	}

	return AllocateObject(ppAllocatedMultipleObjects, dwAllocationCount, errorCode);
}

template <class Object>
BOOL CObjectPool<Object>::ReturnAllocatedMultipleObjects(Object* pReturningMultipleObjects, ERROR_CODE& errorCode)
{
	// 기존 에러가 있는지 검사
	if (ERROR_CODE_NONE != _errorCode)
	{
		errorCode = _errorCode;
		return FALSE;
	}

	return DeallocateObject(pReturningMultipleObjects, errorCode);
}