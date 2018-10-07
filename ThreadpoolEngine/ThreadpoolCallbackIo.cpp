#include "Common.h"
#include "Export.h"
#include "Utilities.h"
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
#include "ThreadpoolGroupManager.h" //
//#include "ThreadpoolCallbackWork.h" //
#include "ThreadpoolCallbackIo.h"

using namespace ThreadpoolGroupManager;

CThreadpoolCallbackIo::CThreadpoolCallbackIo(HANDLE hDevice, THREADPOOL_GROUP_PARAMETER threadpoolGroupParameter, ERROR_CODE& errorCode)
{
	_pThreadpoolCallbackObject = NULL;
	_hDevice = hDevice;
	_pCallbackData = NULL;

	// Ư�� �׷��� ������Ǯ ����, ������ �����ϰ� ������ �׳� ���� ��
	// _errorCode = ERROR_CODE_NONE;
	GetThreadpoolManager()->CreateThreadpool(threadpoolGroupParameter._dwThreadpoolGroup, threadpoolGroupParameter._iMinThreadCount, threadpoolGroupParameter._iMaxThreadCount, errorCode);

	// ������Ǯ Ư�� �׷쿡 �ݹ� ��ü�� �߰�
	if (ERROR_CODE_THREADPOOL_THREADPOOL_GROUP_ALREADY_EXISTS == errorCode || ERROR_CODE_NONE == errorCode)
	{
		errorCode = ERROR_CODE_NONE;
		_bIsGrouping = GetThreadpoolManager()->InsertThreadpoolCallbackObject(threadpoolGroupParameter._dwThreadpoolGroup, this, errorCode);
	}
}

CThreadpoolCallbackIo::~CThreadpoolCallbackIo()
{
	// Ư�� �׷��� ������Ǯ�� �߰��Ǿ��ٸ� ����
	if (TRUE == _bIsGrouping)
	{
		ERROR_CODE errorCode = ERROR_CODE_NONE;
		GetThreadpoolManager()->DeleteThreadpoolCallbackObject(this, FALSE, errorCode);
		_bIsGrouping = FALSE;
	}
}

BOOL CThreadpoolCallbackIo::BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� �����Ǿ� ������ �� ��, �ߺ� ȣ��� ����
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_ALREADY_BOUND;
		return FALSE;
	}

	// ��ġ�� ���õǾ� �־�� ��
	if (NULL == _hDevice || INVALID_HANDLE_VALUE == _hDevice)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_INVALID_DEVICE;
		return FALSE;
	}

	// �ݹ� �����͸� �ݹ� �Լ��� �Ű� ������ ���� �� �ֵ��� ��
	// pTpCallbackEnviron�� NULL�� ���� �־ �˻����� ����
	_pThreadpoolCallbackObject = ::CreateThreadpoolIo(_hDevice, ThreadpoolCallbackIoCallbackFunction, (PVOID)this, pTpCallbackEnviron);
	
	// �����ϸ� NULL�� ������, ���� ��ġ�� ���� ���� �� 
	// OVERLAPPED�� ��ø ����� ��ġ�� �������� �ʾ��� ��� ::GetLastError()���� 87 (ERROR_INVALID_PARAMETER) ����
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackIo::ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	BOOL bResult = FALSE;

	if (NULL != _pThreadpoolCallbackObject)
	{
		bResult = CancelThreadpoolCallbackIo(fCancelPendingCallbacks, errorCode);

		// ���ε��� ��ġ�� ��� ��ø ����� ����� ����� �����ϰ� �ݹ� ��ü�� �����Ǿ�� ��
		// ::WaitForThreadpoolIoCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
		::CloseThreadpoolIo(_pThreadpoolCallbackObject);
		_pThreadpoolCallbackObject = NULL;
	}

	return bResult;
}

// �ݹ� ������ ����
BOOL CThreadpoolCallbackIo::SetCallbackData(ICallbackData* pCallbackData, ERROR_CODE& errorCode)
{
	errorCode = ERROR_CODE_NONE;

	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_NOT_BOUND;
		return FALSE;
	}

	// ������Ǯ IO ��ü�� �ݹ� �����͸� �߰��� �������� ���ϵ��� ����
	if (NULL != _pCallbackData)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_ALREADY_SET;
		return FALSE;
	}

	/*
	// ���� �ݹ� �Լ� ȣ�� ���õ� Ƚ���� 0���� ũ�ٸ�
	if (0 < GetCallbackRunningCount())
	{
		// ��ٸ��� �ʴ´ٸ� ���� ó��
		if (FALSE == bWaitForPreviousCallback)
		{
			errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_FUNCTION_RUNNING;
			return FALSE;
		}

		// ���� �ݹ� �Լ��� �۾��� ��� ����Ѵٸ�
		if (TRUE == fCancelPendingCallbacks)
		{
			
		}

		// ���� �ݹ� �Լ� ȣ���� �Ϸ�� ������ ���
		::WaitForThreadpoolWorkCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);

		// ���� �ݹ� �Լ��� �۾��� ��� ����Ѵٸ�
		if (TRUE == fCancelPendingCallbacks)
		{
			// �ݹ� �Լ� ȣ�� ���� Ƚ���� 0���� �ʱ�ȭ
			ResetCallbackRunningCount();
		}
	}
	*/

	// �ݹ� ������ ����
	_pCallbackData = pCallbackData;

	return TRUE;
}

