#pragma once

using namespace ThreadpoolGroupManager;

// TP_WORK, TP_WAIT, TP_TIMER, TP_IO 4���� �ݹ� ��ü�� �߻� Ÿ��
interface IThreadpoolCallbackObject
{
	//////////////////////////// ��ü ������ ������� ���ÿ� ����� �� ���� �Լ� ////////////////////////////
	// ThreadpoolGroupManager�� ����ϱ� ���� ������ ������ �߻�
	//
	// �ش� �ݹ� ��ü�� TP_CALLBACK_ENVIRON�� ���ε��ϵ��� �����ؾ� ��
	virtual BOOL BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode) = 0;
	//
	// �ش� �ݹ� ��ü�� �����Ǵ� ���� �����ؾ� ��
	virtual BOOL ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode) = 0;
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual ~IThreadpoolCallbackObject() { }
};

// TP_WORK, TP_WAIT, TP_TIMER, TP_IO 4������ ���
template <typename T> 
class CThreadpoolCallbackObject : public IThreadpoolCallbackObject
{
protected:
	// �ݹ� ��ü
	T _pThreadpoolCallbackObject;

	// ������Ǯ �׷쿡 ���������� �߰��Ǿ����� ����
	BOOL _bIsGrouping;

	// �ݹ� ��ü�� ���� �ݹ� �Լ� ȣ�� ���� Ƚ��
	// USHORT _usCallbackRunningCount;

	// �ݹ� ��ü�� ���� �ݹ� �Լ� ȣ�� ���� ������ �ִ� Ƚ��
	// const ULONG _MAXIMUM_CALLBACK_RUNNING_COUNT = USHRT_MAX;

public:
	CThreadpoolCallbackObject() { }

	virtual ~CThreadpoolCallbackObject() { }

	T GetThreadpoolCallbackObject()
	{
		return _pThreadpoolCallbackObject;
	}

	// �ݹ� ��ü ������ ȣ�� �Ŀ� �� �Լ��� ȣ���ؾ� ��
	void Initialize(THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
	{
		_pThreadpoolCallbackObject = NULL;

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

	// �ݹ� ��ü�� �Ҹ��� ȣ�� ���� �� �Լ��� ȣ���ؾ� ��
	void Finalize()
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

	/*
	// �ݹ� �Լ� ȣ�� ���� Ƚ�� ����
	ULONG GetCallbackRunningCount()
	{
		// LONG �ּڰ��� ���õ��� �����Ƿ� ���� ���� Ƚ���� ���� ����
		return ::InterlockedCompareExchange((LONG*)&_usCallbackRunningCount, 0, LONG_MIN);
	}

	// �ݹ� �Լ� ȣ�� ���� Ƚ���� 0���� �ʱ�ȭ
	// WaitForThreadpoolXXXCallbacks ȣ�⿡�� fCancelPendingCallbacks�� TRUE�� �ѱ�� �� �Լ��� ȣ���ؾ� ��
	void ResetCallbackRunningCount()
	{
		::InterlockedExchange((LONG*)&_usCallbackRunningCount, 0);
	}

	// �ݹ� �Լ� ȣ�� ���� Ƚ�� ����
	ULONG IncreaseCallbackRunningCount()
	{
		return ::InterlockedIncrement((LONG*)&_usCallbackRunningCount);
	}

	// �ݹ� �Լ� ȣ�� ���� Ƚ�� ����
	LONG DecreaseCallbackRunningCount()
	{
		// 0 ���ϸ� ������ ����
		LONG lCallbackRunningCount = GetCallbackRunningCount();
		if (0 >= lCallbackRunningCount)
		{
			return lCallbackRunningCount;
		}

		return ::InterlockedDecrement((LONG*)&_usCallbackRunningCount);
	}

	// �ݹ� ������ ����
	// bWaitForPreviousCallback�� �ݹ� ��ü�� ������ ��û�� �ݹ� �Լ� ȣ���� �Ϸ�� �� ������ �� ����
	// fCancelPendingCallbacks�� �ݹ� ��ü�� ������ ��û�� �ݹ� �Լ� ȣ�� ���ø� ����� �� ����
	virtual BOOL SetCallbackData(ICallbackData* pCallbackData, BOOL bWaitForPreviousCallback, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode) = 0;
	*/

	// �ݹ� ��ü�� �ݹ� �Լ� �񵿱� ȣ�� ����
	// 1���� �ݹ� ��ü�� ���ؼ� ��ø���� ������ �� ������ �ݹ� �����Ϳ� �ִ� �ݹ� �Լ��� �񵿱������� ����
	// bWaitForPreviousCallback�� �ݹ� ��ü�� ������ ��û�� �ݹ� �Լ� ȣ���� �Ϸ�� �� ������ �� ����
	// fCancelPendingCallbacks�� �ݹ� ��ü�� ������ ��û�� �ݹ� �Լ� ȣ�� ���ø� ����� �� ����
	// virtual BOOL ExecuteThreadpoolCallbackWork(ERROR_CODE& errorCode, BOOL bWaitForPreviousCallback, BOOL fCancelPendingCallbacks) = 0;
};