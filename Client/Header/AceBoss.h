#pragma once
#include "AceMonster.h"
#include "BossLight.h"

#include "BlackBoard_Monster.h"
#include "BlackBoardPtr.h"

//���� ��� 
#include "MonsterPunch.h"

//��ų �� ����Ʈ ��� 
#include "Awareness.h"
#include "RedLaser.h"
#include "FallingStone.h"
#include "SlowThunder.h"
#include "EnergyBall.h"
#include "SpawnFire.h"
#include "BlueBuff.h"
#include "RedThunder.h"

typedef struct BossDistance
{
	//_flaot fCloseAtk = 3.f; 
	_int a = 0;
};


class CAceBoss : public CAceMonster
{
	DERIVED_CLASS(CAceMonster, CAceBoss)

private:
	explicit CAceBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceBoss(const CAceBoss& rhs);
	virtual ~CAceBoss();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CAceBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(_float, m_tStat.fAwareness, Awareness, GET, SET)

	// �浹 
protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE FCollisionBox* pShape;
	
	//������ - Add BlackBoard���� �߰��� BoardName�� ���ε�, �ٿ�ε� �̸��� ��� ���ƾ��Ѵ�. 
private:
	void	Update_InternalData(); // ������� �����͸� �ø���. 
	void	Update_BlackBoard(); // ������κ��� �����͸� �޾ƿ��¿뵵 

protected:
	FBlackBoardPtr<CBlackBoard_Monster>		m_wpBlackBoard_Monster;

	// �ʿ��� ���� 
private:
	MonsterPhase m_ePhase;

	//�Ÿ����� 

	//���� ��Ʈ�ѿ��� - ������ ���� �Ϸ� 
	_bool m_bLightOn = FALSE;

#pragma region ���¸ӽ� enum����

public:
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, REST, CHASE,				// ���	  (SUS = BACKIDLE)
		RELOADING, RUN, WALK,						// �߰�
		PRE_ATTACK, SIDE_READY, ROLL, SHOUT,				// ���� 
		CLOSEATTACK,  SHOOT, SKILLATTACK,			// ����
		HIT, DAZED, FACEPUNCH, FALLING,	EVASION,	// �ǰ�
		DEATH,										// ����
		RECONNAISSANCE, GOHOME						// ����
	};

	// �ൿ ���¸ӽ�
	enum class STATE_ACT {
		IDLE, APPROACH, MOVING, ATTACK, SKILL, GOHOME
	};

	// �׼�Ű
	enum class ACTION_KEY {
		IDLE,
		RUN, WALK, ROLL, 
		ATTACK, SHOOT,
		SKILL_LASER, SKILL_STONE, SKILL_FIRE, SKILL_THUNDER, SKILL_ENERGYBALL, SKILL_BUFF,//��ų
		GOHOME
	};

private:
	STATE_SET<STATE_OBJ, void(CAceBoss*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CAceBoss*, float)> m_tState_Act;				// �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey;									//���� ����Ű 

#pragma endregion

#pragma region AI 

	// ��� 
	void AI_Idle(float fDeltaTime); // SOUTH �ٶ󺸴��� 
	void AI_Suspicious(float fDeltaTime); // �� ? �ǽ���
	void AI_Rest(float fDeltaTime); // ���ڸ����� ���Ÿ�
	void AI_Chase(float fDeltaTime); // �Ÿ� üũ�ؼ� ���� ���� ���������� ������ ��꿪�� 

	// �߰� 
	void AI_Run(float fDeltaTime); // �޸��� 
	void AI_Walk(float fDeltaTime); // �ȱ�
	void AI_Reloading(float fDeltaTime); // �Ѿ� ������

	//����
	void AI_PreAttack(float fDeltaTime); // �������� ���� 
	void AI_SideReady(float fDeltaTime); // ���� ������ �������� ���� 
	void AI_Roll(float fDeltaTime); // ������ ������ 
	void AI_Shout(float fDeltaTime); // ������ ������ 

	// ����
	void AI_CloseAttack(float fDeltaTime); // �������� - ���Ӹ������� �ĸ���  
	void AI_Shoot(float fDeltaTime); // ������ ��°��� 
	
	// ��ų
	void AI_SkillAttack(float fDeltaTime); // ��ų ����
	
	// �ǰ�
	void AI_Hit(float fDeltaTime); // ���� ��Ʈ���� 
	void AI_Dazed(float fDeltaTime); // hp �����̻� ���� ���� 
	void AI_Falling(float fDeltaTime); // ������ �¾������ 
	void AI_Evasion(float fDeltaTime); // ����ȸ�Ǳ� - ��ų ������ �ٲ����� ? ��   
	void AI_FacePunch(float fDeltaTime); // �󱼿� �¾������ 

	// ���� 
	void AI_Death(float fDeltaTime); //ok // hp 0�λ��� 

	// ����
	void AI_Reconnaissance(float fDeltaTime); // �÷��̾� ���ļ� �ֺ� �����ϴ��� 
	void AI_GoHome(float fDeltaTime);		 // ������ġ�� ����ġ ������ 

#pragma endregion

#pragma region �ൿ : AI ���� �Ѿ�°� 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);		// AI_Run + AI_Walk
	void Moving(float fDeltaTime);			// AI_InchForward + AI_Strafing
	void Attack(float fDeltaTime);			// AI_NORMALATTACK + AI_HeavyAttack
	void Skill(float fDeltaTime);			// ���� Skilló�� 
	void GoHome(float fDeltaTime);			// Gohome
#pragma endregion

};


/* SPEED ����

Speed 1 =
Speed 2 =
Speed 3 =
Speed 3.5 = WALK
Speed 4 =
Speed 5 = RUN
Speed 6 = GoHome
Speed 7 = Strafing
Speed 8 = Inch
Speed 9 =

����� ����

OutputDebugString(L"�� : �浹 ���� �����
OutputDebugString(L"�� : ���¸ӽ� ���� �����
OutputDebugString(L"�ڡڡڡڡ�  �߿� ����׶��� 

swprintf_s(debugString, L"Boss - ���� Ȯ�� m_fAwareness = %f\n", m_fAwareness);
OutputDebugStringW(debugString);

*/