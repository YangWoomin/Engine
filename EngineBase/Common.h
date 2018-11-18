#pragma once

#include <Windows.h>

#include <iostream>

#include <mutex>

#include <map>
#include <set>
#include <list>
#include <queue>
#include <vector>
#include <deque>

namespace EngineBase
{
	// 프로젝트 내부에서 사용하는 공용/전용 에러 코드
	// 프로젝트 내부에서 정의한 에러 코드는 INT_MIN 미만의 음수만 사용
	// 양수는 DWORD GetLastError, int WSAGetLastError,  LONG NTSTATUS 코드로 사용
	enum ERROR_CODE : __int64
	{
		// 에러 아님 (정상)
		ERROR_CODE_NONE
		= 0,

		// 프로젝트 내부의 에러 코드 기준값
		ERROR_CODE_BASE
		= MINLONGLONG, // ~(0x7fffffffffffffff)

		// 힙 영역 생성에 실패
		ERROR_CODE_HEAP_CREATION_FAILED
		= ERROR_CODE_BASE + 1,

		// 메모리 블럭이 이미 할당됨
		ERROR_CODE_MEMORY_BLOCK_ALREADY_ALLOCATED
		= ERROR_CODE_BASE + 2,

		// 메모리 블럭이 할당되지 않음
		ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED
		= ERROR_CODE_BASE + 3,

		// 유효하지 않은 객체 개수
		ERROR_CODE_INVALID_OBJECT_COUNT
		= ERROR_CODE_BASE + 4,

		// 사용 가능한 객체가 없음
		ERROR_CODE_NO_USABLE_OBJECT
		= ERROR_CODE_BASE + 5,

		// 객체풀이 가득 차있는 상태
		ERROR_CODE_FULL_OBJECTS_IN_MEMORY_BLOCK
		= ERROR_CODE_BASE + 6,

		// 유효하지 않은 객체의 반환
		ERROR_CODE_INVALID_OBJECT_RETURN
		= ERROR_CODE_BASE + 7,

		// ::HeapFree 함수 호출에 실패
		ERROR_CODE_FAILURE_IN_HEAP_FREE_FUNCTION
		= ERROR_CODE_BASE + 8,

		// 엔진 정의 에러 코드 최댓값
		ERROR_CODE_MAX
		= ERROR_CODE_BASE + MAXINT
	};
}