#include "stdafx.h"
#include "DynamicCamera.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "TransformComponent.h"

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
	// #���� ī�޶� �̻��ϴٸ� ���� ī�޶�� �����ϼ���

	Check_KeyInput(fTimeDelta);

	// ���� ī�޶�
	Camera_State(fTimeDelta);

	// ���ʹϿ� ���� ī�޶�
	//Quaternion_Ver(fTimeDelta);

	if (false == m_bFix)
	{
		Mouse_Move();
		Mouse_Fix();
	}

	//// ī�޶� ���� �÷��̾� ����
	//if (m_pTarget)
	//{
	//	CTransformComponent* pTransform = dynamic_cast<CTransformComponent*>(m_pTarget->Get_Component(ID_DYNAMIC, L"Com_Transform"));
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

	if (Engine::Get_DIKeyState(DIK_UP) & 0x80)
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (Engine::Get_DIKeyState(DIK_DOWN) & 0x80)
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::Get_DIKeyState(DIK_LEFT) & 0x80)
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::Get_DIKeyState(DIK_RIGHT) & 0x80)
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
	if (Engine::Get_DIKeyState(DIK_F1) & 0x80)
	{
		if (m_bThree)
			m_bThree = false;

		if (!m_bOne)
		{
			m_bOne = true;
		}
	}

	// 3��Ī
	if (Engine::Get_DIKeyState(DIK_F2) & 0x80)
	{
		if (m_bOne)
			m_bOne = false;

		if (!m_bThree)
		{
			m_bThree = true;
		}
	}

	// ���� ����
	if (Engine::Get_DIKeyState(DIK_F3) & 0x80)
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
	pPlayerTransCom = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
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

