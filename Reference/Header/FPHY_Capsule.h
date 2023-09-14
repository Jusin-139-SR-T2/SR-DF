#pragma once

#include "FPHY_Shape.h"

BEGIN(Engine)

class ENGINE_DLL CFPHY_Capsule : public FPHY_Shape
{
	DERIVED_CLASS(FPHY_Shape, CFPHY_Capsule)
private:
	explicit CFPHY_Capsule();
	explicit CFPHY_Capsule(const FPHY_Shape& rhs);
	virtual ~CFPHY_Capsule();

public:
	static CFPHY_Capsule* Create();
	virtual CFPHY_Capsule* Clone();
protected:
	virtual void Free() override;

protected:
	virtual HRESULT Ready_Shape();
};

END