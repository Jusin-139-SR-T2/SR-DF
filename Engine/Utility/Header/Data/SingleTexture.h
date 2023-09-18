#pragma once

#include "Texture.h"

BEGIN(Engine)

class CSingleTexture : public CTexture
{
	DERIVED_CLASS(CTexture, CSingleTexture)

private:
	explicit CSingleTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSingleTexture();

public:
	static CSingleTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual void		Free();

public:
	virtual HRESULT Ready_Texture();
	virtual HRESULT Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey = L"", const _uint& iCount = 0) override;

protected:
	LPDIRECT3DBASETEXTURE9		m_pTexture = nullptr;			// 텍스처 베이스
	TEXTUREID					m_eType = TEX_NORMAL;			// 텍스처 종류



	
};

END