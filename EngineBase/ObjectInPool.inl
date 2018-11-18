
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
		// �޸� ���� ȹ�� ��û
		ChildClass* pAllocationObject = NULL;
		if (FALSE != pObjectPool->AcquireAllocatedObject(&pAllocationObject, errorCode))
		{
			return static_cast<void*>(pAllocationObject);
		}
	}

	// ��üǮ ��뿡 �����ߴٸ�
	// ����Ʈ ������ �ش� ũ���� �޸� ������ �Ҵ�
	return new BYTE[dwObjectSize];
}

template <class ChildClass>
void CObjectInPool<ChildClass>::operator delete(void* pObjectAddress, ERROR_CODE& errorCode)
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

	// ��üǮ�� �ݳ��� ������ ����
	// ���ʿ� ��üǮ ��ü�� ������ ������ ����Ʈ ���� ����ϹǷ� ����Ʈ ������ �����ϴ� ���� �������� ���� ��������
	// �� �Լ� ���ο��� ::HeapFree ���� ������ ����� ����Ʈ ������ �����ϸ� �� ��
	// �޸� ������ ���� �� �����Ƿ� ������ �����ؾ� ��
	if (ERROR_CODE_FAILURE_IN_HEAP_FREE_FUNCTION == errorCode)
	{
		// ���⼭ GetLastError�� ȣ���Ͽ� ���� �ڵ带 ��ȯ
		errorCode = (ERROR_CODE)::GetLastError();
		return;
	}
	
	// �� ���� ���� ����Ʈ ������ �Ҵ��� ����̹Ƿ� ����Ʈ ������ ����
	delete static_cast<BYTE*>(pObjectAddress);
}

template <class ChildClass>
void* CObjectInPool<ChildClass>::operator new[](size_t dwAllocationSize, ERROR_CODE& errorCode)
{
	CObjectPool<ChildClass>* pObjectPool = CObjectPool<ChildClass>::GetInstance(errorCode);
	if (NULL != pObjectPool && ERROR_CODE_NONE == errorCode)
	{
		// �޸� ���� ȹ�� ��û
		ChildClass* pAllocationObject = NULL;
		if (FALSE != pObjectPool->AcquireAllocatedMultipleObjects(&pAllocationObject, dwAllocationSize, errorCode))
		{
			return static_cast<void*>(pAllocationObject);
		}
	}

	// ��üǮ ��뿡 �����ߴٸ�
	// ����Ʈ ������ �ش� ũ���� �޸� ������ �Ҵ�
	return new BYTE[dwAllocationSize * sizeof(ChildClass)];
}

template <class ChildClass>
void CObjectInPool<ChildClass>::operator delete[](void* pObjectAddress, ERROR_CODE& errorCode)
{
	CObjectPool<ChildClass>* pObjectPool = CObjectPool<ChildClass>::GetInstance(errorCode);
	if (NULL != pObjectPool && ERROR_CODE_NONE == errorCode)
	{
		// �޸� ���� �ݳ� ��û
		if (FALSE != pObjectPool->ReturnAllocatedMultipleObjects(static_cast<ChildClass*>(pObjectAddress), errorCode))
		{
			// ���� �ݳ� �Ϸ�
			return;
		}
	}

	// ��üǮ�� �ݳ��� ������ ����
	// ���ʿ� ��üǮ ��ü�� ������ ������ ����Ʈ ���� ����ϹǷ� ����Ʈ ������ �����ϴ� ���� �������� ���� ��������
	// �� �Լ� ���ο��� ::HeapFree ���� ������ ����� ����Ʈ ������ �����ϸ� �� ��
	// �޸� ������ ���� �� �����Ƿ� ������ �����ؾ� ��
	if (ERROR_CODE_FAILURE_IN_HEAP_FREE_FUNCTION == errorCode)
	{
		// ���⼭ GetLastError�� ȣ���Ͽ� ���� �ڵ带 ��ȯ
		errorCode = (ERROR_CODE)::GetLastError();
		return;
	}

	// ���� ��üǮ �Ҵ��� ���� �ʾҰų� ��üǮ�� �ݳ��� ������ ����
	// ����Ʈ ���� ����ϴ� ���̹Ƿ� ����Ʈ �����κ��� ���� ����
	delete[] static_cast<BYTE*>(pObjectAddress);
}