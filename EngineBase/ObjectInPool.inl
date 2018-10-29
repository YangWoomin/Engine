
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
	// ��ü Ǯ�� ����Ѵٸ�
	if (FALSE != _bUseObjectPool)
	{
		CObjectPool<ChildClass>* pObjectPool = CObjectPool<ChildClass>::GetInstance(errorCode);
		if (NULL != pObjectPool && ERROR_CODE_NONE == errorCode)
		{
			// �޸� ���� ȹ�� ��û
			ChildClass* pAllocationObject = NULL;
			if (FALSE != pObjectPool->AcquireAllocatedObject(&pAllocationObject, errorCode))
			{
				return static_cast<void*>(pAllocationObject);
			}
		}
	}

	// ���� ��üǮ �Ҵ��� ���� �ʾҰų� ��üǮ ��뿡 �����ߴٸ�
	// ����Ʈ ������ �ش� ũ���� �޸� ������ �Ҵ�
	return new BYTE[dwObjectSize];
}

template <class ChildClass>
void CObjectInPool<ChildClass>::operator delete(void* pObjectAddress, ERROR_CODE& errorCode)
{
	// ��ü Ǯ�� ����Ѵٸ�
	if (FALSE != _bUseObjectPool)
	{
		CObjectPool<ChildClass>* pObjectPool = CObjectPool<ChildClass>::GetInstance(errorCode);
		if (NULL != pObjectPool && ERROR_CODE_NONE == errorCode)
		{
			// �޸� ���� �ݳ� ��û
			if (FALSE != pObjectPool->ReturnAllocatedObject(static_cast<ChildClass*>(pObjectAddress), errorCode))
			{
				// ���� �ݳ� �Ϸ�
				return;
			}
		}
	}

	// ���� ��üǮ �Ҵ��� ���� �ʾҰų� ��üǮ�� �ݳ��� ������ ����
	// ����Ʈ ���� ����ϴ� ���̹Ƿ� ����Ʈ �����κ��� ���� ����
	delete static_cast<BYTE*>(pObjectAddress);
}

template <class ChildClass>
void* CObjectInPool<ChildClass>::operator new[](size_t dwAllocationSize, ERROR_CODE& errorCode)
{
	// ��ü Ǯ�� ����Ѵٸ�
	if (FALSE != _bUseObjectPool)
	{
		// ��üǮ���� AcquireAllocatedObject�� �ݺ� ȣ���ϸ� ���� ������ �����ߴµ�
		// linear�� �޸� �����̾�� �ϴ� chunk ����Ʈ ������ ���� ��üǮ�� ����ϱ� ����� ��Ȳ
		// ���� ����� �ִ��� ������..
	}

	// ���� ��üǮ �Ҵ��� ���� �ʾҰų� ��üǮ ��뿡 �����ߴٸ�
	// ����Ʈ ������ �ش� ũ���� �޸� ������ �Ҵ�
	return new BYTE[dwAllocationSize];
}

template <class ChildClass>
void CObjectInPool<ChildClass>::operator delete[](void* pObjectAddress, ERROR_CODE& errorCode)
{
	// ��ü Ǯ�� ����Ѵٸ�
	if (FALSE != _bUseObjectPool)
	{
		// ���� ����� �ִ��� ������..
	}

	// ���� ��üǮ �Ҵ��� ���� �ʾҰų� ��üǮ�� �ݳ��� ������ ����
	// ����Ʈ ���� ����ϴ� ���̹Ƿ� ����Ʈ �����κ��� ���� ����
	delete static_cast<BYTE*>(pObjectAddress);
}