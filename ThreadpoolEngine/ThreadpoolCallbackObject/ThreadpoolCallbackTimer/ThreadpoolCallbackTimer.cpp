#include "Common.h"
#include "ExportCommon.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h"
#include "ThreadpoolCallbackObject.h"
#include "ThreadpoolCallbackTimer.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackTimer::CThreadpoolCallbackTimer()
{
	_pCallbackData = NULL;
	memset(&_fileTime, 0, sizeof(_fileTime));
}

CThreadpoolCallbackTimer::~CThreadpoolCallbackTimer()
{

}

BOOL CThreadpoolCallbackTimer::BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode)
{
	// 콜백 객체가 생성되어 있으면 안 됨, 중복 호출로 간주
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_BOUND;
		return FALSE;
	}

	// 콜백 데이터를 콜백 함수의 매개 변수로 받을 수 있도록 함
	// pTpCallbackEnviron는 NULL일 수도 있어서 검사하지 않음
	_pThreadpoolCallbackObject = ::CreateThreadpoolTimer(CallbackThreadpoolCallbackTimer, (PVOID)this, pTpCallbackEnviron);

	// 실패하면 NULL을 리턴함
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackTimer::ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL != _pThreadpoolCallbackObject)
	{
		::SetThreadpoolTimer(_pThreadpoolCallbackObject, NULL, 0, 0);
		::WaitForThreadpoolTimerCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
		::CloseThreadpoolTimer(_pThreadpoolCallbackObject);
		_pThreadpoolCallbackObject = NULL;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackTimer::ExecuteThreadpoolCallbackTimer(ICallbackData* pCallbackData, FILETIME fileTime, DWORD dwPeriodMillisecond, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	// 콜백 객체가 바인딩되지 않았음
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	// 이전 작업이 완료될 때까지 대기
	::SetThreadpoolTimer(_pThreadpoolCallbackObject, NULL, 0, 0);
	::WaitForThreadpoolTimerCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);

	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode)
	{
		return FALSE;
	}

	// 콜백 데이터 세팅
	_pCallbackData = pCallbackData;

	// 타이머 관련 시간 데이터 세팅
	_fileTime = fileTime;

	// 타이머 개시
	::SetThreadpoolTimer(_pThreadpoolCallbackObject, &_fileTime, dwPeriodMillisecond, 0);

	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode && ERROR_IO_PENDING != (DWORD)errorCode)
	{
		return FALSE;
	}

	return TRUE;
}

VOID CThreadpoolCallbackTimer::CallbackThreadpoolCallbackTimer(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PTP_TIMER pTpTimer)
{
	CThreadpoolCallbackTimer* pThreadpoolCallbackTimer = (CThreadpoolCallbackTimer*)pParam;

	// pThreadpoolCallbackWait의 유효성 검사할 방법이 딱히 없음, 외부에서 관리를 잘 해주는 수 밖에
	pThreadpoolCallbackTimer->CallbackThreadpoolCallbackObject(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE));
}

BOOL CThreadpoolCallbackTimer::CancelThreadpoolCallbackTimer(BOOL bWaitForCallbacks, BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	// 콜백 객체가 바인딩되지 않았음
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	// 더이상 콜백 함수 개시를 실행하지 않도록 처리
	::SetThreadpoolTimer(_pThreadpoolCallbackObject, NULL, 0, 0);

	// 콜백 함수들의 실행이 모두 완료되길 원한다면
	if (FALSE != bWaitForCallbacks)
	{
		::WaitForThreadpoolTimerCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
	}
}