#include "Camera.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev),
	m_vEye(), m_vAt(), m_vUp(),
	m_fFov(), m_fAspect(), m_fNear(), m_fFar()
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}

CCamera::~CCamera()
{
}

void CCamera::Free()
{
	SUPER::Free();
}

HRESULT CCamera::Ready_GameObject()
{
	// �� ����� ����� ��ġ���� �Ѱ� ���´�.
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	//LookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	// ���� ��ȯ ����� �ѹ� ������ָ� �ٽ� �ȸ���� �־ �ȴ�.
	// �ٸ� �ɼ��� �ٲ�� ����� �־�� �Ѵ�.
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	//PerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

_int CCamera::Update_GameObject(const _float& fTimeDelta)
{
	// �� ����� ī�޶� �� �����Ӹ��� �����̱� ������ �Ź� ����� �־���Ѵ�.
	LookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return 0;
}

_matrix* CCamera::LookAtLH(_matrix* pmatOut, _vec3* pvEye, _vec3* pvAt, _vec3* pvUp)
{
	_vec3 vDirection, vRight, vUp;
	// Look ���ϱ�
	vDirection = (*pvAt - *pvEye);
	D3DXVec3Normalize(&vDirection, &vDirection); // z

	// Right ���ϱ�
	D3DXVec3Cross(&vRight, pvUp, &vDirection);
	D3DXVec3Normalize(&vRight, &vRight);// x

	// Up ���ϱ�
	D3DXVec3Cross(&vUp, &vDirection, &vRight);
	D3DXVec3Normalize(&vUp, &vUp); //y

	(*pmatOut)._11 = vRight.x;		(*pmatOut)._12 = vUp.x;		(*pmatOut)._13 = vDirection.x;		(*pmatOut)._14 = 0.f;
	(*pmatOut)._21 = vRight.y;		(*pmatOut)._22 = vUp.y;		(*pmatOut)._23 = vDirection.y;		(*pmatOut)._24 = 0.f;
	(*pmatOut)._31 = vRight.z;		(*pmatOut)._32 = vUp.z;		(*pmatOut)._33 = vDirection.z;		(*pmatOut)._34 = 0.f;

	(*pmatOut)._41 = -D3DXVec3Dot(&vRight, pvEye);
	(*pmatOut)._42 = -D3DXVec3Dot(&vUp, pvEye);
	(*pmatOut)._43 = -D3DXVec3Dot(&vDirection, pvEye);
	(*pmatOut)._44 = 1.f;

	return pmatOut;
}

_matrix* CCamera::PerspectiveFovLH(_matrix* pmatOut, _float fFov, _float fAspect, _float fNear, _float fFar)
{
	//D3DXMatrixIdentity(pmatOut);
	memset(pmatOut, 0, sizeof(_matrix));

	_float Yndc, Xndc, Zndc;

	Yndc = 1.f / tanf(fFov * 0.5f);
	Xndc = Yndc / fAspect;
	Zndc = fFar / (fFar - fNear);

	pmatOut->_11 = Xndc;
	pmatOut->_22 = Yndc;
	pmatOut->_33 = Zndc;
	pmatOut->_43 = -Zndc * fNear;
	pmatOut->_44 = 0.f;
	pmatOut->_34 = 1.f;


	return pmatOut;
}
