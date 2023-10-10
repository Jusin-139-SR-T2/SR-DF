#pragma once
#include "GameObject.h"
#include "Export_System.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;
END

class CPlayerBullet : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CPlayerBullet)

private:
	explicit CPlayerBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerBullet(const CPlayerBullet& rhs);
	virtual ~CPlayerBullet();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CPlayerBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fMoveSpeed);

private:
	HRESULT				Add_Component();
	virtual void		Free();
	void				Billboard(const _float& fTimeDelta);

	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr; //플레이어용도 

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
		GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
		GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
		GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)

		// 충돌 -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);


private:
	_float m_fFrame;
	_float m_fFrameEnd;
	_float m_fFrameSpeed; // 프레임속도
	_float m_fMoveSpeed = 50.f; // 투사체 발사 속도 

	_vec3 m_vPlayerPos; // 생성시 플레이어 위치
	
	_bool*	m_bDbugFrame;
	_vec3 m_vDir = { 0.f, 0.f, 0.f };
};

