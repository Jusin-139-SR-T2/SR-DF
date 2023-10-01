#pragma once
#include "GameObject.h"


class CAceObjectFactory : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CAceObjectFactory)

public:
enum class OBJECT_CLASS { 
	FOOD,  // 완 - 블랙보드 나오면 추가구성
	WEAPON, // 완 - 블랙보드 나오면 추가구성
	INTERACTION, // 완 - 블랙보드 나오면 추가구성
	THROW, // 하는중 - 블랙보드 나오면 추가구성

	OBJECT_CLASSEND };

public:
	static Engine::CGameObject* Create
	(LPDIRECT3DDEVICE9 pGraphicDev, 
		OBJECT_CLASS objClass, 
		const _tchar* pObjTag, 
		const _float _fx, 
		const _float _fy, 
		const _float _fz,
		const _float iCnt = 0.f);
};
