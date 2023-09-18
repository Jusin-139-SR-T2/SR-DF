#pragma once

#include "Texture.h"

BEGIN(Engine)

class CMultiTexture : public CTexture
{
	DERIVED_CLASS(CTexture, CMultiTexture)

private:
	explicit CMultiTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMultiTexture();

public:
	static CMultiTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual void		Free();

public:
	virtual HRESULT Ready_Texture();
	virtual HRESULT Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey = L"", const _uint& iCount = 0) override;

protected:
	vector<LPDIRECT3DBASETEXTURE9>		m_vecMulti;			// 스테이트 키에 대한 텍스처 정보
	TEXTUREID				m_eType = TEX_NORMAL;			// 텍스처 종류
};

END