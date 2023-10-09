#pragma once
#include "AceMonster.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

#include "MonsterPunch.h"

#include "Awareness.h"
#include "FallingStone.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CBoss : public CAceMonster
{
	DERIVED_CLASS(CAceMonster, CBoss)

private:
	explicit CBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBoss(const CBoss& rhs);
	virtual ~CBoss();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();

	// Get, Set �Լ� ����� ---------------------------------------------------------------
public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

	// ���¸ӽ� ���� --------------------------------------------------
private:
	// �Լ� ----------
	_bool		Detect_Player();					// ���� �þ߰����� �÷��̾ �ִ��� üũ
	_float		Calc_Distance();					// ���Ϳ� �÷��̾� ������ �Ÿ� üũ�ϴ� �Լ� 
	void		Billboard(const _float& fTimeDelta); // �÷��̾������� ���ϴ� �Լ� 

	// ���� ----------
	_float		m_fCheck = 0;						//Taunt �� ������ ������Ƚ�� ����
	_int		m_iHP;								// ���� ���� hp 
	_int		m_iPreHP;							// ���� HP ����뵵 
	_int		m_iDazedHP = 25;					// ���� �����ϴ� hp
	_int		m_iMaxHP = 100;						// ���� �ִ� hp 
	_int		m_iAttack = 15;						// ���� ���ݷ�
	_int		m_iBulletCnt = 5;					// �������� �Ѿ� ���� 

	_float		m_fFrame = 0.f;						// �̹��� ���������� �����Ӻ��� 
	_float		m_fFrameEnd;						// �̹������� ���� �־������ 
	_float		m_fFrameSpeed;						// ������ ������ �ӵ�

	// ���� �ν� ���� 
	_float		m_fBossAwareness = 0;					// �ǽɰ����� ���� 
	_float		m_fMaxAwareness = 7.f;				// �ǽɰ����� max -> �߰����� ���� 
	_float		m_fConsider = 10.f;					// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 
	_float		m_fMaxConsider = 10.f;				// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 

	// �ӵ����� 
	_float		m_fRunSpeed = 3.0f;					// �پ���� �ӵ�
	_float		m_fWalkSpeed = 2.0f;				// �ȴ¼ӵ�
	_float		m_fRollingSpeed = 3.5f;				// ������ �����ϴ�  �ӵ� 

	// �ð������� 
	_float		m_MaxTime = 12.f; 
	_float		m_ShortTime = 4.f;
	_float		m_ChaseTime = 0.f;
	_float		m_PreAttackTime = 0.f;
	_float		m_SideAttackTime = 0.f;


	// ��Ÿ� , �þ߰�
	_float		m_fMonsterFov = 90;					//�þ߰� - �ݰ� ����
	_float		m_fMonsterSightDistance = 13.f;		// ���Ͱ� �����ϴ� ��Ÿ� 

	_float		m_fRunDistance = 8.f;				// ��Ÿ� ~ Run ���� =  13~8 ���̿� ��ġ 
	_float		m_fWalkDistance = 7.f;				// run~walk ���� = walk
	_float		m_fCloseAttackDistance = 2.f;		// Close ~ Eyes = 

	// ��ġ ���� 
	_vec3		vPlayerPos;							// �÷��̾� ��ġ ����
	_vec3		vDir;								// ���Ͱ� �÷��̾� �ٶ󺸴� ����  
	_vec3		vPatrolPointZero;
	//����ġ on/off 
	_bool m_bGoHome = FALSE;

public:
	// �þߴ� ����, ������ ������, �ǽɰ������� ������, 

	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ { 
	IDLE, SUSPICIOUS, RELOADING, CHASE, BACKIDLE,
	PRE_ATTACK, SIDE_READY,
	WALK, RUN, ROLL, CLOSEATTACK, SHOOTING,
	GOHOME};

	// �ൿ ���¸ӽ�
	enum class STATE_ACT { IDLE, APPROACHING, ROLLING, CLOSEATTACKING, SHOOTING, 
		GOHOME
	};

	// �ൿŰ
	enum class ACTION_KEY { IDLE, WALK, RUN, ROLL, CLOSE_ATTACK, SHOOTING, 
		GOHOME
	};

private:
	STATE_SET<STATE_OBJ, void(CBoss*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CBoss*, float)> m_tState_Act; // �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey; //���� ����Ű 

#pragma region AI 

	void AI_Idle(float fDeltaTime); // idle <-> suspicious
	void AI_Suspicious(float fDeltaTime);
	void AI_Reloading(float fDeltaTime); // idle <-> sus <-> detect
	void AI_BackIdle(float fDeltaTime);// ���۾��ʿ� : ���� ���� �Ⱥ������� 
	void AI_Chase(float fDeltaTime);

	void AI_Pre_Attack(float fDeltaTime);
	void AI_Side_Ready(float fDeltaTime);
	void AI_Walk(float fDeltaTime);
	void AI_Run(float fDeltaTime);
	void AI_Roll(float fDeltaTime);
	void AI_CloseAttack(float fDeltaTime);
	void AI_Shooting(float fDeltaTime);

	void AI_GoHome(float fDeltaTime);

#pragma endregion

#pragma region �ൿ : AI ���� �Ѿ�°� 
	void Idle(float fDeltaTime);
	void Approaching(float fDeltaTime);
	void Rolling(float fDeltaTime);
	void CloseAttacking(float fDeltaTime);
	void Shooting(float fDeltaTime);
	void GoHome(float fDeltaTime);
	

#pragma endregion
	// �׼�Ű�� CPP�ʿ� ������
};

//�������

/*
RedLaser = �÷��̾� ���� �־��ּ��� ���ؾ��� 

1 phase 
- GetOuttaHere ���� ��� 
- ������ �����鼭 �Ѹ��� 

2 phase 
- FallingStone : �����ð��������� ����ؼ� ��ȯ
- SlowThunder : �÷��̾� �ݰ� 2Radius ������ ������ ��ġ�� ��ȯ�Ǵ� Thunder	/ �ܼ��� ��ȯ�ǰ� �����Ӵٵ��� set_dead�ε� ������ �ſ����
- Fire : 2�ʰ� �÷��̾ ����ٴ����� ���Ŀ��� �̵��������� ��� ���� ������ �� 4�� ������Ƶ� �Ҹ�

3 phase 
- YourLate ���� ���
-> ���� �ڹ��� ȹ�� ���̼� ���� / �Ѿ� ȸ�� 
- ������ ���� ���� + �Ϲ����� �� �������� ����
- EnergyBall(����) + RedLaser(�Ϲ�)
- SlowThunder(�Ϲ�) + Fire(���� ����)
- Stone(�Ϲ�) + Laser(�Ϲ�) + Fire(���� ����)
- ����� : Holy�� �浹�ϰ��־�� ������� 30�� ��Ÿ�� 

*/