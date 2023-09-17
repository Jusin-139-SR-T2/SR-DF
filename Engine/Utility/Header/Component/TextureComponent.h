#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTextureComponent : public CComponent
{
	DERIVED_CLASS(CComponent, CTextureComponent)
private:
	explicit CTextureComponent();
	explicit CTextureComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTextureComponent(const CTextureComponent& rhs);
	virtual ~CTextureComponent();

public:
	static CTextureComponent* Create(LPDIRECT3DDEVICE9 pGraphicDev,
										TEXTUREID eType,
										_tchar* pPath,
										const _uint& iCnt = 1);
	virtual CComponent* Clone();

private:
	virtual void			Free();

public:
	HRESULT		Ready_Texture(TEXTUREID eType, const _tchar* pPath, const _uint& iCnt);
	void		Render_Texture(const _uint& iIndex = 0);

private:
	vector<IDirect3DBaseTexture9*>		m_vecTexture;

};

END