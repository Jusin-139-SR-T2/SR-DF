#pragma once
#include "AceMonster.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

#include "MonsterPunch.h"

#include "Awareness.h"
#include "RedLaser.h"
#include "FallingStone.h"
#include "SlowThunder.h"
#include "EnergyBall.h"
#include "SpawnFire.h"
#include "BlueBuff.h"

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
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	void				Billboard(const _float& fTimeDelta); // �÷��̾������� ���ϴ� �Լ� 
	virtual void		Free();

	// Get, Set �Լ� ����� ---------------------------------------------------------------
public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	
	GETSET_EX2(_float, m_fBossAwareness, BossAwareness, GET, SET)   
	GETSET_EX2(GAUGE<_float>, m_gHp, BossHP, GET, SET)   // ����  hp�뵵 
		// �浹 -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE			FCollisionBox* pShape;

	// ���¸ӽ� ���� --------------------------------------------------
private:
	// �Լ� ----------
	HRESULT     Get_PlayerPos(const _float& fTimeDelta); // �÷��̾� dynamic_cast�뵵 	
	_bool		Detect_Player();					// ���� �þ߰����� �÷��̾ �ִ��� üũ
	_float		Calc_Distance();					// ���Ϳ� �÷��̾� ������ �Ÿ� üũ�ϴ� �Լ� 
	void		Phase_Check();

	// ��ų����
	void Falling_Stone_Around(); // ������ġ�� ������� 
	void Pattern_Fire(_int number, _float radius); // radius �������� ������ number��ŭ ������ �ѷ��ΰ� ������� 
	void Pattern_EnergyBall(); // �����տ��� �Ϸķ� ������� - ���� 
	void Pattern_SlowThunder();

	// ���� ----------
	wchar_t		debugString[100];	
	_float		m_fCheck = 0;						//Taunt �� ������ ������Ƚ�� ����
	_float		m_iDazedHP = 25;					// ���� �����ϴ� hp
	_float		phase1HP = 80;
	_float		phase2HP = 50;
	_int		m_iBulletCnt = 5;					// �������� �Ѿ� ���� 

	_float		m_fFrame = 0.f;						// �̹��� ���������� �����Ӻ��� 
	_float		m_fFrameEnd;						// �̹������� ���� �־������ 
	_float		m_fFrameSpeed;						// ������ ������ �ӵ�

	// ���� �ν� ���� 
	_float		m_fBossAwareness = 0;					// �ǽɰ����� ���� 
	_float		m_fMaxAwareness = 15.f;				// �ǽɰ����� max -> �߰����� ���� 
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

	_float		m_fPatternAge = 0.f;
	_float		m_fPatternLifeTime = 3.f;

	// ��Ÿ� , �þ߰�
	_float		m_fMonsterFov = 90;					//�þ߰� - �ݰ� ����
	_float		m_fMonsterSightDistance = 18.f;		// ���Ͱ� �����ϴ� ��Ÿ� 
	
	//�⺻phase�� 
	_float		m_fBasicRunDistance = 9.f;
	_float		m_fPH1basicDistance = 12.f;
	_float		m_fChaseDistance = 15.f; 

	_float		m_fRunDistance = 8.f;				// ��Ÿ� ~ Run ���� =  13~8 ���̿� ��ġ 
	_float		m_fWalkDistance = 7.f;				// run~walk ���� = walk
	_float		m_fCloseAttackDistance = 2.f;		// Close ~ Eyes = 

	// ��ġ ���� 
	_vec3		vPlayerPos;							// �÷��̾� ��ġ ����
	_vec3		vDir;								// ���Ͱ� �÷��̾� �ٶ󺸴� ����  
	_vec3		vPatrolPointZero;

	// bool on/off 
	_bool m_bGoHome = FALSE;
	_bool m_bDead = FALSE;
	_bool m_bArrive = FALSE;
	_bool m_bCheck = FALSE;
	_bool TimerReset = FALSE;
	_bool m_bPH1_RedLaser = TRUE;
	_bool m_bPH2_Buff = TRUE;
	_bool m_bPH2_Route = FALSE;

	// enum 
	MonsterPhase m_eCurrPhase;

