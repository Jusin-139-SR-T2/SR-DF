#pragma once
#include "AceUnit.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"
#include "Player.h"

// Team Agent �߰�����  

typedef struct tag_Monster_Info
{
	//m_tStat
	_float		fAttackDistance;					// ���� �þ߻�Ÿ�
	_float		fAttackFov;							// ���� �þ� ���� �ݰ� (90 = �þ߰��� 180��)

	_float		fConsider = 0.f;					// �÷��̾� ��ģ�ڿ� �ֺ����� ������ (���ҿ���)
	_float		fMaxConsider = 10.f;				// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 

	_float		fAwareness = 0;						// �ǽɰ����� ���� 
	_float		fMaxAwareness = 15.f;				// �ǽɰ����� max -> �߰����� ���� 

	_int		iDazedHP = 25;						// ���� �����ϴ� hp

	_vec3		vPatrolPointZero;					//�ʱ� ��������Ʈ 
}MONSTER;

typedef struct Frame
{
	//   m_tFrame
	_float		fFrame = 0.f;
	_float		fFrameEnd = 0.f;
	_float		fFrameSpeed = 0.f;
	_float		fCheck = 0;						//Taunt �� ������Ƚ��

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