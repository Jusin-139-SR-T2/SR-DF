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
