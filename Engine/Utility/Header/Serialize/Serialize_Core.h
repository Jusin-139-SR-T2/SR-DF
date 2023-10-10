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
/// 모든 직렬화 데이터가 공통적으로 가지는 헤더 데이터
/// </summary>
class FSeialize_Header
{
public:
	ESERIALIZE_TYPE		strType;		// 타입 태그
	string				strName;		// 이름 태그
};

class FSeialize_Vector3
{
public:
	_float		x;
	_float		y;
	_float		z;
};