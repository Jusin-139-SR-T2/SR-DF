#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CMeshCol : public CVIBuffer
{
	DERIVED_CLASS(CVIBuffer, CMeshCol)

protected:
	explicit CMeshCol();
	explicit CMeshCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMeshCol(const CMeshCol& rhs);
	virtual ~CMeshCol();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	static CMeshCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free();
};

END