void CDynamicCamera::Quaternion_Ver(const _float& fTimeDelta)
{
	// �÷��̾� Trans ������Ʈ �޾ƿ���
	pPlayerTransCom = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
	//NULL_CHECK_RETURN(pPlayerTransCom, -1); // NULL

	_vec3	vPlayerPos;				// �÷��̾� ��ġ
	_vec3	vPlayerLook;			// �÷��̾ �ٶ󺸴� ��
	_vec3	vPlayerForward;			// �÷��̾��� �̵� ���� ����

	// �÷��̾��� ��ġ, �ٶ󺸴� ������ ���´�.
	pPlayerTransCom->Get_Info(INFO_POS, &vPlayerPos);
	pPlayerTransCom->Get_Info(INFO_LOOK, &vPlayerLook);
	//pPlayerTransCom->Rotation();

	_vec3 vOffSet = { 0.f, 5.f, -10.f };

	D3DXQUATERNION quatPitch, quatYaw, quatRoll;

	_long dwMouseMove = 0;
	_long dwMouseMoveX = Engine::Get_DIMouseMove(DIMS_X);
	_long dwMouseMoveY = Engine::Get_DIMouseMove(DIMS_Y);

	// ī�޶��� Right���� ���ϱ�
	// vForward = ��ǥ�� - ī�޶�
	_vec3 vForward = pPlayerTransCom->m_vInfo[INFO_POS] - m_vEye;
	//vForward�� ������ up����(0,1,0)�� �����Ͽ� Right���͸� ���Ѵ�.
	D3DXVec3Cross(&m_vRight, &m_vUp, &vForward);

	// 1��Ī
	if (m_bOne)
	{
		// �÷��̾��� ȸ�� ������ �������� ��ȯ
		_float playerRotationY = D3DXToRadian(m_fPlayerRotationY);

		// �÷��̾��� �̵� ���� ���
		vPlayerForward = _vec3(sin(playerRotationY), 0.0f, cos(playerRotationY));

		// �÷��̾��� �̵� ���͸� ����Ͽ� ī�޶� ��ġ ����
		m_vEye = vPlayerPos - vPlayerForward * m_vOffsetOne.z + m_vUp * m_vOffsetOne.y;

		// ī�޶� �ü� ��ġ ���
		m_vAt = vPlayerPos + vPlayerLook;

		// 1��Ī ����� ��� ī�޶� ��ġ�� �ü��� �÷��̾� ��ġ�� ��ġ��Ų��.
		// �ü��� �÷��̾��� ��ġ + �÷��̾ �ٶ󺸴� ����. �� �÷��̾ �ٶ󺸴� ����
		//m_vEye = pPlayerTransCom->m_vInfo[INFO_POS];
		//m_vAt = pPlayerTransCom->m_vInfo[INFO_POS] + pPlayerTransCom->m_vInfo[INFO_LOOK];
	}
	// 3��Ī
	if (m_bThree)
	{
		_vec3 vLook = { 0.f, 0.f, 0.f };

		D3DXQUATERNION quatPitch, quatYaw, quatRoll;

		_long dwMouseMove = 0;
		m_dwMouseMove[ROT_X] = Engine::Get_DIMouseMove(DIMS_X);
		m_dwMouseMove[ROT_Y] = Engine::Get_DIMouseMove(DIMS_Y);


		// ī�޶��� Right���� ���ϱ�
		vLook = pPlayerTransCom->m_vInfo[INFO_POS] - m_vEye;      // m_vLook = ��ǥ�� - ī�޶�
		//m_vLook�� ������ up����(0,1,0)�� �����Ͽ� Right���͸� ���Ѵ�.
		D3DXVec3Cross(&m_vRight, &m_vUp, &vLook);

		m_fMaxAngleY = 150.f;
		m_fMinAngleY = 90.f;

		m_vEye = pPlayerTransCom->m_vInfo[INFO_POS] + m_vOffset;
		m_vAt = pPlayerTransCom->m_vInfo[INFO_POS];

		// ���� ȸ�� (Y�� ȸ��)
		if (dwMouseMoveX != 0)
		{
			D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, D3DXToRadian(static_cast<_float>(dwMouseMoveX) * 1.f));
			m_quaternion *= quatYaw;
			D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
		}

		// ���� ȸ�� (X�� ȸ��)
		if (dwMouseMoveY != 0)
		{
			D3DXQuaternionRotationAxis(&quatPitch, &m_vRight, D3DXToRadian(static_cast<_float>(dwMouseMoveY) * 1.f));
			m_quaternion *= quatPitch;
			D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
		}

		// ���� ���콺 �Է� ���� �����Ͽ� ���� �����ӿ� ���
		m_dwPrevMouseMoveX = dwMouseMoveX;
		m_dwPrevMouseMoveY = dwMouseMoveY;

		// 3��Ī ����� ��� ī�޶� �÷��̾� �������� ȸ����Ų��. (����)
		// ��Ŭ�� �� ȸ�� ����
		if (Engine::Get_DIMouseState(DIM_RB) & 0x80)
		{
			_float fNewAngle = m_fAngleY + (m_dwMouseMove[ROT_Y] * m_fRotSpeed);

			if (fNewAngle < m_fMaxAngleY && fNewAngle > m_fMinAngleY)
			{
				if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
				{
					D3DXQuaternionRotationAxis(&quatPitch, &m_vRight, D3DXToRadian(m_dwMouseMove[ROT_Y] * m_fRotSpeed));
					m_quaternion *= quatPitch;
					D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
				}
			}

			// ���� ȸ�� (Y�� ȸ��)
			if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
			{
				D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, D3DXToRadian(m_dwMouseMove[ROT_X] * m_fRotSpeed));
				m_quaternion *= quatYaw;
				D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
			}
		}

		_matrix matRotation; // ȸ�� ����� ����� ���� ���
		D3DXMatrixRotationQuaternion(&matRotation, &m_quaternion); // ���ʹϾ��� ȸ�� ��ķ� ��ȯ

		_vec3 vPos = m_vEye - pPlayerTransCom->m_vInfo[INFO_POS];
		// ��� ���� ������ ���� �ּ�, ��İ� ���ϱ⸦ �� ��ġ ���� �ּ�, ��� �ּ�;
		D3DXVec3TransformCoord(&vPos, &vPos, &matRotation);      // ��ġ���� 

		m_vEye = vPos + pPlayerTransCom->m_vInfo[INFO_POS];


		//// ī�޶�� �÷��̾��� ���� �� ���ϱ�
		//// ī�޶� �÷��̾ �ٶ󺸴� ���� (��ǥ�� - ������)
		//_vec3  vCameraToPlayer = pPlayerTransCom->m_vInfo[INFO_POS] - m_vEye;
		//D3DXVec3Normalize(&vCameraToPlayer, &vCameraToPlayer);   // ����ȭ

		//// ī�޶� �÷��̾ �ٶ󺸴� ����� �÷��̾��� Up���͸� �����ؼ� �� ������ ���ΰ��� �˾Ƴ���(����)
		//m_fAngleY = acosf(D3DXVec3Dot(&vCameraToPlayer, &pPlayerTransCom->m_vInfo[INFO_UP]));
		//m_fAngleY = D3DXToDegree(m_fAngleY);


		///////////////////////////////////////////////////////////////////////////////////
		//// ���콺 �Է��� ����Ͽ� ī�޶� ȸ��
		//if (Engine::Get_DIMouseState(DIM_RB) & 0x80)
		//{
		//	_long dwMouseMoveX = Engine::Get_DIMouseMove(DIMS_X);
		//	_long dwMouseMoveY = Engine::Get_DIMouseMove(DIMS_Y);

		//	// ���콺 �Է¿� ���� ���ʹϾ��� ȸ���ϱ�
		//	D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, D3DXToRadian(static_cast<_float>(dwMouseMoveY)));
		//	D3DXQuaternionRotationAxis(&quatPitch, &m_vRight, D3DXToRadian(static_cast<_float>(dwMouseMoveX)));

		//	// �� ���ʹϾ��� ���Ͽ� ȸ������ ��ġ��
		//	D3DXQuaternionMultiply(&m_quaternion, &quatYaw, &quatPitch);
		//	D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
		//}

		//// ȸ���� ���ʹϾ��� �̿��Ͽ� ī�޶� �ٶ󺼰�(����)�� ��ġ �����ֱ�
		//_matrix matRotation;
		//D3DXMatrixRotationQuaternion(&matRotation, &m_quaternion);

		//// ī�޶��� ���� ���͸� ȸ����Ű��
		//_vec3 vDir = m_vEye - m_vAt;
		//D3DXVec3TransformCoord(&vDir, &vDir, &matRotation);

		//// ī�޶��� ��ġ�� �缳�����ֱ�
		//m_vEye = m_vAt + vDir;

		//// ���� ��ǥ���� ī�޶��� �� ���͸� �����ֱ�
		//_vec3 vCameraUp;
		//D3DXVec3Cross(&vCameraUp, &m_vRight, &vDir);
		//D3DXVec3Normalize(&vCameraUp, &vCameraUp);

		//_vec3 vPos = m_vEye - pPlayerTransCom->m_vInfo[INFO_POS];

		////D3DXVec3TransformCoord(&vPos, &vPos, &matRotation);

		//m_vEye = pPlayerTransCom->m_vInfo[INFO_POS] + vPos;

		//m_vAt = pPlayerTransCom->m_vInfo[INFO_POS];

		//// ���� ��ǥ�� ī�޶��� �� ���͸� �����Ͽ� ī�޶��� ������ �� ���͸� �������ֱ�
		//_vec3 vWorldUp = { 0.f, 1.f, 0.f };
		//_float fDot = D3DXVec3Dot(&vWorldUp, &vCameraUp);
		//_float fAngle = acosf(fDot);

		//

		//// ������ ����� ����� ���� ������ �� �ֵ��� �����ϱ�
		//if (fAngle >= 0.f)
		//{
		//	// ī�޶��� ��ġ ������Ʈ
		//	m_vEye = vDir + pPlayerTransCom->m_vInfo[INFO_POS];
		//}
	}
}
	// ����
