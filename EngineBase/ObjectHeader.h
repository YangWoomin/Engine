#pragma once

#include "Common.h"

namespace EngineBase
{
	// 객체풀의 객체를 관리하기 위한 객체 헤더
	template <class Object>
	class CObjectHeader
	{
	public:
		// 사용 가능한 다음 객체 헤더를 가리키는 포인터
		CObjectHeader* _pNextUsableObjectHeader;

		// 실제로 사용할 객체의 공간을 가리키는 포인터
		// 이 헤더 영역 다음으로 객체 영역이 존재함
		Object* _pObject;
	};
}