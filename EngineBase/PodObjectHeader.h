#pragma once

using namespace std;

namespace EngineBase
{
	// non pod 객체의 사용 가능 상태
	enum POD_OBJECT_USE_STATUS : BYTE
	{
		POD_OBJECT_USE_STATUS_NONE = 0,
		POD_OBJECT_USE_STATUS_USED = 1, // 사용 중인 상태
		POD_OBJECT_USE_STATUS_USABLE = 2, // 사용 가능한 상태
		POD_OBJECT_USE_STATUS_RELEASED = 3, // 해제된 상태 (메모리 덤프로 남는다면 확인하기 위한 용도)
	};

	// 패킷풀 등의 pod 객체를 관리하기 위한 pod 객체 헤더
	template <class PodObject>
	class CPodObjectHeader
	{
	public:
		// 객체의 사용 상태
		POD_OBJECT_USE_STATUS _podObjectUseStatus;

		// 객체풀에서 사용되며 사용 가능한 다음 객체를 가리키는 포인터
		CPodObjectHeader* _pNextUsablePodObject;

		// 실제로 사용할 객체 공간
		PodObject _podObject;
	};
}