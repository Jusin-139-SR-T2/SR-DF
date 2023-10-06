#pragma once
#include "GameObject.h"
#include "Export_System.h"
#include "Export_Utility.h"


class CAceObjectFactory
{
public:
	enum class OBJECT_CLASS {
		FOOD,  // 완 - 블랙보드 나오면 추가구성
		WEAPON, // 완 - 블랙보드 나오면 추가구성
		INTERACTION, // 완 - 블랙보드 나오면 추가구성
		THROW, // 완 - 블랙보드 나오면 추가구성
		BUILDING,  // 하는중 - 10.06.05

		OBJECT_CLASSEND
	};

	enum class OBJECT_DIRECTION {
		EAST, SOUTHEAST,
		SOUTH, SOUTHWEST, 
		WEST, NORTHWEST, 
		NORTH, NORTHEAST, 
		DIR_END
	};

	static Engine::CGameObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, 
										OBJECT_CLASS objClass, 
										const _tchar* pObjTag, 
										const _float _fx, 
										const _float _fy, 
										const _float _fz,
										const _float iCnt = 0.f,
										CAceObjectFactory::OBJECT_DIRECTION pDirection = CAceObjectFactory::OBJECT_DIRECTION::SOUTH);
};
