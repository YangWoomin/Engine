
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

	// ���� ���� ����, �ϴ� �ִ� ũ�� ���� ���� ����
	// �� Ŭ������ ����� �����忡 ������ ����޾ƾ� �� -> ���� ������ ��������
	// �� Ŭ�����κ��� �Ҵ���� �� ������ �޸� �� �Ϻ��� Object ��ü�� ����ϴ� ���� ��������� ���ٰ� 
	// HEAP_NO_SERIALIZE �ɼ��� ����ؼ� �� ������ �޸� ���� �Ҵ�/�����ϴ� ������ ������ ����ȭ �۾���
	// HEAP_NO_SERIALIZE : �� ������ �޸� ���� �Ҵ�/�����ϴ� ���� ��������� ������ ������ �������� ����
	// _hHeap = ::HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
	// �� ������ ���� �޸� ���� �۾��� ���ؼ� ������� ���̿� ����ȭ�� �����ϵ��� ����
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
		// �޸� ���� �����ϰ�
		// ::HeapFree(_hHeap, HEAP_NO_SERIALIZE, _pBlock);
		// �� ������ ���� �޸� ���� �۾��� ���ؼ� ������� ���̿� ����ȭ�� �����ϵ��� ����
		::HeapFree(_hHeap, 0, _pBlock);

		_pBlock = NULL;
	}

	if (NULL != _hHeap)
	{
		// �� ������ ����
		::HeapDestroy(_hHeap);

		// ������ ���� �Ҹ��ڰ� ȣ��Ǵ� ������ ���α׷� ���� �����̱⸦ (�ſ�) �����Ѵٴ� ��
		// �Ҹ��ڰ� ȣ��Ǿ��� ���� ������� �޸𸮿� ���� ��� �ɾ ����
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
	// �� �Լ��� ��� �Լ������� ȣ��Ǵ� �Լ��̹Ƿ� ���ٸ� ���� ó�� ����

	// chunk���� ü�̴��� ���� �޸� ��� �۾�
	// Chunk* pChunkCursor = static_cast<Chunk*>(pBlock);
	Chunk* pChunkCursor = (Chunk*)pBlock;
	// Chunk* pChunkEnd = reinterpret_cast<Chunk*>(reinterpret_cast<BYTE*>(pChunkCursor) + dwBlockSize);
	Chunk* pChunkEnd = (Chunk*)((BYTE*)pChunkCursor + dwBlockSize);

	// ��� ������ ��ü�� �˻��� �� ����ϱ� ���� ����
	_pNextUsableChunk = pChunkCursor;

	Chunk* pPreviousChunk = NULL;
	while (pChunkCursor < pChunkEnd)
	{
		// chunk ü�̴�
		if (NULL != pPreviousChunk)
		{
			pPreviousChunk->_pNextUsableObjectHeader = pChunkCursor;
		}

		// ��ü ��� ����� ��ü �����Ϳ� ��ü�� ������ ���� (��� �ٷ� ���� ����)
		pChunkCursor->_pObject = (Object*)(pChunkCursor + 1);

		// ���� chunk ����
		pPreviousChunk = pChunkCursor;

		// ���� chunk
		// pChunkCursor = reinterpret_cast<Chunk*>(reinterpret_cast<BYTE*>(pChunkCursor) + dwChunkSize);
		pChunkCursor =(Chunk*)((BYTE*)pChunkCursor + dwChunkSize);
	}
}

