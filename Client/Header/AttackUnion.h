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

class CAttackUnion : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CAttackUnion)

protected:
	explicit CAttackUnion(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAttackUnion(const CAttackUnion& rhs);
	virtual ~CAttackUnion();

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
	HRESULT						Update_PlayerPos();
	void						Knockback_Player(const _float& fTimeDelta, _float fSpeed);
	GAUGE<_float>				PlayerHp; //플레이어 HP 임시저장용 변수 

protected:
	MonsterPhase				m_CurrPahse;
	_tchar						debugString[100];
	FRAME						m_tFrame;

};