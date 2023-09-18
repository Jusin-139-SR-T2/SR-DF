#include "SingleStateTexture.h"

CSingleStateTexture::CSingleStateTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CSingleStateTexture::~CSingleStateTexture()
{
}

CSingleStateTexture* CSingleStateTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Texture()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Texture Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CSingleStateTexture::Free()
{
	for (auto item : m_mapTextureState)
	{
		Safe_Release(item.second);
	}
}

HRESULT CSingleStateTexture::Ready_Texture()
{


	return S_OK;
}

HRESULT CSingleStateTexture::Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey, const _uint& iCount)
{
	// ������Ʈ Ű�� �̹� �ؽ�ó ���� ������ ó������ �ʴ´�.
	if (m_mapTextureState.find(pStateKey)->second == nullptr)
		return E_FAIL;

	LPDIRECT3DBASETEXTURE9 pTexture = nullptr;

	TCHAR	szFileName[256] = L"";
	wsprintf(szFileName, pFilePath, L"");

	switch (eType)
	{
	case TEX_NORMAL:
	{
		FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DTEXTURE9*)&pTexture), E_FAIL);
		break;
	}
	case TEX_CUBE:
	{
		FAILED_CHECK_RETURN(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DCUBETEXTURE9*)&pTexture), E_FAIL);
		break;
	}
	case TEX_VOLUME:
	{
		FAILED_CHECK_RETURN(D3DXCreateVolumeTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DVOLUMETEXTURE9*)&pTexture), E_FAIL);
		break;
	}
	}

	m_mapTextureState.emplace(pTexture);

	return S_OK;
}
