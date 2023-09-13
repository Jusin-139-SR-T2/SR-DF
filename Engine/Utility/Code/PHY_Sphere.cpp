#include "PHY_Sphere.h"

FPHY_Sphere::FPHY_Sphere()
{
}

FPHY_Sphere::FPHY_Sphere(const FPHY_Shape& rhs)
	: Base(rhs)
{
}

FPHY_Sphere::~FPHY_Sphere()
{
}

FPHY_Sphere* FPHY_Sphere::Create()
{
	ThisClass* pInstance = new ThisClass();

	if (FAILED(pInstance->Ready_Shape()))
	{
		Safe_Release(pInstance);

		MSG_BOX("FPHY_Shape Create Failed");
		return nullptr;
	}

	return pInstance;
}

FPHY_Sphere* FPHY_Sphere::Clone()
{
	return new ThisClass(*this);
}

void FPHY_Sphere::Free()
{
	SUPER::Free();
}

HRESULT FPHY_Sphere::Ready_Shape()
{
	SUPER::Ready_Shape();
	eType = EPHY_SHAPE::SPHERE;

	return S_OK;
}