BOOL CThreadpoolCallbackIo::ExecuteThreadpoolCallbackIo(ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_NOT_BOUND;
		return FALSE;
	}

	//// ��ġ�� ���õǾ� �־�� ��
	//if (NULL == _hDevice || INVALID_HANDLE_VALUE == _hDevice)
	//{
	//	errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_INVALID_DEVICE;
	//	return FALSE;
	//}

	// �ݹ� �����Ͱ� ���õǾ� �־�� ��
	if (NULL == _pCallbackData)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_NOT_SET;
		return FALSE;
	}

	// ��ø ����� ����
	::StartThreadpoolIo(_pThreadpoolCallbackObject);

	// ��ø ����� ���� ��� �̻� ���� Ȯ��
	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode)
	{
		return FALSE;
	}

	return TRUE;
}

VOID CThreadpoolCallbackIo::ThreadpoolCallbackIoCallbackFunction(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PVOID pOverlapped, ULONG ulIoResult, ULONG_PTR ulpNumberOfBytesTransferred, PTP_IO pTpIo)
{
	CThreadpoolCallbackIo* pThreadpoolCallbackIo = (CThreadpoolCallbackIo*)pParam;

	// pThreadpoolCallbackIo�� ��ȿ�� �˻��� ����� ���� ����, �ܺο��� ������ �� ���ִ� �� �ۿ�
	ICallbackData* pCallbackData = pThreadpoolCallbackIo->GetCallbackData();
	// �ݹ� �����Ͱ� NULL�̸� �ƹ��͵� ����
	if (NULL == pCallbackData)
	{
		//// �ݹ� �Լ��� ȣ�� ���� Ƚ���� ����
		//pThreadpoolCallbackIo->DecreaseCallbackRunningCount();
		return;
	}

	if (NULL == pTpIo || NULL == pThreadpoolCallbackIo->GetThreadpoolCallbackObject())
	{
		pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_THREADPOOL_CALLBACK_IO_PTP_IO_IS_NULL, 0, (LPOVERLAPPED)pOverlapped, ulIoResult, ulpNumberOfBytesTransferred));
	}
	else
	{
		// �ݹ� �������� �ݹ� �Լ��� ȣ��
		pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE, 0, (LPOVERLAPPED)pOverlapped, ulIoResult, ulpNumberOfBytesTransferred));
	}

	// �ݹ� �Լ� ȣ�� �� �ڵ����� �ݹ� ������ ������ ����ϸ� ����
	if (TRUE == pCallbackData->DeleteCallbackDataAutomatically())
	{
		delete pCallbackData;
	}

	//// �ݹ� �Լ��� ȣ�� ���� Ƚ���� ����
	//pThreadpoolCallbackIo->DecreaseCallbackRunningCount();
}

ICallbackData* CThreadpoolCallbackIo::GetCallbackData()
{
	return _pCallbackData;
}

BOOL CThreadpoolCallbackIo::CancelThreadpoolCallbackIo(BOOL bCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_NOT_BOUND;
		return FALSE;
	}

	//// ��ġ�� ���õǾ� �־�� ��
	//if (NULL == _hDevice || INVALID_HANDLE_VALUE == _hDevice)
	//{
	//	errorCode = ERROR_CODE_THREADPOOL_CALLBACK_IO_INVALID_DEVICE;
	//	return FALSE;
	//}

	// ���ε��� ��ġ�� ��� ��ø ������� ���
	BOOL bResult = ::CancelIoEx(_hDevice, NULL);
	if (FALSE == bResult)
	{
		errorCode = (ERROR_CODE)::GetLastError();
	}

	::WaitForThreadpoolIoCallbacks(_pThreadpoolCallbackObject, bCancelPendingCallbacks);
	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode)
	{
		bResult = FALSE;
	}

	return bResult;
}