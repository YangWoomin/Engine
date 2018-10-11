#include "Common.h"
#include "ExportCommon.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h"
#include "ThreadpoolCallbackObject.h"
#include "ThreadpoolCallbackWait.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackWait::CThreadpoolCallbackWait()
{
	_hObject = INVALID_HANDLE_VALUE;
	memset(&_fileTime, 0, sizeof(_fileTime));
}

CThreadpoolCallbackWait::~CThreadpoolCallbackWait()
{
	
}

BOOL CThreadpoolCallbackWait::BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode)
{
	// 콜백 객체가 생성되어 있으면 안 됨, 중복 호출로 간주
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_BOUND;
		return FALSE;
	}

	// 콜백 데이터를 콜백 함수의 매개 변수로 받을 수 있도록 함
	// pTpCallbackEnviron는 NULL일 수도 있어서 검사하지 않음
	_pThreadpoolCallbackObject = ::CreateThreadpoolWait(CallbackThreadpoolCallbackWait, (PVOID)this, pTpCallbackEnviron);

	// 실패하면 NULL을 리턴함
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackWait::ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL != _pThreadpoolCallbackObject)
	{
		::WaitForThreadpoolWaitCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
		::CloseThreadpoolWait(_pThreadpoolCallbackObject);
		_pThreadpoolCallbackObject = NULL;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackWait::ExecuteThreadpoolCallbackWait(HANDLE hObject, ICallbackData* pCallbackData, FILETIME fileTime, BOOL bInfinite, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	// 콜백 객체가 바인딩되지 않았음
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	// 이전 작업이 완료될 때까지 대기
	::WaitForThreadpoolWaitCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);

	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode)
	{
		return FALSE;
	}

	// 동기화 객체 세팅
	_hObject = hObject;

	// 콜백 데이터 세팅
	_pCallbackData = pCallbackData;

	// 시간 제한이 있다면 관련 데이터 세팅
	if (FALSE == bInfinite)
	{
		_fileTime = fileTime;
		::SetThreadpoolWait(_pThreadpoolCallbackObject, _hObject, &_fileTime);
	}
	// 무한 대기라면
	else
	{
		::SetThreadpoolWait(_pThreadpoolCallbackObject, _hObject, NULL);
	}

	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode && ERROR_IO_PENDING != (DWORD)errorCode)
	{
		return FALSE;
	}

	return TRUE;
}

VOID CThreadpoolCallbackWait::CallbackThreadpoolCallbackWait(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PTP_WAIT pTpWait, TP_WAIT_RESULT tpWaitResult)
{
	CThreadpoolCallbackWait* pThreadpoolCallbackWait = (CThreadpoolCallbackWait*)pParam;

	// pThreadpoolCallbackWait의 유효성 검사할 방법이 딱히 없음, 외부에서 관리를 잘 해주는 수 밖에
	pThreadpoolCallbackWait->CallbackThreadpoolCallbackObject(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE, tpWaitResult, pThreadpoolCallbackWait->GetObjectHandle()));
}

HANDLE CThreadpoolCallbackWait::GetObjectHandle()
{
	return _hObject;
}