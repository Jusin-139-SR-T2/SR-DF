#include "SphereColComp.h"

#include "PHY_Sphere.h"

CSphereColComp::CSphereColComp()
{
}

CSphereColComp::CSphereColComp(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CSphereColComp::CSphereColComp(const CSphereColComp& rhs)
	: Base(rhs)
{
}

CSphereColComp::~CSphereColComp()
{
}

CSphereColComp* CSphereColComp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Collider()))
	{
		Safe_Release(pInstance);

		MSG_BOX("FPHY_Shape Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CSphereColComp::Clone()
{
	return new ThisClass(*this);
}

void CSphereColComp::Free()
{
	SUPER::Free();

	//Safe_Release(m_pShape);
}

HRESULT CSphereColComp::Ready_Collider()
{
	//m_pShape = FPHY_Sphere::Create()->Get_ByRef<FPHY_Sphere>();

	return S_OK;
}
