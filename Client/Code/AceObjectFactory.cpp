#include "stdafx.h"
#include "AceObjectFactory.h"

#include "AceFood.h"
#include "AceWeapon.h"
#include "AceThrow.h"
#include "AceInteraction.h"

Engine::CGameObject* CAceObjectFactory::Create
(LPDIRECT3DDEVICE9 pGraphicDev, 
    OBJECT_CLASS objClass, 
    const _tchar* pObjTag, 
    const _float _fx, 
    const _float _fy, 
    const _float _fz, 
    const _float iCnt)
{
    Engine::CGameObject* pInstance = nullptr;

    switch (objClass)
    {
    case CAceObjectFactory::OBJECT_CLASS::FOOD:
        pInstance = CAceFood::Create(pGraphicDev, pObjTag, _fx, _fy, _fz);
        return pInstance;

    case CAceObjectFactory::OBJECT_CLASS::WEAPON:
        pInstance = CAceWeapon::Create(pGraphicDev, pObjTag, _fx, _fy, _fz);
        return pInstance;

    case CAceObjectFactory::OBJECT_CLASS::INTERACTION:
        pInstance = CAceInteraction::Create(pGraphicDev, pObjTag, _fx, _fy, _fz);
        return pInstance;
        break;

    case CAceObjectFactory::OBJECT_CLASS::THROW:
        pInstance = CAceThrow::Create(pGraphicDev, pObjTag, _fx, _fy, _fz, iCnt);
        return pInstance;
    }

    return pInstance;
}
