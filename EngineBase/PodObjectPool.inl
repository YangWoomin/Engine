
using namespace EngineBase;

template <class PodObject>
CPodObjectPool<PodObject, CHECK_POD_OBJECT_TYPE>::CPodObjectPool(ERROR_CODE& errorCode)
{
	_pPool = NULL;
	_dwPoolSize = 0;
	_dwObjectSize = 0;
	_dwObjectCount = 0;
	_pUsablePodObject = NULL;

	// ���� ���� ����, �ϴ� �ִ� ũ�� ���� ���� ����
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
		// �޸� ���� �����ϰ�
		::HeapFree(_hHeap, 0, _pPool);
	}

	if (NULL != _hHeap)
	{
		// �� ������ ����
		::HeapDestroy(_hHeap);

		// ������ ���� �Ҹ��ڰ� ȣ��Ǵ� ������ ���α׷� ���� �����̱⸦ (�ſ�) �����Ѵٴ� ��
		// �Ҹ��ڰ� ȣ��Ǿ��� ���� ������� �޸𸮿� ���� ��� �ɾ ����

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
	// �� ������ ������
	if (NULL == _hHeap)
	{
		errorCode = ERROR_CODE_HEAP_CREATION_FAILED;
		return FALSE;
	}

	// �޸� ���� �̹� �Ҵ���
	if (NULL != _pPool)
	{
		errorCode = ERROR_CODE_MEMORY_BLOCK_ALREADY_ALLOCATED;
		return FALSE;
	}

	// �ּ��� 1���� �Ҵ��ؾ� ��
	if (0 == dwPodObjectAllocationCount)
	{
		errorCode = ERROR_CODE_INVALID_POD_OBJECT_COUNT;
		return FALSE;
	}

	// ��ü ũ��
	_dwObjectSize = sizeof(CPodObjectHeader<PodObject>);

	// ��ü ����
	_dwObjectCount = dwPodObjectAllocationCount;

	// ��ü ũ�� x ����
	_dwPoolSize = _dwObjectSize * _dwObjectCount;

	// ��üǮ�� ���� �� ������ �޸� ���� �Ҵ� �� �ʱ�ȭ
	_pPool = ::HeapAlloc(_hHeap, HEAP_ZERO_MEMORY, _dwPoolSize);
	if (NULL == _pPool)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	// �� ��ü���� ü�̴��ϱ� ���� �۾�
	CPodObjectHeader<PodObject>* pPodObjectHeader = (CPodObjectHeader<PodObject>*)_pPool;
	pPodObjectHeader->_podObjectUseStatus = POD_OBJECT_USE_STATUS_USABLE; // ��� ���� ����
	_pUsablePodObject = pPodObjectHeader; // ��� ������ ��ü�� �˻��� �� ����ϱ� ���� ����

	// �Ҵ���� ���� �޸� ������ ���ϱ� ���� ũ�� ������ ����
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
	// �� ������ ������
	if (NULL == _hHeap)
	{
		errorCode = ERROR_CODE_HEAP_CREATION_FAILED;
		return FALSE;
	}

	// �޸� ���� �Ҵ���� ����
	if (NULL == _pPool)
	{
		errorCode = ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED;
		return FALSE;
	}

	// ��� ������ ��ü�� ����
	if (NULL == _pUsablePodObject)
	{
		// ��� ������ ��ü�� ������ �޸� �� ũ�⸦ �������غ�
		PVOID pNewPool = ::HeapReAlloc(_hHeap, HEAP_REALLOC_IN_PLACE_ONLY | HEAP_ZERO_MEMORY, _pPool, _dwPoolSize + _dwObjectSize);
		if (NULL == pNewPool)
		{
			errorCode = ERROR_CODE_NO_USABLE_POD_OBJECT;
			*ppPodObject = NULL;
			return FALSE;
		}

		// HEAP_REALLOC_IN_PLACE_ONLY �ɼ��� ��������� �޸� �� ���Ҵ��� �����̸� �ּ�(pNewPool)�� ����(_pPool)�� ������
		// �޸� ���� ������ ��ü�� ã��
		CPodObjectHeader<PodObject>* pEndPodObjectHeader = (CPodObjectHeader<PodObject>*)_pPool + _dwPoolSize;

		// ��� ���� ���·� ����
		pEndPodObjectHeader->_podObjectUseStatus = POD_OBJECT_USE_STATUS_USED;

		// pod ��ü�� ����
		*ppPodObject = &(pEndPodObjectHeader->_podObject);

		// ũ�⵵ �ٽ� ����
		_dwPoolSize += _dwObjectSize;

		// �޸� ���� ��ü�� ���� á�ٴ� ���� �˷���
		errorCode = ERROR_CODE_FULL_POD_OBJECTS_IN_MEMORY_BLOCK;

		return TRUE;
	}

	// ��� ������ ��ü�� ��� ������ ���°� �ƴϴ�?
	if (POD_OBJECT_USE_STATUS_USABLE != _pUsablePodObject->_podObjectUseStatus)
	{
		// ��� ������ ����Ʈ���� �����ϰ� �� �Լ��� �ٽ� ȣ��
		_pUsablePodObject = _pUsablePodObject->_pNextUsablePodObject;
		return AcquireAllocatedPodObject(ppPodObject, errorCode);
	}

	// ��� ������ ����
	_pUsablePodObject->_podObjectUseStatus = POD_OBJECT_USE_STATUS_USED;
	*ppPodObject = &(_pUsablePodObject->_podObject);

	_pUsablePodObject = _pUsablePodObject->_pNextUsablePodObject;

	return TRUE;
}

