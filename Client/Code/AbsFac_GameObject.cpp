#include "stdafx.h"
#include "AbsFac_GameObject.h"

#include "Player.h"
#include "Brown.h"
#include "Gray.h"
#include "Boss.h"
#include "AceFood.h"
#include "AceWeapon.h"
#include "AceThrow.h"
#include "AceInteraction.h"
#include "AceBuilding.h"

#include "Serialize_BaseClass.h"

CGameObject* CAbsFac_GameObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObject)
{
	EGO_CLASS eClassType = tObject.eID;

	switch (eClassType)
	{
	case Engine::ECLASS_NONE:
		break;
	case Engine::ECLASS_PLAYER:
		return static_cast<CGameObject*>(CPlayer::Create(pGraphicDev, tObject));
		break;
	case Engine::ECLASS_BROWN:
		return static_cast<CGameObject*>(CBrown::Create(pGraphicDev, tObject));
		break;
	case Engine::ECLASS_GREY:
		return static_cast<CGameObject*>(CGray::Create(pGraphicDev, tObject));
		break;
	case Engine::ECLASS_BOSS:
		return static_cast<CGameObject*>(CBoss::Create(pGraphicDev, tObject));
		break;
	case Engine::ECLASS_FOOD:
		return static_cast<CGameObject*>(CAceFood::Create(pGraphicDev, tObject));
		break;
	case Engine::ECLASS_WEAPON:
		return static_cast<CGameObject*>(CAceWeapon::Create(pGraphicDev, tObject));
		break;
	case Engine::ECLASS_THROW:
		return static_cast<CGameObject*>(CAceThrow::Create(pGraphicDev, tObject));
		break;
	case Engine::ECLASS_INTERACTION:
		return static_cast<CGameObject*>(CAceInteraction::Create(pGraphicDev, tObject));
		break;
	case Engine::ECLASS_BUILDING:
		return static_cast<CGameObject*>(CAceBuilding::Create(pGraphicDev, tObject));
		break;
	default:
		break;
	}

	return static_cast<CGameObject*>(nullptr);
}