template <class Object>
BOOL CObjectPool<Object>::AllocateObject(Object** ppAllocatedObject, SIZE_T dwAllocationCount, ERROR_CODE& errorCode)
{
	// ���ٸ� ���� ó���� ����

	// ���� ���� �Ҵ�
	// ũ��� Object �����ŭ �Ҵ�
	// ������ �� Ŭ�������� ���� �������� ���� ���̱� ������
	(*ppAllocatedObject) = (Object*)::HeapAlloc(_hHeap, HEAP_ZERO_MEMORY, sizeof(Object) * dwAllocationCount);

	// �Ҵ翡 �����ߴٸ�
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
	// ��ȯ�ϴ� �޸𸮰� ��ȿ���� Ȯ��
	// ���� ����Ʈ : https://docs.microsoft.com/en-us/windows/desktop/api/HeapApi/nf-heapapi-heapvalidate
	// �Ϻ� �߿� ����
	// If the specified heap or memory block is invalid, the return value is zero.
	// On a system set up for debugging, the HeapValidate function then displays 
	// debugging messages that describe the part of the heap or memory block that is invalid, 
	// and stops at a hard - coded breakpoint so that you can examine the system 
	// to determine the source of the invalidity.
	// The HeapValidate function does not set the thread's last error value. 
	// There is no extended error information for this function.
	// do not call GetLastError
	// �� �Լ� �׽�Ʈ�ϸ鼭 �ڲ� ����â �߱淡 �޸� �Ҵ��� ���� �ٸ��� ���ܰ� �߻��Ǵ��� �˾Ҵµ�
	// ����� ��忡���� breakpoint ���� �÷��ִ°Ű� ����� ��尡 �ƴϸ� ������ TRUE/FALSE�� ��ȯ��
	// ��Ȯ���� �ش� ������ �����Ǵ� �޸� ���� ����ü(control structures)���� ���� ��ȿ�� �˻��Ѵٴ� ���� �ְ� (�� ���� official)
	// �Ƹ� �ش� ���� ���ؼ� �Ҵ����� ���� �޸𸮴� ���� ����ü�� ���� ������ FALSE�� �����ϴµ� (�� ���� �� ����)
	// �� ���뿡 �߰� �������� ������� ���� �� �� ������ ������ block�Ǵ� ���� ���Ұ� ��޵Ǵµ�
	// �� Ŭ���� ���ο��� ������ �޸� �ּҵ� �����ϴ� �ͺ��� �ξ� �̵��� �� ���Ƽ� �׳� ����� ��
	if (FALSE == ::HeapValidate(_hHeap, 0, (LPCVOID)pReturningObject))
	{
		// �� ������ GetLastError�� ���� �� ��
		errorCode = ERROR_CODE_INVALID_OBJECT_RETURN; 
		return FALSE;
	}

	// ���� ���� �Ҵ��� ��ü�� ����
	if (FALSE == ::HeapFree(_hHeap, 0, (LPVOID)pReturningObject))
	{
		// GetLastError�� �� ��ü ���� �Լ� ȣ��κ��� ::HeapFree �Լ� ���и� �˸� �� �����ϵ��� ����
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

	// ���� ������ �ִ��� �˻�
	if (ERROR_CODE_NONE != _errorCode)
	{
		errorCode = _errorCode;
		return FALSE;
	}

	// �� ������ ������
	if (NULL == _hHeap)
	{
		errorCode = ERROR_CODE_HEAP_CREATION_FAILED;
		return FALSE;
	}

	// �޸� ���� �̹� �Ҵ���
	if (NULL != _pBlock)
	{
		errorCode = ERROR_CODE_MEMORY_BLOCK_ALREADY_ALLOCATED;
		return FALSE;
	}
	
	// �ּ��� 1��(��)�� �Ҵ��ؾ� ��
	if (0 == dwInitialObjectCount || 0 == dwAdditoryObjectCount)
	{
		errorCode = ERROR_CODE_INVALID_OBJECT_COUNT;
		return FALSE;
	}

	// ���ĵ� chunk ũ��
	/*
#ifdef _WIN64
	_dwChunkSize = (sizeof(Chunk) + sizeof(Object) + (8 - 1)) & ~(8 - 1);
#else
	_dwChunkSize = (sizeof(Chunk) + sizeof(Object) + (4 - 1)) & ~(4 - 1);
#endif
	*/
	_dwChunkSize = (sizeof(Chunk) + sizeof(Object) + (sizeof(PVOID) - 1)) & ~(sizeof(PVOID) - 1);

	// �޸� ���� �Ҵ� �� �ʱ�ȭ
	// HEAP_ZERO_MEMORY �ɼ� : �޸� ���� �Ҵ� �ް� 0���� �ʱ�ȭ
	// _pBlock = ::HeapAlloc(_hHeap, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, _dwBlockSize);
	// �� ������ ���� �޸� ���� �۾��� ���ؼ� ������� ���̿� ����ȭ�� �����ϵ��� ����
	_pBlock = ::HeapAlloc(_hHeap, HEAP_ZERO_MEMORY, dwInitialObjectCount * _dwChunkSize);
	if (NULL == _pBlock)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	// ��ü ����
	_dwObjectCount = dwInitialObjectCount;

	// HeapReAlloc ȣ���ؼ� �޸� �� ũ�⸦ Ȯ���� �� �߰��� �Ҵ��� ��ü ����
	// _dwAdditoryObjectCount = dwAdditoryObjectCount; // ������� ����

	// �� ũ�� = chunk ũ�� x ����
	_dwBlockSize = _dwChunkSize * _dwObjectCount;

	// chunk ����Ʈ ü�̴�
	ChainingAllocatedObjects(_pBlock, _dwChunkSize, _dwBlockSize);

	return TRUE;
}

template <class Object>
BOOL CObjectPool<Object>::AcquireAllocatedObject(Object** ppAllocatedObject, ERROR_CODE& errorCode)
{
	// ���� ������ Interlocked �ø���� ��ü
	//CLock lock(_criticalSection); 

	// ���� ������ �ִ��� �˻�
	if (ERROR_CODE_NONE != _errorCode)
	{
		errorCode = _errorCode;
		return FALSE;
	}

	// �޸� ���� �Ҵ��ߴ��� �˻�
	if (NULL == _pBlock)
	{
		// �޸� ���� �Ҵ����� �ʾҾ �⺻������ ������ �Ҵ��ϵ��� ����
		// errorCode = ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED;
		return AllocateObject(ppAllocatedObject, 1, errorCode);
	}

	// �׽�Ʈ2 ����
	// ��� ���� ��ü ������ �������� ����
	LONG lCurrentUsedObjectCount = ::InterlockedIncrement(&_lCurrentUsedObjectCount);

	// �Ҵ�� ������ �ʰ��ϸ� ��� ���ϴ� �ɷ� ó��
	// ���� ���� ���� �Ҵ��غ��� ������� ó��
	if (lCurrentUsedObjectCount > _dwObjectCount)
	{
		// 1 ������������ 1 ���Ҵ� ���ְ� ������
		::InterlockedDecrement(&_lCurrentUsedObjectCount);

		// ���� ���� �Ҵ��ϵ��� ��
		if (FALSE != AllocateObject(ppAllocatedObject, 1, errorCode))
		{
			// ��üǮ�� ������ ������ ���� �˸����� ��
			errorCode = ERROR_CODE_FULL_OBJECTS_IN_MEMORY_BLOCK;
			return TRUE;
		}
		
		return FALSE;
	}

	// _pNextUsableChunk->_pNextUsableObjectHeader�� NULL�̾ ��
	Chunk* pNextUsableChunk = _pNextUsableChunk;
	Chunk* pUsableChunk = NULL;

	while (pNextUsableChunk != pUsableChunk)
	{
		// �ӽ� �����
		pNextUsableChunk = _pNextUsableChunk;

		// ��� ������ chunk�� ���´�
		pUsableChunk = (Chunk*)::InterlockedCompareExchangePointer((volatile PVOID*)&_pNextUsableChunk, 
			_pNextUsableChunk->_pNextUsableObjectHeader, pNextUsableChunk);
	} 

	// ������ �� �����ߴٸ�
	*ppAllocatedObject = pUsableChunk->_pObject;

	// ���� ��ü�� chunk�� ��� ������ chunk ����
	pUsableChunk->_pNextUsableObjectHeader = NULL;

	return TRUE;
	// �׽�Ʈ2 ��

	
	/*
	// ���� ��ü�� chunk
	Chunk* pAcquiredChunk = NULL;

	// ��� ������ ��ü�� ������
	if (NULL != _pNextUsableChunk)
	{
		
		// ���� ��ü�� chunk�� ����
		pAcquiredChunk = _pNextUsableChunk;

		// ��� ������ ���°� �ƴϸ� ü�̴� ����Ʈ���� �����ϰ� �ٽ� ȣ��
		// critical section�� ���� �����忡 ���� ��ø Enter/Leave�� �����ϹǷ�..
		//if (OBJECT_USE_STATUS_USABLE != pAcquiredChunk->_ObjectUseStatus)
		//{
		//	_pNextUsableChunk = _pNextUsableChunk->_pNextUsableObjectHeader;
		//	return AcquireAllocatedObject(ppAllocatedObject, errorCode);
		//}

		// ��� ������ ��ü�� ����
		*ppAllocatedObject = pAcquiredChunk->_pObject;

		// ��� ���� ���·� ����
		pAcquiredChunk->_ObjectUseStatus = OBJECT_USE_STATUS_USED;

		// ������ �ִ� ��� ������ ��ü�� ����
		_pNextUsableChunk = pAcquiredChunk->_pNextUsableObjectHeader;

		// ���� ��ü�� chunk�� ��� ������ chunk ����
		pAcquiredChunk->_pNextUsableObjectHeader = NULL;

		return TRUE;
	}

	// ������ ���Ҵ� �õ�
	// HEAP_REALLOC_IN_PLACE_ONLY : �޸� ���� ũ�� ������ ���� �޸� ���� �ּ� �̵��� ���� ������ ����
	PVOID pNewPool = ::HeapReAlloc(_hHeap, HEAP_REALLOC_IN_PLACE_ONLY | HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, 
		_pBlock, _dwBlockSize + (_dwAdditoryObjectCount * _dwChunkSize));

	// ���Ҵ翡 �����ϸ� ����Ʈ ������ �Ҵ��� ����ϵ��� ����
	if (NULL == pNewPool)
	{
		errorCode = ERROR_CODE_NO_USABLE_OBJECT;
		return FALSE;
	}

	// ���Ҵ翡 ���� ������ chunk ü�̴׺��� �۾�
	// ���� �Ҵ��� �κ��� ü�̴�, ����� pNewPool�� _pBlock�� ����
	// ChainingAllocatedObjects(static_cast<PVOID>(static_cast<BYTE*>(pNewPool) + _dwBlockSize), _dwChunkSize, _dwAdditoryObjectCount * _dwChunkSize);
	ChainingAllocatedObjects((PVOID)((BYTE*)pNewPool + _dwBlockSize), _dwChunkSize, _dwAdditoryObjectCount * _dwChunkSize);

	// ���� ��ü�� chunk�� ����
	pAcquiredChunk = _pNextUsableChunk;

	// �޸� �� ������ ������
	_dwBlockSize += (_dwAdditoryObjectCount * _dwChunkSize);

	// �Ҵ�� ��ü ������ ����
	_dwObjectCount += _dwAdditoryObjectCount;

	// ��� ������ ��ü�� ����
	*ppAllocatedObject = pAcquiredChunk->_pObject;

	// ��� ���� ���·� ����
	pAcquiredChunk->_ObjectUseStatus = OBJECT_USE_STATUS_USED;

	// ������ �ִ� ��� ������ ��ü�� ����
	_pNextUsableChunk = pAcquiredChunk->_pNextUsableObjectHeader;

	// ���� ��ü�� chunk�� ��� ������ chunk ����
	pAcquiredChunk->_pNextUsableObjectHeader = NULL;

	return TRUE;
	*/
}

template <class Object>
BOOL CObjectPool<Object>::ReturnAllocatedObject(Object* pReturningObject, ERROR_CODE& errorCode)
{
	// ���� ������ Interlocked �ø���� ��ü
	//CLock lock(_criticalSection);

	// ���� ������ �ִ��� �˻�
	if (ERROR_CODE_NONE != _errorCode)
	{
		errorCode = _errorCode;
		return FALSE;
	}

	// �޸� ���� �Ҵ��ߴ��� �˻�
	if (NULL == _pBlock)
	{
		// �޸� ���� �Ҵ����� �ʾҾ �⺻������ ������ �Ҵ��ϵ��� ����
		// errorCode = ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED;
		// ���� ��üǮ���� ����ϰ� ���� ���� ��ü�� ������ ���� �����ϵ��� ��
		return DeallocateObject(pReturningObject, errorCode);
	}

	// �޸� ��ȿ�� �˻�
	//if (FALSE == ::HeapValidate(_hHeap, 0, _pBlock))
	//{
	//	errorCode = ERROR_CODE_INVALID_POD_OBJECT;
	//	return FALSE;
	//}

	// �� �Լ� HeapValidate�� �޸� �� ��ü�� ���� ��ȿ�� �˻縦 �ϴ°Ű� �� ���� �Ϻθ� �˻��ϸ� FALSE�� ����;
	// ���� �׳� ���� ���� �˻縦 ���ִ� ������� ����
	// �޸� �ּҰ� ���꿡 ���� �÷������� ������ ������ 8����Ʈ Ÿ���̸� ����� �� ���Ƽ� �и����� ����
	
	// ������ chunk�� ã��
	// Chunk* pLastChunk = reinterpret_cast<Chunk*>(static_cast<BYTE*>(_pBlock) + (_dwChunkSize * (_dwObjectCount - 1)));
	Chunk* pLastChunk = (Chunk*)((BYTE*)_pBlock + (_dwChunkSize * (_dwObjectCount - 1)));
	
	// ù ��° ��ü�� ���� �ּ�
	// ULONGLONG ullFirstObjectAddress = reinterpret_cast<ULONGLONG>((static_cast<Chunk*>(_pBlock))->_pObject);
	ULONGLONG ullFirstObjectAddress = (ULONGLONG)(((Chunk*)_pBlock)->_pObject);

	// ������ ��ü�� ���� �ּ�
	// ULONGLONG ullLastObjectAddress = reinterpret_cast<ULONGLONG>(pLastChunk->_pObject);
	ULONGLONG ullLastObjectAddress = (ULONGLONG)(pLastChunk->_pObject);

	// ��ȯ�Ϸ��� ��ü�� ���� �ּ�
	// ULONGLONG ullReturningObjectAddress = reinterpret_cast<ULONGLONG>(pReturningObject);
	ULONGLONG ullReturningObjectAddress = (ULONGLONG)pReturningObject;

	// ��ȿ ���� �ȿ� �ִ� �޸� �ּ����� �˻�
	if (ullFirstObjectAddress > ullReturningObjectAddress || ullLastObjectAddress < ullReturningObjectAddress)
	{
		// ��üǮ���� ����ϰ� ���� ���� ��ü�� ������ ���� �����ϵ��� ��
		return DeallocateObject(pReturningObject, errorCode);
	}

	// �Ϲ������� Object ������ �޸� ������ ȿ������ ���� �е� ����Ʈ�� �߰��Ǿ� �Ҵ�ǰ�����
	// pack�� �� ���� �����Ƿ� ��Ʈ ������ ���� ���带 ���ϱ� ���ٴ� ���� ����ϵ��� ��

	// ((ullReturningObjectAddress - ullFirstObjectAddress) / _dwChunkSize) : �� ��° chunk�ΰ�?
	SIZE_T dwReturningChunkOffset = ((ullReturningObjectAddress - ullFirstObjectAddress) / _dwChunkSize) * _dwChunkSize;

	// ��ȯ�ϴ� chunk�� ã��
	// Chunk* pReturnChunk = reinterpret_cast<Chunk*>(static_cast<BYTE*>(_pBlock) + dwReturningChunkOffset);
	Chunk* pReturnChunk = (Chunk*)((BYTE*)_pBlock + dwReturningChunkOffset);


	// �׽�Ʈ2 ����
	// _pNextUsableChunk�� NULL�̾ ���� ������
	Chunk* pUsableChunk = NULL;
	do
	{
		// �ӽ� �����
		pReturnChunk->_pNextUsableObjectHeader = _pNextUsableChunk;

		// ����� chunk�� �־
		pUsableChunk = (Chunk*)::InterlockedCompareExchangePointer((volatile PVOID*)&_pNextUsableChunk,
			pReturnChunk, pReturnChunk->_pNextUsableObjectHeader);
	} while (pReturnChunk->_pNextUsableObjectHeader != pUsableChunk);

	// �ݳ������� ��� ���� ���� ����
	::InterlockedDecrement(&_lCurrentUsedObjectCount);

	return TRUE;
	// �׽�Ʈ2 ��
	
	/*
	// ��� ������ ��ü�� ������ ���� �Ǵ� ��ü�� �ٲ㼭 ����
	// ��ȯ�ϴ� ��ü�� ����Ʈ�� ���� �տ� �־�� locality ������ �� ����
	// chunk ����Ʈ�� (���� ���� ����������) ���̸� locality ���� �ޱⰡ ������� �� ������.. ���� �����ϵ��� ��
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
	// ���� ������ �ִ��� �˻�
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
	// ���� ������ �ִ��� �˻�
	if (ERROR_CODE_NONE != _errorCode)
	{
		errorCode = _errorCode;
		return FALSE;
	}

	return DeallocateObject(pReturningMultipleObjects, errorCode);
}