#pragma once

#include "Texture.h"

BEGIN(Engine)

class CSingleStateTexture : public CTexture
{
	DERIVED_CLASS(CTexture, CSingleStateTexture)

private:
	explicit CSingleStateTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSingleStateTexture();

public:
	static CSingleStateTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual void		Free();

public:
	virtual HRESULT Ready_Texture();
	virtual HRESULT Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey = L"", const _uint& iCount = 0) override;


protected:
	using multi_tex_map = _unmap<wstring, LPDIRECT3DBASETEXTURE9>;
	multi_tex_map			m_mapTextureState;				// ������Ʈ Ű�� ���� �ؽ�ó ����
	TEXTUREID				m_eType = TEX_NORMAL;			// �ؽ�ó ����
	
};

END