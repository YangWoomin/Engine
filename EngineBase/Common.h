#pragma once

#include <Windows.h>

#include <iostream>

#include <map>
#include <set>
#include <list>
#include <queue>

namespace EngineBase
{
	// ������Ʈ ���ο��� ����ϴ� ����/���� ���� �ڵ�
	// ������Ʈ ���ο��� ������ ���� �ڵ�� INT_MIN �̸��� ������ ���
	// ����� DWORD GetLastError, int WSAGetLastError,  LONG NTSTATUS �ڵ�� ���
	enum ERROR_CODE : __int64
	{
		// ���� �ƴ� (����)
		ERROR_CODE_NONE
		= 0,

		// ������Ʈ ������ ���� �ڵ� ���ذ�
		ERROR_CODE_BASE
		= INT_MIN, // -2147483647 - 1

				   // �� ���� ������ ����
		ERROR_CODE_HEAP_CREATION_FAILED
		= ERROR_CODE_BASE - 1,

		// �޸� ���� �̹� �Ҵ��
		ERROR_CODE_MEMORY_BLOCK_ALREADY_ALLOCATED
		= ERROR_CODE_BASE - 2,

		// �޸� ���� �Ҵ���� ����
		ERROR_CODE_MEMORY_BLOCK_NEVER_ALLOCATED
		= ERROR_CODE_BASE - 3,

		// ��ȿ���� ���� pod ��ü ����
		ERROR_CODE_INVALID_POD_OBJECT_COUNT
		= ERROR_CODE_BASE - 4,

		// ��� ������ pod ��ü�� ����
		ERROR_CODE_NO_USABLE_POD_OBJECT
		= ERROR_CODE_BASE - 5,

		// ��üǮ�� ���� ���ִ� ����
		ERROR_CODE_FULL_POD_OBJECTS_IN_MEMORY_BLOCK
		= ERROR_CODE_BASE - 6,

		// ��ȿ���� ���� pod ��ü�� ��ȯ
		ERROR_CODE_INVALID_POD_OBJECT
		= ERROR_CODE_BASE - 7,
	};
}
