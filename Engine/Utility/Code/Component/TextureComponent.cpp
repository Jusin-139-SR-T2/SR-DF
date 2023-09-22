#include "TextureComponent.h"

#include "TextureMgr.h"

CTextureComponent::CTextureComponent()
{
}

CTextureComponent::CTextureComponent(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CTextureComponent::CTextureComponent(const CTextureComponent& rhs)
    : Base(rhs)
{
    // ���ʹ� ���� ���� �� ����
    size_t	iSize = rhs.m_vecTexture.size();
    m_vecTexture.reserve(iSize);

    m_vecTexture = rhs.m_vecTexture;

    // �ؽ�ó���� ����ī��Ʈ ����
    for (size_t i = 0; i < iSize; ++i)
        m_vecTexture[i]->AddRef();
}

CTextureComponent::~CTextureComponent()
{
}

CTextureComponent* CTextureComponent::Create(LPDIRECT3DDEVICE9 pGraphicDev, TEXTUREID eID, const _tchar* pTextureKey, const _tchar* pStateKey)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Texture(eID, pTextureKey, pStateKey)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Texture Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CTextureComponent::Clone()
{
	return new ThisClass(*this);
}

void CTextureComponent::Free()
{
	SUPER::Free();

	//for (size_t i = 0; i < m_vecTexture.size(); ++i)
		//Safe_Release(m_vecTexture[i]);
}

HRESULT CTextureComponent::Ready_Texture(TEXTUREID eID, const _tchar* pTextureKey, const _tchar* pStateKey)
{
	return Receive_Texture(eID, pTextureKey, pStateKey);
}

void CTextureComponent::Render_Texture(const _uint& iIndex)
{
	// �ε��� �����÷ο� ����
	if (m_vecTexture.size() <= iIndex)
		return;

	m_pGraphicDev->SetTexture(0, m_vecTexture[iIndex]);
}

HRESULT CTextureComponent::Receive_Texture(TEXTUREID eID, const _tchar* pTextureKey, const _tchar* pStateKey)
{
	return CTextureMgr::GetInstance()->Transfer_Texture(&m_vecTexture, eID, pTextureKey, pStateKey);
}
