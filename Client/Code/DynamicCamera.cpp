#include "stdafx.h"
#include "DynamicCamera.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include <Transform.h>

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{

}

CDynamicCamera::~CDynamicCamera()
{
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pInstance);
		MSG_BOX("DynamicCamera Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CDynamicCamera::Free()
{
	SUPER::Free();
}


HRESULT CDynamicCamera::Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;
	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	FAILED_CHECK_RETURN(CCamera::Ready_GameObject(), E_FAIL);

	ShowCursor(false);

	return S_OK;
}

_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{
	Check_KeyInput(fTimeDelta);
	Camera_State(fTimeDelta);

	if (false == m_bFix)
	{
		Mouse_Move();
		Mouse_Fix();
	}

	// ī�޶� ���� �÷��̾� ����
	//if (m_pTarget)
	//{
	//	CTransform* pTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	//	_vec3 vPos;
	//	pTransform->Get_Info(INFO_POS, &vPos);
	//	m_vAt = vPos;
	//}

	_int	iExit = CCamera::Update_GameObject(fTimeDelta);

	return iExit;
}

void CDynamicCamera::Check_KeyInput(const _float& fTimeDelta)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, nullptr, &m_matView);

	if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::Get_DIKeyState(DIK_A) & 0x80)
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::Get_DIKeyState(DIK_D) & 0x80)
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (Engine::Get_DIKeyState(DIK_Q) & 0x80)
	{
		_vec3	vUp;
		memcpy(&vUp, &matCamWorld.m[1][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeed * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::Get_DIKeyState(DIK_E) & 0x80)
	{
		_vec3	vUp;
		memcpy(&vUp, &matCamWorld.m[1][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vUp, &vUp) * m_fSpeed * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	// 1��Ī
	if (Engine::Get_DIKeyState(DIK_1) & 0x80)
	{
		if (m_bThree)
			m_bThree = false;

		if (!m_bOne)
		{
			m_bOne = true;
		}
	}

	// 3��Ī
	if (Engine::Get_DIKeyState(DIK_2) & 0x80)
	{
		if (m_bOne)
			m_bOne = false;

		if (!m_bThree)
		{
			m_bThree = true;
		}
	}

	// ���� ����
	if (Engine::Get_DIKeyState(DIK_3) & 0x80)
	{
		if (m_bOne)
			m_bOne = false;

		if (m_bThree)
		{
			m_bThree = false;
		}
	}

	// �� Ű�� ���� ���콺 Fix�� �״� ���� ��
	if (Engine::Get_DIKeyState(DIK_TAB) & 0x80)
	{
		if (m_bCheck)
			return;

		m_bCheck = true;

		if (m_bFix)
			m_bFix = false;
		else
			m_bFix = true;
	}
	else
		m_bCheck = false;

	if (false == m_bFix)
		return;
}

void CDynamicCamera::Mouse_Move()
{
	// ���콺�� �̵��ϴ� ����
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

	_long	dwMouseMove = 0;

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));
		_vec3		vLook = m_vAt - m_vEye;
		_matrix		matRot;

		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));
		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
		m_vAt = m_vEye + vLook;
	}

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
	{
		_vec3	vUp = { 0.f, 1.f, 0.f };
		_vec3		vLook = m_vAt - m_vEye;
		_matrix		matRot;

		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));
		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
		m_vAt = m_vEye + vLook;
	}
}

void CDynamicCamera::Mouse_Fix()
{
	POINT	pt{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

// ī�޶� ���� (1��Ī, 3��Ī, ���� ����)
void CDynamicCamera::Camera_State(const _float& fTimeDelta)
{
	// �÷��̾� Trans ������Ʈ �޾ƿ���
	pPlayerTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	//NULL_CHECK_RETURN(pPlayerTransCom, -1); // NULL

	_vec3	vPlayerPos;				// �÷��̾� ��ġ
	_vec3	vPlayerLook;			// �÷��̾ �ٶ󺸴� ��
	_vec3	vPlayerCameraPos;		// 3��Ī ī�޶� ��ġ ����(Eye)
	_vec3	vDist = { 0, 5, -8 };	// ī�޶� �÷��̾�� ���� �־����� �Ÿ�

	_matrix	matPlayerRotX;		// �÷��̾� ȸ�� ���� x
	_matrix	matPlayerRotY;		// �÷��̾� ȸ�� ���� y
	_matrix matPlayerRot;		// ���������� �÷��̾ ȸ���� ����

	// �÷��̾��� ��ġ, �ٶ󺸴� ������ ���´�.
	pPlayerTransCom->Get_Info(INFO_POS, &vPlayerPos);
	pPlayerTransCom->Get_Info(INFO_LOOK, &vPlayerLook);

	// �÷��̾��� ȸ���� ������ ���´�.
	float fX = pPlayerTransCom->m_vAngle.x;
	float fY = pPlayerTransCom->m_vAngle.y;

	// �÷��̾� ȸ�������� ��ķ� �����ش�.
	D3DXMatrixRotationX(&matPlayerRotX, fX);
	D3DXMatrixRotationY(&matPlayerRotY, fY);

	// �÷��̾� ȸ�� ��� X, Y �� ���� ��Ŀ� ���� �ϼ���Ų��. 
	D3DXMatrixMultiply(&matPlayerRot, &matPlayerRotX, &matPlayerRotY);


	// �÷��̾ �ٶ󺸴� ������ ����ȭ ���ش�.
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);		

	//							ī�޶� ��ġ, ������ �Ÿ�, �÷��̾� ����
	D3DXVec3TransformCoord(&vPlayerCameraPos, &vDist, &matPlayerRot);

	// �÷��̾�� ���� ī�޶� ������ �Ÿ��� ���Ѵ�.
	vPlayerCameraPos = vPlayerPos + vPlayerCameraPos;

	// 3��Ī
	if (m_bThree)
	{
		// ī�޶� ��ġ ����
		m_vEye = { vPlayerCameraPos.x, vPlayerCameraPos.y, vPlayerCameraPos.z };

		// ī�޶� �ٶ󺸴� ��� (�÷��̾�)
		m_vAt = vPlayerPos;
	}

	// 1��Ī
	if (m_bOne)
	{
		// ī�޶� ��ġ ����
		m_vEye = { vPlayerPos.x,
				   vPlayerPos.y,
				   vPlayerPos.z };

		// ī�޶� �ٶ󺸴� ��� (�÷��̾ �ٶ󺸴� ����)
		m_vAt = vPlayerPos + vPlayerLook;

		Mouse_Move(); // ���콺 �̵�
		Mouse_Fix();  // ���콺 ����
	}
}

