#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

/// <summary>
/// 텍스처 데이터 저장용 클래스
/// </summary>
class CTexture abstract : public CBase
{
	DERIVED_CLASS(CBase, CTexture)

private:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTexture();

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev,
							const _uint& iIndex);

private:
	virtual void		Free();

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	_uint						m_iIndex;

public:
	//HRESULT			Ready_Light(const D3DLIGHT9* pLightInfo, const _uint& iIndex);
};

END