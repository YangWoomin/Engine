
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
	// Ư�� �׷��� ������Ǯ ����, ������ �����ϰ� ������ �׳� ���� ��
	ThreadpoolGroupManager()->CreateThreadpool(threadpoolGroupParameter._dwThreadpoolGroup, threadpoolGroupParameter._iMinThreadCount, threadpoolGroupParameter._iMaxThreadCount, errorCode);

	// ������Ǯ Ư�� �׷쿡 �ݹ� ��ü�� �߰�
	if (ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS == errorCode || ERROR_CODE_NONE == errorCode)
	{
		// ������Ǯ �׷� �Ŵ����� ���� BindThreadpoolCallbackObject ȣ���
		errorCode = ERROR_CODE_NONE;
		_bIsGrouping = ThreadpoolGroupManager()->InsertThreadpoolCallbackObject(threadpoolGroupParameter._dwThreadpoolGroup, this, errorCode);
	}
}

template<typename PTP_OBJECT>
void CThreadpoolCallbackObject<PTP_OBJECT>::FinalizeThreadpoolCallbackObject()
{
	// Ư�� �׷��� ������Ǯ�� �߰��Ǿ��ٸ� ����
	if (TRUE == _bIsGrouping)
	{
		// �� �����忡 ���õǾ� �ִ� �ݹ� ��ü �ν��Ͻ��� �� Ŭ���� �ν��Ͻ��� �ݹ� ��ü�� ����
		// �ݹ� �ν��Ͻ� ���� ���õǾ� �ִٸ� �ݹ� �Լ� ���ο��� �ݹ� (����) ��ü�� �����ϴ� ���̹Ƿ� ������� ���ƾ� ��
		if (_pThreadpoolCallbackObject == _pThreadpoolCallbackObjectInstance && NULL != _pTpCallbackInstance)
		{
			::DisassociateCurrentThreadFromCallback(_pTpCallbackInstance);
			_pTpCallbackInstance = NULL;
			_pThreadpoolCallbackObjectInstance = NULL;
		}

		// ������Ǯ �׷� �Ŵ����� ���� ReleaseThreadpoolCallbackObject ȣ���
		ERROR_CODE errorCode = ERROR_CODE_NONE;
		ThreadpoolGroupManager()->DeleteThreadpoolCallbackObject(this, FALSE, errorCode);
		_bIsGrouping = FALSE;
	}

	// �����ߴ� ������ Ǯ�� ���⼭ �������� �ʵ��� ��
	// ������Ǯ �׷� �Ŵ����� ���α׷� ������ �� �ϰ������� �ڵ� ����
}

template<typename PTP_OBJECT>
void CThreadpoolCallbackObject<PTP_OBJECT>::CallbackThreadpoolCallbackObject(CALLBACK_DATA_PARAMETER callbackDataParameter, PTP_CALLBACK_INSTANCE pTpCallbackInstance)
{
	// �����帶�� �ݹ� ��ü�� �����ϰ� �ִ� �޸� ������ �ش� �ݹ� ��ü�� ����
	_pThreadpoolCallbackObjectInstance = _pThreadpoolCallbackObject;

	// �����帶�� �ݹ� �ν��Ͻ��� �����ϰ� �ִ� �޸� ������ ���޹��� �ݹ� �ν��Ͻ��� ����
	_pTpCallbackInstance = pTpCallbackInstance;

	if (NULL != _pCallbackData)
	{
		// �ݹ� �Լ� ȣ��
		_pCallbackData->CallbackFunction(callbackDataParameter);

		// �ݹ� �Լ� ȣ�� �� �ڵ����� �ݹ� ������ ������ ����ϸ� ����
		if (TRUE == _pCallbackData->DeleteCallbackDataAutomatically())
		{
			delete _pCallbackData;
		}
	}

	// �ݹ� �Լ� ȣ���� �Ϸ�Ǹ� �ٽ� NULL�� ����
	_pTpCallbackInstance = NULL;

	// �ݹ� �Լ� ȣ���� �Ϸ�Ǹ� �ٽ� NULL�� ����
	_pThreadpoolCallbackObjectInstance = NULL;
}