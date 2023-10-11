#pragma once
#include "AceUnit.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"
#include "Player.h"

// Team Agent 추가예정  

typedef struct tag_Monster_Info
{
	//m_tStat
	_float		fAttackDistance;					// 몬스터 시야사거리
	_float		fAttackFov;							// 몬스터 시야 사이 반각 (90 = 시야각도 180도)

	_float		fConsider = 0.f;					// 플레이어 놓친뒤에 주변정찰 게이지 (감소예정)
	_float		fMaxConsider = 10.f;				// 플레이어 놓친뒤에 주변정찰 게이지 

	_float		fAwareness = 0;						// 의심게이지 숫자 
	_float		fMaxAwareness = 15.f;				// 의심게이지 max -> 추격으로 변함 

	_int		iDazedHP = 25;						// 몬스터 기절하는 hp

	_vec3		vPatrolPointZero;					//초기 시작포인트 
}MONSTER;

typedef struct Frame
{
	//   m_tFrame
	_float		fFrame = 0.f;
	_float		fFrameEnd = 0.f;
	_float		fFrameSpeed = 0.f;
	_float		fCheck = 0;						//Taunt 등 돌리는횟수

	_float		fAge = 0.f;
	_float		fLifeTime = 0.f;

}FRAME;


BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CColliderComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CPlayer;

class CAceMonster : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CAceMonster)

protected:
	explicit CAceMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceMonster(const CAceMonster& rhs);
	virtual ~CAceMonster();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() PURE;
	virtual void		Free() override;

protected:
	CPlayer::STATE_RIGHTHAND	ePlayerRighthand;
	CPlayer::STATE_PLAYER		ePlayerState;
	MONSTER						m_tStat;
	FRAME						m_tFrame;
	_vec3						vPlayerPos;			
	_tchar						debugString[100];

public:
	CRcBufferComp*				m_pBufferComp = nullptr; 
	CTextureComponent*			m_pTextureComp = nullptr;
	CColliderComponent*			m_pColliderComp = nullptr;
	CTransformComponent*		m_pTransformComp = nullptr;
	CCalculatorComponent*		m_pCalculatorComp = nullptr;
	CTransformComponent*		m_pPlayerTransformcomp = nullptr;

public:
	_float						Calc_Distance();
	_bool						Detect_Player();
	HRESULT						Get_PlayerPos();
	void						Height_On_Terrain();
	void						Billboard(const _float& fTimeDelta); 

};