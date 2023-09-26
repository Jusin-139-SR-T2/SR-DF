#pragma once

#include "VIBufferComp.h"

BEGIN(Engine)

class ENGINE_DLL CRcBufferComp : public CVIBufferComp
{
	DERIVED_CLASS(CVIBufferComp, CRcBufferComp)
private:
	explicit CRcBufferComp();
	explicit CRcBufferComp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRcBufferComp(const CRcBufferComp& rhs);
	virtual ~CRcBufferComp();

public:
	static CRcBufferComp*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

	VTXTEX* Get_Vertex() { return pVertex; }

	void Set_Vertex(_float _fX, _float _fY, _float _fZ) 
	{
		m_pVB->Lock(0, 0, (void**)&pVertex, 0);

		pVertex[0].vPosition += { _fX, _fY, _fZ };
		pVertex[1].vPosition += { _fX, _fY, _fZ };
		pVertex[2].vPosition += { _fX, _fY, _fZ };
		pVertex[3].vPosition += { _fX, _fY, _fZ };

		m_pVB->Unlock();
	}

private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

private:
	VTXTEX* pVertex = nullptr;

	_float fX;
	_float fY;
	_float fZ;

};

END