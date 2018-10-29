#pragma once

#include "Common.h"

namespace EngineBase
{
	// ��üǮ�� �ִ� ��ü�� ��� ���� ����
	enum OBJECT_USE_STATUS : BYTE
	{
		OBJECT_USE_STATUS_NONE			= 0,
		OBJECT_USE_STATUS_USED			= 1, // ��� ���� ����
		OBJECT_USE_STATUS_USABLE		= 2, // ��� ������ ����
		OBJECT_USE_STATUS_RELEASED		= 3, // ������ ���� (�޸� ������ ���´ٸ� Ȯ���ϱ� ���� �뵵)
	};

	// ��üǮ�� ��ü�� �����ϱ� ���� ��ü ����
	template <class Object>
	class CObjectHeader
	{
	public:
		// ������ ����� ��ü�� ������ ����Ű�� ������
		// �� ��� ���� �������� ��ü ������ ������
		Object* _pObject;

		// ��ü�� ��� ����
		OBJECT_USE_STATUS _ObjectUseStatus;

		// ��� ������ ���� ��ü ����� ����Ű�� ������
		CObjectHeader* _pNextUsableObjectHeader;
	};
}