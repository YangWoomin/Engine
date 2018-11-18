#pragma once

#include "Common.h"
#include "Singleton.h"
#include "ObjectHeader.h"
#include "Lock.h"

namespace EngineBase
{
	template <class Object>
	class CObjectHeader;

	// �� ��ü(Object)�� �޸� ������ ������ �ؾ� �ϹǷ� �̱����� ����
	template <class Object>
	class CObjectPool final : public CSingletonTemplate<Object, CObjectPool>
	{
	private:
		// chunk (CObjectHeader + Object) 
		typedef CObjectHeader<Object> Chunk;

		// �̱��� ó��
		friend CSingletonTemplate;
		CObjectPool();

		// �޸� ���� �Ҵ�� �� ����
		HANDLE _hHeap;

		// ���� �������� �������κ��� �� ���� ��ȣ�� ���� critical section
		CRITICAL_SECTION _criticalSection;

		// ���� �ڵ�
		ERROR_CODE _errorCode;

		// ��ü���� �Ҵ�� �޸� ��
		PVOID _pBlock;

		// �޸� �� ũ��
		SIZE_T _dwBlockSize;

		// chunk ũ��
		SIZE_T _dwChunkSize;

		// ���� �Ҵ�� ��ü ����
		SIZE_T _dwObjectCount;

		// ���� ��� ���� ��ü ����
		LONG _lCurrentUsedObjectCount;

		// �޸� �� ũ�⸦ Ȯ���� �� �߰��� �Ҵ��� ��ü ����
		SIZE_T _dwAdditoryObjectCount;

		// chunk ����Ʈ���� ��� ������ object�� �ִ� ���� chunk
		Chunk* _pNextUsableChunk;

		// chunk���� ü�̴��ϱ� ���� �Լ�
		void ChainingAllocatedObjects(PVOID pBlock, SIZE_T dwChunkSize, SIZE_T dwBlockSize);

		// ������ ���� ��ü�� �Ҵ��ϴ� �ζ��� �Լ�
		// �� Ŭ���� ��� �Լ������� ���Ǳ� ������ ���ٸ� ���� �˻� ���� ������
		inline BOOL AllocateObject(Object** ppAllocatedObject, SIZE_T dwAllocationCount, ERROR_CODE& errorCode);

		// ������ ���� ��ü�� �����ϴ� �ζ��� �Լ�
		// �� Ŭ���� ��� �Լ������� ���Ǳ� ������ ���ٸ� ���� �˻� ���� ������
		inline BOOL DeallocateObject(Object* pReturningObject, ERROR_CODE& errorCode);

	public:
		~CObjectPool();

		// ���� �޸� �� �Ҵ� �Լ�
		BOOL AllocateInitialBlock(SIZE_T dwInitialObjectCount, SIZE_T dwAdditoryObjectCount, ERROR_CODE& errorCode);

		// ��ü ȹ�� �Լ� (1���� ȹ��)
		BOOL AcquireAllocatedObject(Object** ppAllocatedObject, ERROR_CODE& errorCode);

		// ��ü ��ȯ �Լ� (1���� ��ȯ)
		BOOL ReturnAllocatedObject(Object* pReturningObject, ERROR_CODE& errorCode);

		// ��ü ȹ�� �Լ� (2�� �̻� ȹ��)
		BOOL AcquireAllocatedMultipleObjects(Object** ppAllocatedMultipleObjects, SIZE_T dwAllocationCount, ERROR_CODE& errorCode);

		// ��ü ��ȯ �Լ� (2�� �̻� ��ȯ)
		BOOL ReturnAllocatedMultipleObjects(Object* pReturningMultipleObjects, ERROR_CODE& errorCode);
	};
}

#include "ObjectPool.inl"