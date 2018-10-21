#pragma once

using namespace std;

namespace EngineBase
{
	template <class PodObject> class CPodObjectHeader;

	// CPodObjectPool 클래스는 PodObject가 pod 타입만 허용함
	template <class PodObject, typename Enable = void>
	class CPodObjectPool;

	// CPodObjectHeader 클래스 타입 검사
	// #define CHECK_POD_OBJECT_CLASS_TYPE typename enable_if<is_base_of<CPodObjectHeader, PodObject>::value>::type

	// PodObject가 실제로 pod 타입인지 검사
#define CHECK_POD_OBJECT_TYPE typename enable_if<is_pod<PodObject>::value>::type

	template <class PodObject>
	class CPodObjectPool <PodObject, CHECK_POD_OBJECT_TYPE>
	{
	private:
		// 객체들이 할당될 힙 영역의 핸들
		HANDLE _hHeap;

		// 객체풀 블럭의 시작 포인터
		PVOID _pPool;

		// 객체풀 크기
		SIZE_T _dwPoolSize;

		// 객체(CPodObjectHeader) 크기
		SIZE_T _dwObjectSize;

		// 객체 개수
		SIZE_T _dwObjectCount;

		// 사용 가능한 객체 리스트의 첫 번째 객체
		CPodObjectHeader<PodObject>* _pUsablePodObject;

	public:
		CPodObjectPool(ERROR_CODE& errorCode);
		~CPodObjectPool();

		// CPodObjectPool 객체를 생성한 후 객체풀을 생성하기 위한 함수
		BOOL CreatePodObjectPool(SIZE_T dwPodObjectAllocationCount, ERROR_CODE& errorCode);

		// 사용 가능한 객체를 획득
		BOOL AcquireAllocatedPodObject(PodObject** ppPodObject, ERROR_CODE& errorCode);

		// 사용한 객체를 반환
		BOOL ReleaseAllocatedPodObject(PodObject* pPodObject, ERROR_CODE& errorCode);
	};
}

#include "PodObjectPool.inl"