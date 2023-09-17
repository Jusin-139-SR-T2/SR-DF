#pragma once

#include "VIBufferComp.h"

BEGIN(Engine)

class ENGINE_DLL CCubeBufferComp : public CVIBufferComp
{
	DERIVED_CLASS(CVIBufferComp, CCubeBufferComp)
private:
	explicit CCubeBufferComp();
	explicit CCubeBufferComp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeBufferComp(const CCubeBufferComp& rhs);
	virtual ~CCubeBufferComp();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	static CCubeBufferComp* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free();
};

END