
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
		// 콜백 객체를 스레드풀에 그루핑
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
		// 콜백 객체를 스레드풀에 그루핑
		_pThreadpoolCallbackObject->InitializeThreadpoolCallbackObject(threadpoolGroupParameter, errorCode);
	}
}

template <class ThreadpoolCallbackObject>
CThreadpoolCallbackObjectWrapper<ThreadpoolCallbackObject>::~CThreadpoolCallbackObjectWrapper()
{
	if (NULL != _pThreadpoolCallbackObject)
	{
		// 콜백 객체를 그루핑된 스레드풀에서 제거
		_pThreadpoolCallbackObject->FinalizeThreadpoolCallbackObject();

		delete _pThreadpoolCallbackObject;
		_pThreadpoolCallbackObject = NULL;
	}
}