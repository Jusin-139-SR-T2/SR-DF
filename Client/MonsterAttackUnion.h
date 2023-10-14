#pragma once
#include "AttackUnion.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CColliderComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CMonsterAttackUnion : public CAttackUnion
{
	DERIVED_CLASS(CAttackUnion, CMonsterAttackUnion)

protected:
	explicit CMonsterAttackUnion(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonsterAttackUnion(const CMonsterAttackUnion& rhs);
	virtual ~CMonsterAttackUnion();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	virtual void		Free() override;

protected:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr;

public:
	void						Height_On_Terrain(_float Height);
	void						Billboard();

public:
	void						Change_PlayerHp(_float pAttack);
	void						Knockback_Player(const _float& fTimeDelta, _float fSpeed);

protected:
	_tchar						debugString[100];	//����׿� string
	MonsterPhase				m_CurrPahse;		// ��ȭ���� ����� 
	GAUGE<_float>				PlayerHp;			// �÷��̾� HP �ӽ������ ���� 
	FRAME						m_tFrame;			// �����ӱ���ü 
	_vec3						vPlayerPos;			//�÷��̾� ��ġ ���� 
};