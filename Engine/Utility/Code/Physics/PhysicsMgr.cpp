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

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_GetGameObject(const _uint iWorldID, FCollisionPrimitive* pSrc)
{
	list_collide_test listCollision;
	list<pair<CGameObject*, FContact>> listGameObject;

	listCollision = m_vecWorld3D[iWorldID]->Test_Contacts(static_cast<FCollisionPrimitive*>(pSrc));
	for (auto iter = listCollision.begin(); iter != listCollision.end(); ++iter)
	{
		CColliderComponent* pCollideComp = static_cast<CColliderComponent*>((*iter).first->Get_Owner());
		CGameObject* pObj = static_cast<CGameObject*>(pCollideComp->Get_Owner());
		listGameObject.push_back(pair<CGameObject*, FContact>(pObj, (*iter).second));
	}

	return listGameObject;
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_Collider_GetGameObject(const _uint iWorldID, const _vec3 vPos, CColliderComponent* pSrc, _ulong iMask)
{
	FCollisionPrimitive pShape = *static_cast<FCollisionPrimitive*>(pSrc->Get_Shape());
	pShape.Set_Position(pShape.Get_Position() + FVector3(vPos.x, vPos.y, vPos.z));
	pShape.Set_CollisionMask(iMask);

	return IntersectTests_GetGameObject(iWorldID, &pShape);
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_Sphere_GetGameObject(const _uint iWorldID, _vec3 vPos, _float fRadius, _ulong iMask)
{
	FCollisionSphere pShape;
	pShape.Set_Position(FVector3(vPos.x, vPos.y, vPos.z));
	pShape.fRadius = fRadius;
	pShape.Set_CollisionMask(iMask);

	return IntersectTests_GetGameObject(iWorldID, static_cast<FCollisionPrimitive*>(&pShape));
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_Box_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vHalfSize, _ulong iMask)
{
	FCollisionBox pShape;
	pShape.Set_Position(FVector3(vPos.x, vPos.y, vPos.z));
	pShape.vHalfSize = FVector3(vHalfSize.x, vHalfSize.y, vHalfSize.z);
	pShape.Set_CollisionMask(iMask);

	return IntersectTests_GetGameObject(iWorldID, static_cast<FCollisionPrimitive*>(&pShape));
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_Capsule_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vNormal, _float fRadius, _ulong iMask)
{
	FCollisionCapsule pShape;
	pShape.Set_Position(FVector3(vPos.x, vPos.y, vPos.z));
	pShape.vDirHalfSize = FVector3(vNormal.x, vNormal.y, vNormal.z);
	pShape.fRadius = fRadius;
	pShape.Set_CollisionMask(iMask);

	return IntersectTests_GetGameObject(iWorldID, static_cast<FCollisionPrimitive*>(&pShape));
}

list<pair<CGameObject*, FContact>> CPhysicsMgr::IntersectTests_Ray_GetGameObject(const _uint iWorldID, const _vec3 vPos, const _vec3 vNormal, _ulong iMask)
{
	FCollisionRay pShape;
	pShape.vOrigin = (FVector3(vPos.x, vPos.y, vPos.z));
	pShape.vDir = FVector3(vNormal.x, vNormal.y, vNormal.z).Unit();
	pShape.Set_CollisionMask(iMask);

	return IntersectTests_GetGameObject(iWorldID, static_cast<FCollisionPrimitive*>(&pShape));
}
