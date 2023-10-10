#pragma once

#include "Engine_Define.h"



enum ESERIALIZE_TYPE
{
	ESERIALIZE_PROJECT,
	ESERIALIZE_SCENE,
	ESERIALIZE_LAYER,
	ESERIALIZE_OBJECT,
	ESERIALIZE_TERRAIN,
	ESERIALIZE_COMPONENT,
	ESERIALIZE_LIGHT,
	ESERIALIZE_SIZE
};

/// <summary>
/// ��� ����ȭ �����Ͱ� ���������� ������ ��� ������
/// </summary>
class FSeialize_Header
{
public:
	ESERIALIZE_TYPE		strType;		// Ÿ�� �±�
	string				strName;		// �̸� �±�
};

class FSeialize_Vector3
{
public:
	_float		x;
	_float		y;
	_float		z;
};