#include "Common.h"
#include "Export.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
//#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackIo.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackIo::CThreadpoolCallbackIo(HANDLE hDevice)
{
	_hDevice = hDevice;
	_pCallbackData = NULL;
}

CThreadpoolCallbackIo::~CThreadpoolCallbackIo()
{

}

BOOL CThreadpoolCallbackIo::BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� �����Ǿ� ������ �� ��, �ߺ� ȣ��� ����
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_BOUND;
		return FALSE;
	}

	// ��ġ�� ���õǾ� �־�� ��
	if (NULL == _hDevice || INVALID_HANDLE_VALUE == _hDevice)
	{
		errorCode = ERROR_CODE_THREADPOOL_INVALID_HANDLE;
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
		bResult = CleanupThreadpoolCallbackIo(fCancelPendingCallbacks, errorCode);

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
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	// ������Ǯ IO ��ü�� �ݹ� �����͸� �߰��� �������� ���ϵ��� ����
	if (NULL != _pCallbackData)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_DATA_ALREADY_SET;
		return FALSE;
	}

	// �ݹ� ������ ����
	_pCallbackData = pCallbackData;

	return TRUE;
}

BOOL CThreadpoolCallbackIo::ExecuteThreadpoolCallbackIo(ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	// �ݹ� �����Ͱ� ���õǾ� �־�� ��
	if (NULL == _pCallbackData)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_DATA_NOT_SET;
		return FALSE;
	}

	// �ݹ� ��ü�� ���ε��� ��ġ�� ��ø ����� ���õ� ���� ����
	::StartThreadpoolIo(_pThreadpoolCallbackObject);

	// ��ø ����� ���� ��� �̻� ���� Ȯ��
	// STATUS_PENDING(ERROR_IO_PENDING)�� ���ϵ� �� ����
	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode && ERROR_IO_PENDING != (DWORD)errorCode)
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
		return;
	}

	if (NULL == pTpIo || NULL == pThreadpoolCallbackIo->GetThreadpoolCallbackObject())
	{
		pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_IS_NULL, (LPOVERLAPPED)pOverlapped, ulIoResult, ulpNumberOfBytesTransferred));
	}
	else
	{
		// �ݹ� �������� �ݹ� �Լ��� ȣ��
		pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE, (LPOVERLAPPED)pOverlapped, ulIoResult, ulpNumberOfBytesTransferred));
	}

	// �ݹ� �Լ� ȣ�� �� �ڵ����� �ݹ� ������ ������ ����ϸ� ����
	if (TRUE == pCallbackData->DeleteCallbackDataAutomatically())
	{
		delete pCallbackData;
	}
}

ICallbackData* CThreadpoolCallbackIo::GetCallbackData()
{
	return _pCallbackData;
}

BOOL CThreadpoolCallbackIo::CleanupThreadpoolCallbackIo(BOOL bCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	//// ��ġ�� ���õǾ� �־�� ��
	//if (NULL == _hDevice || INVALID_HANDLE_VALUE == _hDevice)
	//{
	//	errorCode = ERROR_CODE_THREADPOOL_INVALID_HANDLE;
	//	return FALSE;
	//}

	// ���ε��� ��ġ�� ��� ��ø ������� ���
	BOOL bResult = ::CancelIoEx(_hDevice, NULL);
	if (FALSE == bResult)
	{
		errorCode = (ERROR_CODE)::GetLastError();
	}

	// ���� ��ġ�� ���� ����� ��Ұ� �����ϴ��� IO �۾��� ����ؾ� �ϹǷ�..
	// �ƴϸ� ���ε��� ��ġ�� ����� ���ð� ���� �ʾҴ��� ũ�� ������ ���� �����Ƿ� ȣ����
	::WaitForThreadpoolIoCallbacks(_pThreadpoolCallbackObject, bCancelPendingCallbacks);
	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode && ERROR_IO_PENDING != (DWORD)errorCode)
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL CThreadpoolCallbackIo::CancelThreadpoolCallbackIo(ERROR_CODE& errorCode)
{
	// �ݹ� ��ü�� ���ε����� �ʾ���
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	::CancelThreadpoolIo(_pThreadpoolCallbackObject);
	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode)
	{
		return FALSE;
	}

	return TRUE;
}