//	// �÷��̾� ��ġ���� ��¦ �ڿ� ī�޶� ��ġ
//	m_vEye = pPlayerTransCom->m_vInfo[INFO_POS] + m_vOffset;
//	// �÷��̾ �ٶ󺸱�
//	m_vAt = pPlayerTransCom->m_vInfo[INFO_POS];
//
//	// ī�޶� �� ����
//	_vec3 vCameraUp;
//	D3DXVec3Cross(&vCameraUp, &m_vRight, &m_vAt);
//	D3DXVec3Normalize(&vCameraUp, &vCameraUp);
//
//	// ���� ��ǥ�� ī�޶��� �� ���ͷ� ���� (�׻� ����)
//	// ������ ����� ����� ���� ������ �� �ֵ��� ����
//	_vec3 vWorldUp = { 0.f, 1.f, 0.f };
//	_float fDot = D3DXVec3Dot(&vWorldUp, &vCameraUp);
//	_float fAngle = acosf(fDot);
//
//	// 3��Ī ����� ��� ī�޶� �÷��̾� �������� ȸ����Ų��. (����)
//	if (Engine::Get_DIMouseState(DIM_RB) & 0x80)
//	{
//		if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
//		{
//			D3DXQuaternionRotationAxis(&quatYaw, &m_vUp, dwMouseMove);
//			m_quaternion *= quatYaw;
//			D3DXQuaternionNormalize(&m_quaternion, &m_quaternion);
//		}
//	}
//
//	_matrix matRotation; // ȸ�� ����� ����� ���� ���
//	D3DXMatrixRotationQuaternion(&matRotation, &m_quaternion); // ���ʹϾ��� ȸ�� ��ķ� ��ȯ
//
//	_vec3 vDir = m_vEye - pPlayerTransCom->m_vInfo[INFO_POS];
//	D3DXVec3TransformCoord(&vDir, &vDir, &matRotation);
//
//	m_vEye = vDir + pPlayerTransCom->m_vInfo[INFO_POS];
//
//	//// ȸ�� ���� ����
//	//_vec3 vUpOffset = pPlayerTransCom->m_vInfo[INFO_POS];
//
//	//_vec3 vCameraDir = pPlayerTransCom->m_vInfo[INFO_POS] - m_vEye;
//	//_float fLength = D3DXVec3Length(&vCameraDir);
//
//	////vUpOffset.z = 0.f;
//
//	//vUpOffset.y += 5.f;
//	//vUpOffset.x = m_vEye.x;
//	//vUpOffset.z = m_vEye.z;
//
//	//if (m_vEye.y >= vUpOffset.y )
//	//{
//	//	m_vEye = vUpOffset;
//	//}
//}
