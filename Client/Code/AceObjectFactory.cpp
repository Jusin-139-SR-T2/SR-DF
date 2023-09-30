#include "stdafx.h"
#include "AceObjectFactory.h"

#include "AceFood.h"
#include "AceWeapon.h"

CAceObjectFactory::CAceObjectFactory(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CAceObjectFactory::CAceObjectFactory(const CAceObjectFactory& rhs)
	: Base(rhs)
{
}

CAceObjectFactory::~CAceObjectFactory()
{
}

Engine::CGameObject* CAceObjectFactory::Create(LPDIRECT3DDEVICE9 pGraphicDev, OBJECT_CLASS objClass, const _tchar* pObjTag)
{
    Engine::CGameObject* pInstance = nullptr;


    switch (objClass)
    {
    case CAceObjectFactory::OBJECT_CLASS::FOOD:
        pInstance = CAceFood::Create(pGraphicDev, pObjTag);
        return pInstance;

    case CAceObjectFactory::OBJECT_CLASS::WEAPON:
        pInstance = CAceWeapon::Create(pGraphicDev, pObjTag);
        return pInstance;

    case CAceObjectFactory::OBJECT_CLASS::DISPLAY:
        break;

    case CAceObjectFactory::OBJECT_CLASS::INTERACTION:
        break;

    case CAceObjectFactory::OBJECT_CLASS::KEY:
        break;

    case CAceObjectFactory::OBJECT_CLASS::THROW_WEAPON:
        break;
    }

    return pInstance;
}
