
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
	// �� Ŭ������ ����� �����忡 ������ ����޾ƾ� ��
	// �� Ŭ�����κ��� �Ҵ���� �� ������ �޸� �� �Ϻ��� Object ��ü�� ����ϴ� ���� ��������� ���ٰ� 
	// HEAP_NO_SERIALIZE �ɼ��� ����ؼ� �� ������ �޸� ���� �Ҵ�/�����ϴ� ������ ������ ����ȭ �۾���
	// HEAP_NO_SERIALIZE : �� ������ �޸� ���� �Ҵ�/�����ϴ� ���� ��������� ������ ������ �������� ����
	_hHeap = ::HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
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
		::HeapFree(_hHeap, HEAP_NO_SERIALIZE, _pBlock);

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

		// ��� ���� ���·� ����
		pChunkCursor->_ObjectUseStatus = OBJECT_USE_STATUS_USABLE;

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

	// chunk ũ��
	_dwChunkSize = sizeof(Chunk) + sizeof(Object);

	// ��ü ����
	_dwObjectCount = dwInitialObjectCount;

	// HeapReAlloc ȣ���ؼ� �޸� �� ũ�⸦ Ȯ���� �� �߰��� �Ҵ��� ��ü ����
	_dwAdditoryObjectCount = dwAdditoryObjectCount;

	// �� ũ�� = chunk ũ�� x ����
	_dwBlockSize = _dwChunkSize * _dwObjectCount;

	// �޸� ���� �Ҵ� �� �ʱ�ȭ
	// HEAP_ZERO_MEMORY �ɼ� : �޸� ���� �Ҵ� �ް� 0���� �ʱ�ȭ
	_pBlock = ::HeapAlloc(_hHeap, HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE, _dwBlockSize);
	if (NULL == _pBlock)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

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
		errorCode = ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED;
		return FALSE;
	}

	// �׽�Ʈ2 ����
	// ��� ���� ��ü ������ �������� ����
	LONG lCurrentUsedObjectCount = ::InterlockedIncrement(&_lCurrentUsedObjectCount);

	// �Ҵ�� ���� �̻��̸� ��� ���ϴ� �ɷ� ó�� (��� ������ ��ü�� 1���� ���⵵�� ��)
	if (lCurrentUsedObjectCount >= _dwObjectCount)
	{
		// 1 ������������ 1 ���Ҵ� ���ְ� ������
		::InterlockedDecrement(&_lCurrentUsedObjectCount);
		errorCode = ERROR_CODE_NO_USABLE_OBJECT;
		return FALSE;
	}

	// ���� ��� ��Ŀ� ���� ��� ������ ��ü�� �ּ��� 1���� ���ܵιǷ� _pNextUsableChunk->_pNextUsableObjectHeader�� NULL�� �� ����
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

	// ��� ���� ���·� ����
	pUsableChunk->_ObjectUseStatus = OBJECT_USE_STATUS_USED;

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
		errorCode = ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED;
		return FALSE;
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
		errorCode = ERROR_CODE_INVALID_OBJECT_RETURN;
		return FALSE;
	}

	// �Ϲ������� Object ������ �޸� ������ ȿ������ ���� �е� ����Ʈ�� �߰��Ǿ� �Ҵ�ǰ�����
	// pack�� �� ���� �����Ƿ� ��Ʈ ������ ���� ���带 ���ϱ� ���ٴ� ���� ����ϵ��� ��

	// ((ullReturningObjectAddress - ullFirstObjectAddress) / _dwChunkSize) : �� ��° chunk�ΰ�?
	SIZE_T dwReturningChunkOffset = ((ullReturningObjectAddress - ullFirstObjectAddress) / _dwChunkSize) * _dwChunkSize;

	// ��ȯ�ϴ� chunk�� ã��
	// Chunk* pReturnChunk = reinterpret_cast<Chunk*>(static_cast<BYTE*>(_pBlock) + dwReturningChunkOffset);
	Chunk* pReturnChunk = (Chunk*)((BYTE*)_pBlock + dwReturningChunkOffset);

	// ��� ������ ���·� ����
	pReturnChunk->_ObjectUseStatus = OBJECT_USE_STATUS_USABLE;


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