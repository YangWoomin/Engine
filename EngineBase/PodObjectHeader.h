#pragma once

using namespace std;

namespace EngineBase
{
	// non pod ��ü�� ��� ���� ����
	enum POD_OBJECT_USE_STATUS : BYTE
	{
		POD_OBJECT_USE_STATUS_NONE = 0,
		POD_OBJECT_USE_STATUS_USED = 1, // ��� ���� ����
		POD_OBJECT_USE_STATUS_USABLE = 2, // ��� ������ ����
		POD_OBJECT_USE_STATUS_RELEASED = 3, // ������ ���� (�޸� ������ ���´ٸ� Ȯ���ϱ� ���� �뵵)
	};

	// ��ŶǮ ���� pod ��ü�� �����ϱ� ���� pod ��ü ���
	template <class PodObject>
	class CPodObjectHeader
	{
	public:
		// ��ü�� ��� ����
		POD_OBJECT_USE_STATUS _podObjectUseStatus;

		// ��üǮ���� ���Ǹ� ��� ������ ���� ��ü�� ����Ű�� ������
		CPodObjectHeader* _pNextUsablePodObject;

		// ������ ����� ��ü ����
		PodObject _podObject;
	};
}