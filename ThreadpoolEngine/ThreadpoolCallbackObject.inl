
using namespace ThreadpoolEngine;

template<typename TP_OBJECT>
CThreadpoolCallbackObject<TP_OBJECT>::CThreadpoolCallbackObject()
{
	_pThreadpoolCallbackObject = NULL;
	_bIsGrouping = FALSE;
	_pCallbackData = NULL;
}

template<typename TP_OBJECT>
CThreadpoolCallbackObject<TP_OBJECT>::~CThreadpoolCallbackObject()
{
	
}

template<typename TP_OBJECT>
TP_OBJECT CThreadpoolCallbackObject<TP_OBJECT>::GetThreadpoolCallbackObject()
{
	return _pThreadpoolCallbackObject;
}

template<typename TP_OBJECT>
void CThreadpoolCallbackObject<TP_OBJECT>::InitializeThreadpoolCallbackObject(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
{
	// 특정 그룹의 스레드풀 생성, 없으면 생성하고 있으면 그냥 쓰면 됨
	ThreadpoolGroupManager()->CreateThreadpool(threadpoolGroupParameter._dwThreadpoolGroup, threadpoolGroupParameter._iMinThreadCount, threadpoolGroupParameter._iMaxThreadCount, errorCode);

	// 스레드풀 특정 그룹에 콜백 객체를 추가
	if (ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS == errorCode || ERROR_CODE_NONE == errorCode)
	{
		// 스레드풀 그룹 매니저에 의해 BindThreadpoolCallbackObject 호출됨
		errorCode = ERROR_CODE_NONE;
		_bIsGrouping = ThreadpoolGroupManager()->InsertThreadpoolCallbackObject(threadpoolGroupParameter._dwThreadpoolGroup, this, errorCode);
	}
}

template<typename TP_OBJECT>
void CThreadpoolCallbackObject<TP_OBJECT>::FinalizeThreadpoolCallbackObject()
{
	// 특정 그룹의 스레드풀에 추가되었다면 제거
	if (TRUE == _bIsGrouping)
	{
		// 스레드풀 그룹 매니저에 의해 ReleaseThreadpoolCallbackObject 호출됨
		ERROR_CODE errorCode = ERROR_CODE_NONE;
		ThreadpoolGroupManager()->DeleteThreadpoolCallbackObject(this, FALSE, errorCode);
		_bIsGrouping = FALSE;
	}

	// 생성했던 스레드 풀은 여기서 제거하지 않도록 함
	// 스레드풀 그룹 매니저가 프로그램 종료할 때 일괄적으로 자동 제거
}

template<typename TP_OBJECT>
void CThreadpoolCallbackObject<TP_OBJECT>::CallbackThreadpoolCallbackObject(CALLBACK_DATA_PARAMETER callbackDataParameter)
{
	if (NULL != _pCallbackData)
	{
		// 콜백 객체가 바인딩되지 않았음
		if (NULL == _pThreadpoolCallbackObject)
		{
			callbackDataParameter.SetErrorCode(ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND);
		}

		_pCallbackData->CallbackFunction(callbackDataParameter);

		// 콜백 함수 호출 후 자동으로 콜백 데이터 해제를 허용하면 해제
		if (TRUE == _pCallbackData->DeleteCallbackDataAutomatically())
		{
			delete _pCallbackData;
		}
	}
}