#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera :  public CGameObject
{
	DERIVED_CLASS(CGameObject, CCamera)
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera();

protected:
	virtual void	Free();

public:
	virtual HRESULT	Ready_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject() {}

public:
	GETSET_EX2(_vec3, m_vEye, Eye, SET, GET)
	GETSET_EX2(_vec3, m_vAt, At, GET, SET)
	GETSET_EX2(_vec3, m_vUp, Up, GET, SET)

protected:
	_vec3			m_vEye, m_vAt, m_vUp;
	_float			m_fFov, m_fAspect, m_fNear, m_fFar;
	_matrix			m_matView, m_matProj;

	_float			m_fWidth, m_fHeight;

private:
	_matrix* LookAtLH(_matrix* pmatOut, _vec3* pvEye, _vec3* pvAt, _vec3* pvUp);
	_matrix* PerspectiveFovLH(_matrix* pmatOut, _float fFov, _float fAspect, _float fNear, _float fFar);

};

END