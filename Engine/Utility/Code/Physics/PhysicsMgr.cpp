#include "PhysicsMgr.h"

IMPLEMENT_SINGLETON(CPhysicsMgr)

CPhysicsMgr::CPhysicsMgr()
{

}

CPhysicsMgr::~CPhysicsMgr()
{
	Free();
}

void CPhysicsMgr::Free()
{
	for (auto item : m_vecWorld3D)
	{
		Safe_Release(item);
	}
}

HRESULT CPhysicsMgr::Ready_Physics(const _uint iMaxPhysicsWorld3D)
{
	m_vecWorld3D.reserve(iMaxPhysicsWorld3D);
	m_vecWorld3D.clear();

	for (_uint i = 0; i < m_vecWorld3D.capacity(); i++)
	{
		m_vecWorld3D.push_back(CPhysicsWorld3D::Create(1000, 1000));
	}

	return S_OK;
}

void CPhysicsMgr::StartFrame_Physics()
{
	for (auto iter = m_vecWorld3D.begin(); iter != m_vecWorld3D.end(); ++iter)
	{
		(*iter)->StartFrame_Physics();
	}
}

_int CPhysicsMgr::Update_Physics(const Real& fTimeDelta)
{
	for (auto iter = m_vecWorld3D.begin(); iter != m_vecWorld3D.end(); ++iter)
	{
		(*iter)->Update_Physics(fTimeDelta);
	}

	return 0;
}

list<CColliderComponent*> CPhysicsMgr::IntersectTests(const _uint iWorldID, _vec3 vPos, CColliderComponent* pSrc)
{
	list<CColliderComponent*> listColliderComp;
	list<FCollisionPrimitive*> listCollision;

	listCollision = m_vecWorld3D[iWorldID]->Test_Contacts(pSrc->Get_Shape());
	for (auto iter = listCollision.begin(); iter != listCollision.end(); ++iter)
	{
		if (CColliderComponent* pObj = static_cast<CColliderComponent*>((*iter)->Get_Owner()))
			listColliderComp.push_back(pObj);
	}

	return listColliderComp;
}

list<CColliderComponent*> CPhysicsMgr::IntersectTests_Sphere(const _uint iWorldID, _vec3 vPos, _float fRadius)
{
	FCollisionSphere pShape;
	pShape.vPos = FVector3(vPos.x, vPos.y, vPos.z);
	pShape.fRadius = fRadius;

	list<CColliderComponent*> listColliderComp;
	list<FCollisionPrimitive*> listCollision;

	listCollision = m_vecWorld3D[iWorldID]->Test_Contacts(static_cast<FCollisionPrimitive*>(&pShape));
	for (auto iter = listCollision.begin(); iter != listCollision.end(); ++iter)
	{
		if (CColliderComponent* pObj = static_cast<CColliderComponent*>((*iter)->Get_Owner()))
			listColliderComp.push_back(pObj);
	}

	return listColliderComp;
}

list<CColliderComponent*> CPhysicsMgr::IntersectTests_Box(const _uint iWorldID, _vec3 vPos, _vec3 vHalfSize)
{
	FCollisionBox pShape;
	pShape.vPos = FVector3(vPos.x, vPos.y, vPos.z);
	pShape.vHalfSize = FVector3(vHalfSize.x, vHalfSize.y, vHalfSize.z);

	list<CColliderComponent*> listColliderComp;
	list<FCollisionPrimitive*> listCollision;

	listCollision = m_vecWorld3D[iWorldID]->Test_Contacts(static_cast<FCollisionPrimitive*>(&pShape));
	for (auto iter = listCollision.begin(); iter != listCollision.end(); ++iter)
	{
		if (CColliderComponent* pObj = static_cast<CColliderComponent*>((*iter)->Get_Owner()))
			listColliderComp.push_back(pObj);
	}

	return listColliderComp;
}

list<CColliderComponent*> CPhysicsMgr::IntersectTests_Capsule(const _uint iWorldID, _vec3 vPos, _vec3 vNormal, _float fRadius)
{
	FCollisionCapsule pShape;
	pShape.vPos = FVector3(vPos.x, vPos.y, vPos.z);
	pShape.vDirHalfSize = FVector3(vNormal.x, vNormal.y, vNormal.z);
	pShape.fRadius = fRadius;

	list<CColliderComponent*> listColliderComp;
	list<FCollisionPrimitive*> listCollision;

	listCollision = m_vecWorld3D[iWorldID]->Test_Contacts(static_cast<FCollisionPrimitive*>(&pShape));
	for (auto iter = listCollision.begin(); iter != listCollision.end(); ++iter)
	{
		if (CColliderComponent* pObj = static_cast<CColliderComponent*>((*iter)->Get_Owner()))
			listColliderComp.push_back(pObj);
	}

	return listColliderComp;
}

list<CGameObject*> CPhysicsMgr::IntersectTests_GetGameObject(const _uint iWorldID, _vec3 vPos, CColliderComponent* pSrc)
{
	list<CGameObject*> listGameObject;
	list<CColliderComponent*> listCollierComp;

	listCollierComp = IntersectTests(iWorldID, vPos, pSrc);
	for (auto iter = listCollierComp.begin(); iter != listCollierComp.end(); ++iter)
	{
		if (CGameObject* pObj = static_cast<CGameObject*>((*iter)->Get_Owner()))
			listGameObject.push_back(pObj);
	}

	return listGameObject;
}

list<CGameObject*> CPhysicsMgr::IntersectTests_Sphere_GetGameObject(const _uint iWorldID, _vec3 vPos, _float fRadius)
{
	list<CGameObject*> listGameObject;
	list<CColliderComponent*> listCollierComp;

	listCollierComp = IntersectTests_Sphere(iWorldID, vPos, fRadius);
	for (auto iter = listCollierComp.begin(); iter != listCollierComp.end(); ++iter)
	{
		if (CGameObject* pObj = static_cast<CGameObject*>((*iter)->Get_Owner()))
			listGameObject.push_back(pObj);
	}

	return listGameObject;
}

list<CGameObject*> CPhysicsMgr::IntersectTests_Box_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vHalfSize)
{
	list<CGameObject*> listGameObject;
	list<CColliderComponent*> listCollierComp;

	listCollierComp = IntersectTests_Box(iWorldID, vPos, vHalfSize);
	for (auto iter = listCollierComp.begin(); iter != listCollierComp.end(); ++iter)
	{
		if (CGameObject* pObj = static_cast<CGameObject*>((*iter)->Get_Owner()))
			listGameObject.push_back(pObj);
	}

	return listGameObject;
}

list<CGameObject*> CPhysicsMgr::IntersectTests_Capsule_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vNormal, _float fRadius)
{
	list<CGameObject*> listGameObject;
	list<CColliderComponent*> listCollierComp;

	listCollierComp = IntersectTests_Capsule(iWorldID, vPos, vNormal, fRadius);
	for (auto iter = listCollierComp.begin(); iter != listCollierComp.end(); ++iter)
	{
		if (CGameObject* pObj = static_cast<CGameObject*>((*iter)->Get_Owner()))
			listGameObject.push_back(pObj);
	}

	return listGameObject;
}
