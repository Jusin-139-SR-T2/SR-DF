#include "PhysicsMgr.h"

#include "PHY_Shape.h"

IMPLEMENT_SINGLETON(CPhysicsMgr)

CPhysicsMgr::CPhysicsMgr()
{
}

CPhysicsMgr::~CPhysicsMgr()
{
}

void CPhysicsMgr::Free()
{
	for (auto item : m_ShapeList)
		Safe_Release(item);
}

HRESULT CPhysicsMgr::Ready_Physics()
{


	return S_OK;
}

_int CPhysicsMgr::Update_Physics()
{
	for (auto iter = m_ShapeList.begin(); iter != m_ShapeList.end(); ++iter)
	{
		//(*iter)
	}

	return 0;
}

void CPhysicsMgr::Add_Shape(FPHY_Shape* pShape)
{
	m_ShapeList.push_back(pShape->Get_ByRef<FPHY_Shape>());
}
