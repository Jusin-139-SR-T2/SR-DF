#pragma once

#include "Base.h"
#include "Engine_Define.h"
#include "TextureMgr.h"

BEGIN(Engine)

/// <summary>
/// �ؽ�ó ������ ����� Ŭ����
/// �߻�Ŭ����
/// </summary>
class CTexture abstract : public CBase
{
	DERIVED_CLASS(CBase, CTexture)

protected:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTexture();

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual void		Free() PURE;

public:
	virtual HRESULT		Ready_Texture() PURE;
	virtual HRESULT		Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey = L"", const _uint& iCount = 0) PURE;

protected:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;						// ��ġ
	ETEXTURE_COMTYPE			m_eTexCompType = ETEXTURE_COMTYPE::SINGLE;		// ��Ƽ�ؽ������� Ȯ���ϴ� �뵵

};

END