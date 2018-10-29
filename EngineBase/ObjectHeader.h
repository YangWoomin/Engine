#pragma once

#include "Common.h"

namespace EngineBase
{
	// 객체풀에 있는 객체의 사용 가능 상태
	enum OBJECT_USE_STATUS : BYTE
	{
		OBJECT_USE_STATUS_NONE			= 0,
		OBJECT_USE_STATUS_USED			= 1, // 사용 중인 상태
		OBJECT_USE_STATUS_USABLE		= 2, // 사용 가능한 상태
		OBJECT_USE_STATUS_RELEASED		= 3, // 해제된 상태 (메모리 덤프로 남는다면 확인하기 위한 용도)
	};

	// 객체풀의 객체를 관리하기 위한 객체 래퍼
	template <class Object>
	class CObjectHeader
	{
	public:
		// 실제로 사용할 객체의 공간을 가리키는 포인터
		// 이 헤더 영역 다음으로 객체 영역이 존재함
		Object* _pObject;

		// 객체의 사용 상태
		OBJECT_USE_STATUS _ObjectUseStatus;

		// 사용 가능한 다음 객체 헤더를 가리키는 포인터
		CObjectHeader* _pNextUsableObjectHeader;
	};
}