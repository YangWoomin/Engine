#pragma once

#include "Common.h"
#include "ObjectPool.h"

namespace EngineBase
{
	// ��üǮ���� ���� ��ü �ֻ��� base Ŭ����
	// ChildClass ��ü�� ��üǮ�� ��ü�� �����
	template <class ChildClass>
	class CObjectInPool
	{
	public:
		// ��üǮ ��� ����
		// AllocateObjectPool�� ���� ��üǮ ���� �� TRUE ����
		static BOOL _bUseObjectPool;

	protected:
		CObjectInPool();
		virtual ~CObjectInPool();

	public:
		// ��üǮ ���� �Լ�
		static BOOL AllocateObjectPool(SIZE_T dwInitialObjectCount, SIZE_T dwAdditoryObjectCount, ERROR_CODE& errorCode);

		// ��üǮ�κ��� �޸� ���� �Ҵ�޴� operator new
		void* operator new(size_t dwObjectSize, ERROR_CODE& errorCode);

		// ��üǮ�� �޸� ���� �ݳ��ϴ� operator delete
		void operator delete(void* pObjectAddress, ERROR_CODE& errorCode);

		// ��üǮ�κ��� �޸� ���� �Ҵ�޴� operator new[]
		void* operator new[](size_t dwAllocationSize, ERROR_CODE& errorCode);

		// ��üǮ�� �޸� ���� �ݳ��ϴ� operator delete[]
		void operator delete[](void* pObjectAddress, ERROR_CODE& errorCode);
	};
}

#include "ObjectInPool.inl"