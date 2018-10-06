#pragma once

#include <Windows.h>

#include <iostream>

#include <map>
#include <set>
#include <list>
#include <queue>

/*
// 프로젝트 내부에서 사용하는 공용/전용 에러 코드
// 프로젝트 내부에서 정의한 에러 코드는 음수만 사용하며 양수는 DWORD GetLastError 코드로 사용
enum ERROR_CODE : __int64
{
	// 에러 아님 (정상)
	ERROR_CODE_NONE
	= 0, 

	// 공용 에러 //
	// 이미 존재
	ERROR_CODE_ALREADY_EXISTS
	= -1, 

	// 존재하지 않음
	ERROR_CODE_DOES_NOT_EXIST
	= -2, 

	// 유효하지 않은 접근
	ERROR_CODE_INVALID_ACCESS
	= -3, 

	// CThreadpoolCallbackWork 전용 에러 코드 //
	// 콜백 객체는 이미 바인딩 되었음
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_ALREADY_BOUND
	= -101,	

	// 콜백 객체의 콜백 함수가 실행중
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_FUNCTION_RUNNING
	= -102, 

	// 콜백 객체의 콜백 함수로 넘겨질 콜백 데이터가 세팅되지 않음
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_CALLBACK_DATA_NOT_SET
	= -103,
	
	// 콜백 객체가 바인딩된 적이 없음
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_NOT_BOUND
	= -104, 

	// 콜백 객체가 NULL인 상태
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_PTP_WORK_IS_NULL
	= -105, 

	// 콜백 함수가 호출되기 전에 콜백 객체가 해제됨
	ERROR_CODE_THREADPOOL_CALLBACK_WORK_RELEASE_CALLBACK_OBJECT
	= -106,


	// CThreadpoolCallbackIo 전용 에러 코드 //
	// 콜백 객체는 이미 바인딩 되었음
	ERROR_CODE_THREADPOOL_CALLBACK_IO_ALREADY_BOUND
	= -401,

	// 콜백 객체의 콜백 함수가 실행중
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_FUNCTION_RUNNING
	= -402,

	// 콜백 객체의 콜백 함수로 넘겨질 콜백 데이터가 세팅되지 않음
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_NOT_SET
	= -403,

	// 콜백 객체가 바인딩된 적이 없음
	ERROR_CODE_THREADPOOL_CALLBACK_IO_NOT_BOUND
	= -404,

	// 콜백 객체가 NULL인 상태
	ERROR_CODE_THREADPOOL_CALLBACK_IO_PTP_IO_IS_NULL
	= -405,

	// IOCP와 바인딩할 장치가 유효하지 않음
	ERROR_CODE_THREADPOOL_CALLBACK_IO_INVALID_DEVICE
	= -410,

	// 콜백 데이터가 이미 세팅되었음
	ERROR_CODE_THREADPOOL_CALLBACK_IO_CALLBACK_DATA_ALREADY_SET
	= -411,
};
*/