template <class PodObject>
BOOL CPodObjectPool<PodObject, CHECK_POD_OBJECT_TYPE>::ReleaseAllocatedPodObject(PodObject* pPodObject, ERROR_CODE& errorCode)
{
	// �� ������ ������
	if (NULL == _hHeap)
	{
		errorCode = ERROR_CODE_HEAP_CREATION_FAILED;
		return FALSE;
	}

	// �޸� ���� �Ҵ���� ����
	if (NULL == _pPool)
	{
		errorCode = ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED;
		return FALSE;
	}

	// �޸� ��ȿ�� �˻�
	//if (FALSE == ::HeapValidate(_hHeap, 0, _pPool))
	//{
	//	errorCode = ERROR_CODE_INVALID_POD_OBJECT;
	//	return FALSE;
	//}

	// �� �Լ� HeapValidate�� �޸� �� ��ü�� ���� ��ȿ�� �˻縦 �ϴ°Ű� �� ���� �Ϻθ� �˻��ϸ� FALSE�� ����;
	// ���� �׳� ���� ���� �˻縦 ���ִ� ������� ����
	CPodObjectHeader<PodObject>* pLastPodObject = (CPodObjectHeader<PodObject>*)_pPool + (_dwObjectCount - 1);
	if ((__int64)_pPool > (__int64)pPodObject || (__int64)(&(pLastPodObject->_podObject)) < (__int64)pPodObject)
	{
		errorCode = ERROR_CODE_INVALID_POD_OBJECT;
		return FALSE;
	}
	
	// �ּҰ� ���� ������ ��ó����� �÷������� ����� ������ �ߴµ� ���� �ʿ� ������
	// �Ϲ������� PodObject ũ�Ⱑ �޸� ���� ����(32bit, 64bit ��� 4byte����)�� ���缭 ����������
	// pack�� �� ���� �����Ƿ� ��Ʈ ������ ���� ���带 ���ϱ� ���ٴ� ���� ����ϵ��� ��
	CPodObjectHeader<PodObject>* pReleasePodObject 
		= (CPodObjectHeader<PodObject>*)_pPool + (((__int64)pPodObject - (__int64)_pPool) / _dwObjectSize);

	// ��� ������ ���·� ����
	pReleasePodObject->_podObjectUseStatus = POD_OBJECT_USE_STATUS_USABLE;

	// ��� ������ ��ü�� ������ ���� �Ǵ� ��ü�� �ٲ㼭 ����
	// ��ȯ�ϴ� ��ü�� ����Ʈ�� ���� �տ� �־�� locality ������ �� ����
	if (NULL != _pUsablePodObject)
	{
		pReleasePodObject->_pNextUsablePodObject = _pUsablePodObject;
	}
	_pUsablePodObject = pReleasePodObject;

	return TRUE;
}