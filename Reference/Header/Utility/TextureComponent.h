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
										TEXTUREID eID, 
										const _tchar* pTextureKey, const _tchar* pStateKey = L"");
	virtual CComponent* Clone();

private:
	virtual void			Free();

public:
	HRESULT		Ready_Texture(TEXTUREID eID, const _tchar* pTextureKey, const _tchar* pStateKey = L"");
	void		Render_Texture(const _uint& iIndex = 0);
	HRESULT		Receive_Texture(TEXTUREID eID, const _tchar* pTextureKey, const _tchar* pStateKey = L"");

public:
	GETSET_EX2(vector<LPDIRECT3DBASETEXTURE9>, m_vecTexture, VecTexture, GET_PTR, SET_PTR)

private:
	vector<LPDIRECT3DBASETEXTURE9>		m_vecTexture;
	
};

END