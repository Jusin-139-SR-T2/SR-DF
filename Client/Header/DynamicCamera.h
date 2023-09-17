#pragma once

#include "Camera.h"

#include "Engine_Macro.h"

class CDynamicCamera : public Engine::CCamera
{
	DERIVED_CLASS(CCamera, CDynamicCamera)
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera();

public:
	static CDynamicCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
									const _vec3* pEye,
									const _vec3* pAt,
									const _vec3* pUp,
									const _float& fFov = D3DXToRadian(60.f),
									const _float& fAspect = (float)WINCX / WINCY,
									const _float& fNear = 0.1f,
									const _float& fFar = 1000.f);

private:
	virtual void Free();

public:
	HRESULT Ready_GameObject(const _vec3* pEye,
								const _vec3* pAt,
								const _vec3* pUp,
								const _float& fFov,
								const _float& fAspect,
								const _float& fNear,
								const _float& fFar);

	virtual _int Update_GameObject(const _float& fTimeDelta) override;

private:
	void		Check_KeyInput(const _float& fTimeDelta);
	void		Mouse_Move();
	void		Mouse_Fix();
	void		Camera_State(const _float& fTimeDelta);

private:
	_float		m_fSpeed = 10.f;
	_bool		m_bFix = false;
	_bool		m_bCheck = false;

	//3인칭 스위치
	_bool		m_Change = false;
	_bool		m_bOne = false;

	//1인칭 스위치
	_bool		m_Change2 = false;
	_bool		m_bThree = false;

	//마우스 무브 좌표
	_matrix		m_matRotX;
	_matrix		m_matRotY;

private:
	CTransformComponent* pPlayerTransCom = nullptr;

public:
	GETSET_EX2(CGameObject*, m_pTarget, Target, GET, SET)

private:
	CGameObject*	m_pTarget = nullptr;
	_float			m_fTarget_Distance = 10.f;


};

