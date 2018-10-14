
using namespace ThreadpoolEngine;

template<typename PTP_OBJECT>
PTP_OBJECT CThreadpoolCallbackObject<PTP_OBJECT>::_pThreadpoolCallbackObjectInstance = NULL;

template<typename PTP_OBJECT>
PTP_CALLBACK_INSTANCE CThreadpoolCallbackObject<PTP_OBJECT>::_pTpCallbackInstance = NULL;

template<typename PTP_OBJECT>
CThreadpoolCallbackObject<PTP_OBJECT>::CThreadpoolCallbackObject()
{
	_pThreadpoolCallbackObject = NULL;
	_bIsGrouping = FALSE;
	_pCallbackData = NULL;
}

template<typename PTP_OBJECT>
CThreadpoolCallbackObject<PTP_OBJECT>::~CThreadpoolCallbackObject()
{
	
}

template<typename PTP_OBJECT>
PTP_OBJECT CThreadpoolCallbackObject<PTP_OBJECT>::GetThreadpoolCallbackObject()
{
	return _pThreadpoolCallbackObject;
}

template<typename PTP_OBJECT>
void CThreadpoolCallbackObject<PTP_OBJECT>::InitializeThreadpoolCallbackObject(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
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

template<typename PTP_OBJECT>
void CThreadpoolCallbackObject<PTP_OBJECT>::FinalizeThreadpoolCallbackObject()
{
	// 특정 그룹의 스레드풀에 추가되었다면 제거
	if (TRUE == _bIsGrouping)
	{
		// 이 스레드에 세팅되어 있는 콜백 객체 인스턴스가 이 클래스 인스턴스의 콜백 객체와 같고
		// 콜백 인스턴스 값이 세팅되어 있다면 콜백 함수 내부에서 콜백 (래퍼) 객체를 해제하는 것이므로 데드락을 막아야 함
		if (_pThreadpoolCallbackObject == _pThreadpoolCallbackObjectInstance && NULL != _pTpCallbackInstance)
		{
			::DisassociateCurrentThreadFromCallback(_pTpCallbackInstance);
			_pTpCallbackInstance = NULL;
			_pThreadpoolCallbackObjectInstance = NULL;
		}

		// 스레드풀 그룹 매니저에 의해 ReleaseThreadpoolCallbackObject 호출됨
		ERROR_CODE errorCode = ERROR_CODE_NONE;
		ThreadpoolGroupManager()->DeleteThreadpoolCallbackObject(this, FALSE, errorCode);
		_bIsGrouping = FALSE;
	}

	// 생성했던 스레드 풀은 여기서 제거하지 않도록 함
	// 스레드풀 그룹 매니저가 프로그램 종료할 때 일괄적으로 자동 제거
}

template<typename PTP_OBJECT>
void CThreadpoolCallbackObject<PTP_OBJECT>::CallbackThreadpoolCallbackObject(CALLBACK_DATA_PARAMETER callbackDataParameter, PTP_CALLBACK_INSTANCE pTpCallbackInstance)
{
	// 스레드마다 콜백 객체를 저장하고 있는 메모리 영역을 해당 콜백 객체로 세팅
	_pThreadpoolCallbackObjectInstance = _pThreadpoolCallbackObject;

	// 스레드마다 콜백 인스턴스를 저장하고 있는 메모리 영역을 전달받은 콜백 인스턴스로 세팅
	_pTpCallbackInstance = pTpCallbackInstance;

	if (NULL != _pCallbackData)
	{
		// 콜백 함수 호출
		_pCallbackData->CallbackFunction(callbackDataParameter);

		// 콜백 함수 호출 후 자동으로 콜백 데이터 해제를 허용하면 해제
		if (TRUE == _pCallbackData->DeleteCallbackDataAutomatically())
		{
			delete _pCallbackData;
		}
	}

	// 콜백 함수 호출이 완료되면 다시 NULL로 리셋
	_pTpCallbackInstance = NULL;

	// 콜백 함수 호출이 완료되면 다시 NULL로 리셋
	_pThreadpoolCallbackObjectInstance = NULL;
}