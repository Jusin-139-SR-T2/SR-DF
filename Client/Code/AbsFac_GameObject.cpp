#include "stdafx.h"
#include "AbsFac_GameObject.h"

#include "Player.h"
#include "Brown.h"
#include "Gray.h"
#include "AceBoss.h"
#include "AceFood.h"
#include "AceWeapon.h"
#include "AceThrow.h"
#include "AceInteraction.h"
#include "AceBuilding.h"

#include "Serialize_BaseClass.h"

CGameObject* CAbsFac_GameObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObject)
{
	EGO_CLASS eClassType = tObject.strID;

	switch (eClassType)
	{
	case Engine::ECLASS_NONE:
		break;
	case Engine::ECLASS_PLAYER:
	{
		FPlayer_Create tCreate;
		tCreate.vPos = tObject.vPos;
		tCreate.vRot = tObject.vRotation;
		tCreate.vScale = tObject.vScale;
		tCreate.fPriority[0] = tObject.fPriority_Update;
		tCreate.fPriority[1] = tObject.fPriority_LateUpdate;
		tCreate.fPriority[2] = tObject.fPriority_Render;
		return static_cast<CGameObject*>(CPlayer::Create(pGraphicDev, tCreate));
		break;
	}
	case Engine::ECLASS_BROWN:

		break;
	case Engine::ECLASS_GREY:

		break;
	case Engine::ECLASS_BOSS:

		break;
	case Engine::ECLASS_FOOD:
		//return static_cast<CGameObject*>(CAceFood::Create(pGraphicDev));
		break;
	case Engine::ECLASS_WEAPON:

		break;
	case Engine::ECLASS_THROW:

		break;
	case Engine::ECLASS_INTERACTION:

		break;
	case Engine::ECLASS_BUILDING:
		//return static_cast<CGameObject*>(CAceBuilding::Create(pGraphicDev));
		break;
	default:
		break;
	}


	return static_cast<CGameObject*>(nullptr);
}
