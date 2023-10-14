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

class CPlayerBullet : public CPlayerAttackUnion
{
	DERIVED_CLASS(CPlayerAttackUnion, CPlayerBullet)

private:
	explicit CPlayerBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerBullet(const CPlayerBullet& rhs);
	virtual ~CPlayerBullet();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

	static CPlayerBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fMoveSpeed, CAceUnit* _Owner, PLAYER_ATTACK_STATE _AttackState, ETEAM_ID _eTeamID);

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(PLAYER_ATTACK_STATE, m_ePlayer_AttackState, Player_AttackState, GET, SET)

		// �浹 -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);

private:
	HRESULT				Add_Component();
	virtual void		Free();

	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr; //�÷��̾�뵵 

private:
	struct _PlayerBullet	// �÷��̾� �Ѿ�
	{
		_float fFrame = 0.f;		// ������
		_float fFrameEnd = 0.f;	// ������
		_float fFrameSpeed = 0.f; // �����Ӽӵ�
		_float fMoveSpeed = 50.f; // ����ü �߻� �ӵ� 
		_float fDamage = 0.f;		// ������

		_vec3 vDir = { 0.f, 0.f, 0.f }; // ����
	};

private:
	_PlayerBullet m_tBullet;

	_vec3 m_vPlayerPos = { 0.f, 0.f, 0.f }; // ������ �÷��̾� ��ġ
	
	_bool*	m_bDbugFrame;

};

