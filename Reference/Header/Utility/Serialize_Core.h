#pragma once

#include "Engine_Define.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

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
class ENGINE_DLL FSeialize_Header
{
public:
	ESERIALIZE_TYPE		strType;		// Ÿ�� �±�
	string				strName;		// �̸� �±�
};

class ENGINE_DLL FSeialize_Vector3
{
public:
	_float		x;
	_float		y;
	_float		z;
};