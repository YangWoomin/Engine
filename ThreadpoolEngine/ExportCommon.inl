
using namespace ThreadpoolEngine;

template <class ThreadpoolCallbackObject> 
CThreadpoolCallbackObjectWrapper<ThreadpoolCallbackObject>::CThreadpoolCallbackObjectWrapper(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
{
	_pThreadpoolCallbackObject = new ThreadpoolCallbackObject();
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
	}
	else
	{
		// �ݹ� ��ü�� ������Ǯ�� �׷���
		_pThreadpoolCallbackObject->InitializeThreadpoolCallbackObject(threadpoolGroupParameter, errorCode);
	}
}

template <class ThreadpoolCallbackObject>
CThreadpoolCallbackObjectWrapper<ThreadpoolCallbackObject>::CThreadpoolCallbackObjectWrapper(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
{
	_pThreadpoolCallbackObject = new ThreadpoolCallbackObject(hDevice);
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CREATE_CALLBACK_OBJECT_FAILURE;
	}
	else
	{
		// �ݹ� ��ü�� ������Ǯ�� �׷���
		_pThreadpoolCallbackObject->InitializeThreadpoolCallbackObject(threadpoolGroupParameter, errorCode);
	}
}

template <class ThreadpoolCallbackObject>
CThreadpoolCallbackObjectWrapper<ThreadpoolCallbackObject>::~CThreadpoolCallbackObjectWrapper()
{
	if (NULL != _pThreadpoolCallbackObject)
	{
		// �ݹ� ��ü�� �׷��ε� ������Ǯ���� ����
		_pThreadpoolCallbackObject->FinalizeThreadpoolCallbackObject();

		delete _pThreadpoolCallbackObject;
		_pThreadpoolCallbackObject = NULL;
	}
}