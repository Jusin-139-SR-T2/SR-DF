#include "MyTexture.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include <ImguiMgr.h>

CMyTexture::CMyTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CMyTexture::CMyTexture(const CMyTexture& rhs)
	: Base(rhs)
{
}

CMyTexture::~CMyTexture()
{
}

CMyTexture* CMyTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("MyTexture Create Failed");
		return nullptr;
	}

	return pInstance;
}


HRESULT CMyTexture::Add_Component()
{
	// Ʈ������ ������Ʈ
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	// ����
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	// �ؽ��� ������Ʈ
	NULL_CHECK_RETURN(m_pBackTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_TitleTexture", L"Proto_ClientTextureComp"), E_FAIL);
	// �ؽ��� ������Ʈ
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Comp_LogoTexture", L"Proto_LogoTextureComp"), E_FAIL);

	return S_OK;
}

HRESULT CMyTexture::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pAnimationTool = dynamic_cast<CImguiAnimationTool*>
		(CImguiMgr::GetInstance()->Get_MapImguiWin()[L"AnimationTool"]);

	if (m_pAnimationTool->Get_Animation()->empty())
	{
		m_vecAnimationInfo = m_pAnimationTool->Get_Animation();
	}

	m_fSizeX = 1280;
	m_fSizeY = 720;

	m_fX = m_fSizeX * 0.5f; // ������ġ 
	m_fY = m_fSizeY * 0.5f;

	m_pTransformComp->Set_Pos({ 0.f, 0.f, 0.f });	// �̹��� ��ġ
	m_pTransformComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });	// �̹��� ũ��

	return S_OK;
}

