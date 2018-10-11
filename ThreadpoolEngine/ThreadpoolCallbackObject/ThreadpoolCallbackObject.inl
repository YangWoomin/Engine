
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

template<typename TP_OBJECT>
void CThreadpoolCallbackObject<TP_OBJECT>::FinalizeThreadpoolCallbackObject()
{
	// Ư�� �׷��� ������Ǯ�� �߰��Ǿ��ٸ� ����
	if (TRUE == _bIsGrouping)
	{
		// ������Ǯ �׷� �Ŵ����� ���� ReleaseThreadpoolCallbackObject ȣ���
		ERROR_CODE errorCode = ERROR_CODE_NONE;
		ThreadpoolGroupManager()->DeleteThreadpoolCallbackObject(this, FALSE, errorCode);
		_bIsGrouping = FALSE;
	}

	// �����ߴ� ������ Ǯ�� ���⼭ �������� �ʵ��� ��
	// ������Ǯ �׷� �Ŵ����� ���α׷� ������ �� �ϰ������� �ڵ� ����
}

template<typename TP_OBJECT>
void CThreadpoolCallbackObject<TP_OBJECT>::CallbackThreadpoolCallbackObject(CALLBACK_DATA_PARAMETER callbackDataParameter)
{
	if (NULL != _pCallbackData)
	{
		// �ݹ� ��ü�� ���ε����� �ʾ���
		if (NULL == _pThreadpoolCallbackObject)
		{
			callbackDataParameter.SetErrorCode(ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND);
		}

		_pCallbackData->CallbackFunction(callbackDataParameter);

		// �ݹ� �Լ� ȣ�� �� �ڵ����� �ݹ� ������ ������ ����ϸ� ����
		if (TRUE == _pCallbackData->DeleteCallbackDataAutomatically())
		{
			delete _pCallbackData;
		}
	}
}