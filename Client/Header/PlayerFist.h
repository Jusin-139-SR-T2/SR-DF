#pragma once
#include "PlayerAttackUnion.h"
#include "Export_System.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CPlayerFist : public CPlayerAttackUnion
{
	DERIVED_CLASS(CPlayerAttackUnion, CPlayerFist)

private:
	explicit CPlayerFist(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerFist(const CPlayerFist& rhs);
	virtual ~CPlayerFist();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CPlayerFist* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vDir, CAceUnit* _Owner, PLAYER_ATTACK_STATE _AttackState, ETEAM_ID _eTeamID);

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
		GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
		GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
		GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
		GETSET_EX2(PLAYER_ATTACK_STATE, m_ePlayer_AttackState, Player_AttackState, GET, SET)

		// 충돌 -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionEntered(CGameObject* pDst, const FContact* const pContact);
	virtual void	OnCollisionExited(CGameObject* pDst);

private:
	HRESULT				Add_Component();
	virtual void		Free();

	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr; //플레이어용도 
	FCollisionSphere* pShape;
	//LPD3DXMESH	m_MeshSphere;

private:
	struct _PlayerFist	// 플레이어 총알
	{
		_float fFrame = 0.f;		// 프레임
		_float fFrameEnd = 0.f;	// 프레임
		_float fFrameSpeed = 0.f; // 프레임속도
		_float fMoveSpeed = 50.f; // 투사체 발사 속도 
		_float fDamage = 0.f;		// 데미지
		_float fDeleteTime = 0.f;

		_vec3 vDir = { 0.f, 0.f, 0.f }; // 방향
	};

private:
	_PlayerFist m_tFist;

	_vec3 m_vPlayerPos = { 0.f, 0.f, 0.f }; // 생성시 플레이어 위치

	_bool* m_bDbugFrame;

};