_int CMyTexture::Update_GameObject(const _float& fTimeDelta)
{

	SUPER::Update_GameObject(fTimeDelta);

	//// ������� �˻�
	//if (!m_vecAnimationInfo->empty())
	//{
	//	if (m_pAnimationTool->Get_FramePlaying())
	//	{
	//		m_fMaxFrame = m_vecAnimationInfo->size(); // ����� �ִ� ���������� ����
	//	}
	//}

	//// ������ ��� ����
	//if (m_pAnimationTool->Get_FramePlaying())
	//{
	//	// ���� �������� �ð�(������)���� ������Ű��
	//	m_fAniamtionFrame += m_fAnimationSpeed * fTimeDelta;

	//	// ���� �������� �ִ� �����ӿ� ������ ���
	//	if (m_fAniamtionFrame >= (*m_vecAnimationInfo)[m_vecAnimationInfo->size()].time)
	//	{
	//		// ���� ������ �ʱ�ȭ
	//		m_fAniamtionFrame = 0.f;

	//		// ������ Off
	//		m_pAnimationTool->Set_FramePlaying(false);
	//	}
	//}

	//if (!m_vecAnimationInfo->empty())
	//{
	//	if (m_fAniamtionFrame >= 0.f &&
	//		m_fAniamtionFrame <= m_vecAnimationInfo->back().time)
	//	{
	//		//m_eAnimationInfo = m_vecAnimationInfo[(int)m_iFrameCount].front();
	//		_uint iFrameIndex = 0U;
	//		for (_uint i = m_vecAnimationInfo->size() - 1; i >= 0; i--)
	//		{
	//			if ((*m_vecAnimationInfo)[i].time <= m_fAniamtionFrame)
	//			{
	//				iFrameIndex = i;
	//				break;
	//			}
	//		}

	//		// Constant
	//		//m_fSizeX = (*m_vecAnimationInfo)[iFrameIndex].vScale.x;
	//		//m_fSizeY = (*m_vecAnimationInfo)[iFrameIndex].vScale.y;

	//		//m_fX = m_fSizeX * 0.5f; // ������ġ 
	//		//m_fY = m_fSizeY * 0.5f;

	//		//m_pTransformComp->Set_Pos({ (*m_vecAnimationInfo)[iFrameIndex].vPos.x,
	//		//							(*m_vecAnimationInfo)[iFrameIndex].vPos.y,
	//		//							0.f });	// �̹��� ��ġ

	//		//m_pTransformComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });	// �̹��� ũ��


	//		// Linear
	//		if (iFrameIndex + 1U < m_vecAnimationInfo->size())
	//		{


	//			// Ű �����Ӱ� �ð� ��ȭ��
	//			fFrameTimeDelta = (*m_vecAnimationInfo)[iFrameIndex + 1U].time - (*m_vecAnimationInfo)[iFrameIndex].time;
	//			// ���� Ű �����ӽð����� ���� �ð� ��ȭ��
	//			fCurFrameTimeDelta = (m_fAniamtionFrame - (*m_vecAnimationInfo)[iFrameIndex].time);

	//			fSizeX_Delta = (*m_vecAnimationInfo)[iFrameIndex + 1U].vScale.x - (*m_vecAnimationInfo)[iFrameIndex].vScale.x;
	//			fSizeX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
	//			fSizeY_Delta = (*m_vecAnimationInfo)[iFrameIndex + 1U].vScale.y - (*m_vecAnimationInfo)[iFrameIndex].vScale.y;
	//			fSizeY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

	//			fPosX_Delta = (*m_vecAnimationInfo)[iFrameIndex + 1U].vPos.x - (*m_vecAnimationInfo)[iFrameIndex].vPos.x;
	//			fPosX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
	//			fPosY_Delta = (*m_vecAnimationInfo)[iFrameIndex + 1U].vPos.y - (*m_vecAnimationInfo)[iFrameIndex].vPos.y;
	//			fPosY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

	//			m_pTransformComp->Set_Pos({ (*m_vecAnimationInfo)[iFrameIndex].vPos.x + fPosX_Delta,
	//										(*m_vecAnimationInfo)[iFrameIndex].vPos.y + fPosX_Delta,
	//										0.f });	// �̹��� ��ġ

	//			m_pTransformComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });	// �̹��� ũ��
	//		}
	//		else
	//		{
	//			m_fSizeX = (*m_vecAnimationInfo)[iFrameIndex].vScale.x;
	//			m_fSizeY = (*m_vecAnimationInfo)[iFrameIndex].vScale.y;

	//			m_fX = m_fSizeX * 0.5f; // ������ġ 
	//			m_fY = m_fSizeY * 0.5f;

	//			m_pTransformComp->Set_Scale({ (*m_vecAnimationInfo)[iFrameIndex].vScale.x + fSizeX_Delta, 	// �̹��� ũ��
	//										  (*m_vecAnimationInfo)[iFrameIndex].vScale.y + fSizeY_Delta,
	//										  1.f });

	//			//m_pTransformComp->Set_Rotation({});

	//			m_pTransformComp->Set_Pos({ (*m_vecAnimationInfo)[iFrameIndex].vPos.x,
	//										(*m_vecAnimationInfo)[iFrameIndex].vPos.y,
	//										0.f });	// �̹��� ��ġ
	//		}
	//	}

	//}

	Engine::Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CMyTexture::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

void CMyTexture::Render_GameObject()
{
	// �ؽ�ó�� Ʈ�������� ���� ���, DYNAMIC���� ������ �������� ���� �ʿ䰡 ����
	m_pBackTextureComp->Readjust_Transform();
	// �ؽ�ó ��� * �θ�(���ӿ�����Ʈ)�� ���
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pBackTextureComp->Set_TransformToWorld(*m_pTransformComp->Get_Transform());
	// ���� �ΰ��� ���� �ؽ�ó ��İ� �θ� ����� ������ �ΰ� ����� �� ����.


	// �������� Render_Texture �Լ� �ȿ��� �ڵ����� �ؽ�ó�� ����� ����̽��� ����.(SetTransform(D3DTS_WORLD, �ؽ�ó ���))
	m_pBackTextureComp->Render_Texture(0, true);
	m_pBufferComp->Render_Buffer();

	// �̰� �θ� ����� �ؽ�ó ��Ŀ� �״�� ���� ���, �ؽ�ó ������ ����� �ʿ���� �� ���
	//m_pTextureComp->Set_Transform(m_pTransformComp->Get_Transform());

	// m_pBackTextureComp�� ������ �Ͱ� m_pTextureComp�� ������ ��. �ΰ��� ��׶��忡�� ������ ��ġ, ũ��� �����ǹǷ� ��ȿ��.

	// Set_Transform : �ܼ� ��� �����
	// Readjust_Transform : �ؽ�ó�� ���� ��ǥ, ȸ��, ũ�⸦ �ؽ�ó�� ���� ���(Ʈ������)�� �����Ѵ�.
	// Set_TransformToWorld : �ؽ�ó ���� ��� * �θ� ���, ��İ���
	//m_pTextureComp->Render_Texture(0, true);
	m_pBufferComp->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CMyTexture::Free()
{
	SUPER::Free();


}
