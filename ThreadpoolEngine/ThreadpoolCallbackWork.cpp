#include "Common.h"
#include "Export.h"
#include "Utilities.h"
#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackObject.h"
//#include "CallbackData.h"
//#include "ThreadpoolGroupManager.h" //
#include "ThreadpoolCallbackWork.h"

using namespace ThreadpoolEngine;
using namespace ThreadpoolGroupManager;

CThreadpoolCallbackWork::CThreadpoolCallbackWork()
{
	::InitializeCriticalSection(&_criticalSection);
}

CThreadpoolCallbackWork::~CThreadpoolCallbackWork()
{
	::DeleteCriticalSection(&_criticalSection);
}

BOOL CThreadpoolCallbackWork::BindThreadpoolCallbackObject(const PTP_CALLBACK_ENVIRON pTpCallbackEnviron, ERROR_CODE& errorCode)
{
	// 콜백 객체가 생성되어 있으면 안 됨, 중복 호출로 간주
	if (NULL != _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_ALREADY_BOUND;
		return FALSE;
	}

	// 콜백 데이터를 콜백 함수의 매개 변수로 받을 수 있도록 함
	// pTpCallbackEnviron는 NULL일 수도 있어서 검사하지 않음
	_pThreadpoolCallbackObject = ::CreateThreadpoolWork(CallbackThreadpoolCallbackWork, (PVOID)this, pTpCallbackEnviron);

	// 실패하면 NULL을 리턴함
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = (ERROR_CODE)::GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackWork::ReleaseThreadpoolCallbackObject(BOOL fCancelPendingCallbacks, ERROR_CODE& errorCode)
{
	if (NULL != _pThreadpoolCallbackObject)
	{
		::WaitForThreadpoolWorkCallbacks(_pThreadpoolCallbackObject, fCancelPendingCallbacks);
		::CloseThreadpoolWork(_pThreadpoolCallbackObject);
		_pThreadpoolCallbackObject = NULL;
	}

	return TRUE;
}

BOOL CThreadpoolCallbackWork::ExecuteThreadpoolCallbackWork(ICallbackData* pCallbackData, ERROR_CODE& errorCode)
{
	// 콜백 객체가 바인딩되지 않았음
	if (NULL == _pThreadpoolCallbackObject)
	{
		errorCode = ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_NOT_BOUND;
		return FALSE;
	}

	{
		CLock lock(_criticalSection);
		// 콜백 데이터 큐에 콜백 데이터를 추가
		_callbackDataQueue.push(pCallbackData);
	}

	// 콜백 함수 호출 개시
	::SubmitThreadpoolWork(_pThreadpoolCallbackObject);

	// submit 결과 이상 여부 확인
	// STATUS_PENDING(ERROR_IO_PENDING)이 리턴될 수 있음
	errorCode = (ERROR_CODE)::GetLastError();
	if (ERROR_CODE_NONE != errorCode && ERROR_IO_PENDING != (DWORD)errorCode)
	{
		return FALSE;
	}

	return TRUE;
}

VOID CThreadpoolCallbackWork::CallbackThreadpoolCallbackWork(PTP_CALLBACK_INSTANCE pInstance, PVOID pParam, PTP_WORK pTpWork)
{
	CThreadpoolCallbackWork* pThreadpoolCallbackWork = (CThreadpoolCallbackWork*)pParam;

	// pThreadpoolCallbackWork의 유효성 검사할 방법이 딱히 없음, 외부에서 관리를 잘 해주는 수 밖에
	// 가장 먼저 push한 콜백 데이터부터 처리
	ICallbackData* pCallbackData = pThreadpoolCallbackWork->PopCallbackDataFromQueue();
	if (NULL != pCallbackData)
	{
		if (NULL == pThreadpoolCallbackWork->GetThreadpoolCallbackObject())
		{
			pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_THREADPOOL_CALLBACK_OBJECT_IS_NULL));
		}
		else
		{
			// 콜백 데이터의 콜백 함수를 호출
			pCallbackData->CallbackFunction(CALLBACK_DATA_PARAMETER(ERROR_CODE_NONE));
		}

		// 콜백 함수 호출 후 자동으로 콜백 데이터 해제를 허용하면 해제
		if (TRUE == pCallbackData->DeleteCallbackDataAutomatically())
		{
			delete pCallbackData;
		}
	}
}

ICallbackData* CThreadpoolCallbackWork::PopCallbackDataFromQueue()
{
	ICallbackData* pCallbackData = NULL;

	{
		CLock lock(_criticalSection);
		if (false == _callbackDataQueue.empty())
		{
			pCallbackData = _callbackDataQueue.front();
			_callbackDataQueue.pop();
		}
	}

	return pCallbackData;
}