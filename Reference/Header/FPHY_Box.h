#pragma once

#include "FPHY_Shape.h"

BEGIN(Engine)

class ENGINE_DLL CFPHY_Box : public FPHY_Shape
{
	DERIVED_CLASS(FPHY_Shape, CFPHY_Box)
private:
	explicit CFPHY_Box();
	explicit CFPHY_Box(const FPHY_Shape& rhs);
	virtual ~CFPHY_Box();

public:
	static CFPHY_Box* Create();
	virtual CFPHY_Box* Clone();
protected:
	virtual void Free() override;

protected:
	virtual HRESULT Ready_Shape();
};

END