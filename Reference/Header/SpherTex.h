#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CSpherTex : public CVIBuffer
{
	DERIVED_CLASS(CVIBuffer, CSpherTex)

protected:
	explicit CSpherTex();
	explicit CSpherTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSpherTex(const CSpherTex& rhs);
	virtual ~CSpherTex();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	static CSpherTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free();
};

END