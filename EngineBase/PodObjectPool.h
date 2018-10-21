#pragma once

using namespace std;

namespace EngineBase
{
	template <class PodObject> class CPodObjectHeader;

	// CPodObjectPool Ŭ������ PodObject�� pod Ÿ�Ը� �����
	template <class PodObject, typename Enable = void>
	class CPodObjectPool;

	// CPodObjectHeader Ŭ���� Ÿ�� �˻�
	// #define CHECK_POD_OBJECT_CLASS_TYPE typename enable_if<is_base_of<CPodObjectHeader, PodObject>::value>::type

	// PodObject�� ������ pod Ÿ������ �˻�
#define CHECK_POD_OBJECT_TYPE typename enable_if<is_pod<PodObject>::value>::type

	template <class PodObject>
	class CPodObjectPool <PodObject, CHECK_POD_OBJECT_TYPE>
	{
	private:
		// ��ü���� �Ҵ�� �� ������ �ڵ�
		HANDLE _hHeap;

		// ��üǮ ���� ���� ������
		PVOID _pPool;

		// ��üǮ ũ��
		SIZE_T _dwPoolSize;

		// ��ü(CPodObjectHeader) ũ��
		SIZE_T _dwObjectSize;

		// ��ü ����
		SIZE_T _dwObjectCount;

		// ��� ������ ��ü ����Ʈ�� ù ��° ��ü
		CPodObjectHeader<PodObject>* _pUsablePodObject;

	public:
		CPodObjectPool(ERROR_CODE& errorCode);
		~CPodObjectPool();

		// CPodObjectPool ��ü�� ������ �� ��üǮ�� �����ϱ� ���� �Լ�
		BOOL CreatePodObjectPool(SIZE_T dwPodObjectAllocationCount, ERROR_CODE& errorCode);

		// ��� ������ ��ü�� ȹ��
		BOOL AcquireAllocatedPodObject(PodObject** ppPodObject, ERROR_CODE& errorCode);

		// ����� ��ü�� ��ȯ
		BOOL ReleaseAllocatedPodObject(PodObject* pPodObject, ERROR_CODE& errorCode);
	};
}

#include "PodObjectPool.inl"