public:
	enum class STATE_OBJ { 
	IDLE, SUSPICIOUS, RELOADING, BACKIDLE,
	CHASE, KEEP_DISTANCE, PATTERNCHECK,
	PRE_ATTACK, SIDE_READY,
	WALK, RUN, ROLL, CLOSEATTACK, SHOOTING,
	RECONNAISSANCE, GOHOME, TRACK, 
	
	//����üũ 
	PH1_LASER
	};

	// �ൿ ���¸ӽ�
	enum class STATE_ACT { 
		IDLE, 
		APPROACHING, RUN,KEEPDISTANCE,
		ROLLING, 
		CLOSEATTACKING, SHOOTING, 
		GOHOME, TRACK,

		//����
		PHASE1_LASER	};

	// �ൿŰ
	enum class ACTION_KEY { 
		IDLE, 
		WALK, RUN, KEEPDISTANCE,
		ROLL, TRACK,
		CLOSE_ATTACK, SHOOTING, 
		GOHOME,      
		//����
		NORMAL_LASER 
	};

private:
	STATE_SET<STATE_OBJ, void(CBoss*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CBoss*, float)> m_tState_Act; // �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey; //���� ����Ű 

#pragma region AI 

	void AI_Idle(float fDeltaTime); 
	void AI_Suspicious(float fDeltaTime);
	void AI_Reloading(float fDeltaTime); 
	void AI_BackIdle(float fDeltaTime); //ǰ�ȿ� �� ������ 

	void AI_Chase(float fDeltaTime);
	void AI_KeepDistance(float fDeltaTime);
	void AI_Walk(float fDeltaTime);
	void AI_Run(float fDeltaTime);

	void AI_Pre_Attack(float fDeltaTime);
	void AI_Side_Ready(float fDeltaTime);
	void AI_Roll(float fDeltaTime);
	void AI_CloseAttack(float fDeltaTime);
	void AI_Shooting(float fDeltaTime);

	void AI_GoHome(float fDeltaTime);
	void AI_Reconnaissance(float fDeltaTime);
	void AI_Track(float fDeltaTime);

	//������ AI 
	void AI_Ph1_Laser(float fDeltaTime);

#pragma endregion

#pragma region �ൿ : AI ���� �Ѿ�°� 
	void Idle(float fDeltaTime);
	void Approaching(float fDeltaTime); //  walk 
	void Run(float fDeltaTime); 
	void Rolling(float fDeltaTime); 
	void GoHome(float fDeltaTime); 
	void KeepDistance(float fDeltaTime); 
	void Phase1_LaserOn(float fDeltaTime);
	void Track(float fDeltaTime);

#pragma endregion

	void Get_RandomVector(_vec3* out, _vec3* min, _vec3* max)
	{
		out->x = Get_RandomFloat(min->x, max->x);
		out->y = Get_RandomFloat(min->y, max->y);
		out->z = Get_RandomFloat(min->z, max->z);
	}

	_float Get_RandomFloat(_float lowBound, _float hightBound)
	{
		if (lowBound >= hightBound)
			return lowBound;

		_float f = (rand() % 10000) * 0.0001f;

		return (f * (hightBound - lowBound)) + lowBound;
	}
	void GetRandomPointInCircle(_vec3* out, _vec3* center, float radius)
	{
		// ������ ������ ������ ���� �� �������� ����

		_float angle = static_cast<_float>(rand()) / RAND_MAX * 2 * D3DX_PI;
		_float r = static_cast<_float>(rand()) / RAND_MAX * radius;

		// ����ǥ�� ���� ��ǥ�� ��ȯ
		out->x = center->x + r * cosf(angle);
		out->y = center->y;
		out->z = center->z + r * sinf(angle);
	}


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