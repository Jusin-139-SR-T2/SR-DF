#pragma once
#include "AceUnit.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Define.h"

// Team Agent 추가예정  
BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CColliderComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CPlayerAttackUnion : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CPlayerAttackUnion)

protected:
	explicit CPlayerAttackUnion(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerAttackUnion(const CPlayerAttackUnion& rhs);
	virtual ~CPlayerAttackUnion();

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

public: //플레이어 관련 셋팅 
	void						Change_PlayerHp(_float pAttack);
	void						Change_MonsterHp(_float pAttack, CGameObject* _AttackTarget);
	HRESULT						Update_PlayerPos();
	void						Recoil_Player(const _float& fTimeDelta, _float fSpeed);
	GAUGE<_float>				PlayerHp; //플레이어 HP 저장용 변수 
	GAUGE<_float>				MonsterHp; //몬스터 HP 저장용 변수 

protected:
	MonsterPhase				m_CurrPahse;
	ATTACK_TARGET				m_eAttack_Target;
	_tchar						debugString[100];
	FRAME						m_tFrame;

};