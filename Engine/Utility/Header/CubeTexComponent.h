#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCubeTexComponent : public CVIBuffer
{
	DERIVED_CLASS(CVIBuffer, CCubeTexComponent)
private:
	explicit CCubeTexComponent();
	explicit CCubeTexComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeTexComponent(const CCubeTexComponent& rhs);
	virtual ~CCubeTexComponent();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	static CCubeTexComponent* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free();
};

END