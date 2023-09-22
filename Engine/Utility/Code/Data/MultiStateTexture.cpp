#include "MultiStateTexture.h"



CMultiStateTexture::CMultiStateTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
	m_eTexComType = ETEXTURE_COMTYPE::MULTI_STATE;
}

CMultiStateTexture::~CMultiStateTexture()
{
}

CMultiStateTexture* CMultiStateTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Texture(pGraphicDev)))
	{
		Safe_Release(pInstance);

		MSG_BOX("Texture Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMultiStateTexture::Free()
{
	for (auto item : m_mapMultiState)
	{
		for(auto indexItem : item.second)
			Safe_Release(indexItem);
	}
}

HRESULT CMultiStateTexture::Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;

	return S_OK;
}

HRESULT CMultiStateTexture::Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey, const _range<_uint>& iCntRange)
{
	// ������Ʈ Ű�� �̹� �ؽ�ó ���� ������ ó������ �ʴ´�.
	if (m_mapMultiState.find(pStateKey) != m_mapMultiState.end())
		return E_FAIL;

	// ���� ���翩�� Ȯ��, Ư�� ����� �ش��ϴ� ī��Ʈ ���� �����ϴ��� Ȯ��, �߸��� ��ΰ� �ִٸ� ���� ��ȯ
	//{
	//	TCHAR	szFileName[256] = L"";
	//	wsprintf(szFileName, pFilePath, iCntRange.second);

	//	wifstream file(szFileName);
	//	FALSE_CHECK_RETURN(file.good(), E_FAIL);
	//	
	//}
	

	LPDIRECT3DBASETEXTURE9 pTexture = nullptr;

	for (_uint i = 0; i <= iCntRange.second; ++i)
	{
		TCHAR	szFileName[256] = L"";
		wsprintf(szFileName, pFilePath, i);

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

		// �ʱ⼼��, ���� �˻��� ����
		if (i == 0U)
		{
			m_mapMultiState[pStateKey].reserve(iCntRange.second + 1U);
			m_mapMultiState.emplace(pStateKey, vector<LPDIRECT3DBASETEXTURE9>());
		}

		m_mapMultiState[pStateKey].push_back(pTexture);
	}

	return S_OK;
}

void CMultiStateTexture::Transfer_Texture(vector<LPDIRECT3DBASETEXTURE9>* pVecTexture, const _tchar* pStateKey)
{
	if (m_mapMultiState.empty())
		return;

	auto iter = m_mapMultiState.find(pStateKey);

	if (iter == m_mapMultiState.end())
		return;

	if (m_mapMultiState[pStateKey].size() > pVecTexture->size())
	{
		pVecTexture->reserve(m_mapMultiState[pStateKey].size());
	}
		(*pVecTexture) = m_mapMultiState[pStateKey];
}
