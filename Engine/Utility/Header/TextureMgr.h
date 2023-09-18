#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CTexture;

// 컴객체 텍스처의 타입
enum class ETEXTURE_COMTYPE
{
	SINGLE,
	SINGLE_STATE,
	MULTI,
	MULTI_STATE
};

/// <summary>
/// 텍스처를 관리하는 매니저
/// </summary>
class CTextureMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CTextureMgr)

private:
	explicit CTextureMgr();
	virtual ~CTextureMgr();

public:
	virtual void	Free();

public:
	HRESULT			Ready_Texture();
	HRESULT			Load_Texture(TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey, const _uint& iCount = 0U);
	HRESULT			Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey = L"", const _uint& iCount = 0U);

private:
	LPDIRECT3DDEVICE9				m_pGraphicDev = nullptr;
	_unmap<wstring, CTexture*>		m_mapTexture;


};

END