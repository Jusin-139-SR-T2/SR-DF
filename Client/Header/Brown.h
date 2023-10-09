#pragma once
#include "GameObject.h"
#include "Player.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

#include "BlackBoard_Monster.h"
#include "BlackBoardPtr.h"

#include "Awareness.h"
#include "MonsterPunch.h"


BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

enum PlayerHit {PUNCH, PISTOL, TOMSON, RUN, }; //�ӽÿ� 

class CBrown : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CBrown)

private:
	explicit CBrown(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBrown(const CBrown& rhs);
	virtual ~CBrown();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CBrown* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);

private:
	CRcBufferComp*			m_pBufferComp = nullptr;
	CTextureComponent*		m_pTextureComp = nullptr;
	CColliderComponent*		m_pColliderComp = nullptr;				// �� �浹 �ݶ��̴�
	CTransformComponent*	m_pTransformComp = nullptr;
	CCalculatorComponent*	m_pCalculatorComp = nullptr;
	CTransformComponent*	m_pPlayerTransformcomp = nullptr;
	
private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();
	void				Billboard(const _float& fTimeDelta); // �÷��̾������� ���ϴ� �Լ� 

	// Get, Set �Լ� ����� --------------------------------------------------
public: 
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(_bool, m_fBrownAwareness, Awareness, GET, SET)
		
	// �浹 -----------------------------------------------------------------
protected: 
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE FCollisionBox* pShape;

	// BlackBoard
public:
	void		Update_BlackBoard();

private:
	FBlackBoardPtr<CBlackBoard_Monster>		m_wpBlackBoard_Monster;


	// ���¸ӽ� ���� ---------------------------------------------------------
private:
	// �Լ� -----------------------------------------------------------------
	_bool		Detect_Player();					// ���� �þ߰����� �÷��̾ �ִ��� üũ
	_float		Calc_Distance();						// ���Ϳ� �÷��̾� ������ �Ÿ� üũ�ϴ� �Լ� 
	HRESULT     Get_PlayerPos(const _float& fTimeDelta); // �÷��̾� dynamic_cast�뵵 	

	PlayerHit   m_PlayerState;

	// ���� -----------------------------------------------------------------
	wchar_t		debugString[100];
	_float		m_fCheck = 0;						// ������ ������Ƚ�� ����
	_int		m_iHP;								// ���� ���� hp 
	_int		m_iPreHP;							// ���� HP ����뵵 
	_int		m_iDazedHP = 25;					// ���� �����ϴ� hp
	_int		m_iMaxHP = 100;						// ���� �ִ� hp 
	_int		m_iAttack = 15;						// ���� ���ݷ�

	_float		m_fFrame = 0.f;						// �̹��� ���������� �����Ӻ��� 
	_float		m_fFrameEnd;						// �̹������� ���� �־������ 
	_float		m_fFrameSpeed;						// ������ ������ �ӵ�

	_float		m_fAge = 0.f;		// ������ �ƴ� pause �ð� 
	_float		m_fLifeTime = 0.f;

	// ���� �ν� ���� 
	_float		m_fBrownAwareness = 0;				// �ǽɰ����� ���� 
	_float		m_fMaxAwareness = 15.f;				// �ǽɰ����� max -> �߰����� ���� 
	_float		m_fConsider = 10.f;					// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 
	_float		m_fMaxConsider = 10.f;				// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 

	// �ӵ����� 
	_float		m_fRunSpeed = 3.0f;					// �پ���� �ӵ�
	_float		m_fWalkSpeed = 2.0f;				// �ȴ¼ӵ�
	_float		m_fInchSpeed = 8.f;					// ������ �����ϸ� �����ϴ� �ӵ� 
	_float		m_fStrafingSpeed = 7.f;				// ������ �����ϴ�  �ӵ� 
	_float		m_fNormalAttackSpeed = 3.f;			// �Ϲݰ��ݶ� �پ���� �ӵ� 
	_float		m_fHeavAttackSpeed = 4.f;			// �����ݶ� �پ���� �ӵ� 

	// ��Ÿ� , �þ߰�
	_float		m_fMonsterFov = 90;					//�þ߰� - �ݰ� ����
	_float		m_fMonsterSightDistance = 12.f;		// ���Ͱ� �����ϴ� ��Ÿ� 

	_float		m_fRunDistance = 6.f;				// ��Ÿ� ~ Run ���� =  run
	_float		m_fWalkDistance = 5.5f;				// run~walk ���� = walk
	_float		m_fInchDistance = 1.5f;  

	// ��ġ ���� 
	_vec3		vPlayerPos;							// �÷��̾� ��ġ ����
	_vec3		vDir;								// ���Ͱ� �÷��̾� �ٶ󺸴� ����  
	_vec3		vPatrolPointZero;
	
	//����ġ on/off 
	_bool		Dead = false;
	_bool		DeadSpin = true;
	_bool		m_bArrive = false;
	_bool		m_bAwareness = false;
	_bool		m_ComboAttack = false;
	_bool		m_AgeTime = false;
	_bool		m_AttackOnce = false;

	// ���¸ӽ� enum --------------------------------------------------
public: 
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, TAUNT, REST, CHASE, 			 
		RUN, WALK, INCHFORWARD, STRAFING, 
		NORMALATTACK, HEAVYATTACK, 	// �Ÿ���ºκ�
		HIT, FACEPUNCH, CROTCHHIT,											// �ǰ����� 
		DAZED, FALLING, CHOPPED, DEATH, HEADLESS,							// ������
		RECONNAISSANCE, GOHOME,

		//�̱��� ����Ʈ 
		JUMP,
	};

	// �ൿ ���¸ӽ�
	enum class STATE_ACT {
		IDLE, APPROACH, MOVING, ATTACK, GOHOME
	};

	// �ൿŰ
	enum class ACTION_KEY { 
		IDLE, 
		RUN, WALK, INCHFORWARD, STRAFING, 
		JUMP, 
		NORMALATTACK, HEAVY_ATTACK,
		GOHOME 
	};

private:
	STATE_SET<STATE_OBJ, void(CBrown*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CBrown*, float)> m_tState_Act;				// �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey;									//���� ����Ű 

#pragma region AI 

	// ���� 
	void AI_Idle(float fDeltaTime); // SOUTH �ٶ󺸴��� 
	void AI_Suspicious(float fDeltaTime); // �� ? �ǽ���
	void AI_Taunt(float fDeltaTime); // �ٰ��ͺ� �ϰ� ���߻��� 
	void AI_Chase(float fDeltaTime); // �Ÿ� üũ�ؼ� ���� ���� ���������� ������ ��꿪�� 
	void AI_Rest(float fDeltaTime); // ���ڸ����� ���Ÿ�

	// �߰� 
	void AI_Run(float fDeltaTime); // �޸��� 
	void AI_Walk(float fDeltaTime); // �ȱ�
	void AI_InchForward(float fDeltaTime); // ������ �� �ٰ����� ���� �ڼ� 
	void AI_Strafing(float fDeltaTime); // �¿�� ���鼭 ��, �� ���� �ϳ��� �̵� 
	void AI_Jump(float fDeltaTime); // �����ϴ»��� - �÷��̾ �����ϸ� �������� �ڷ� ������ 

	// ����
	void AI_HeavyAttack(float fDeltaTime); // ������ 
	void AI_NormalATTACK(float fDeltaTime); // �Ϲ� ����
	
	// �ǰ�
	void AI_Hit(float fDeltaTime); // ���� ��Ʈ���� 
	void AI_Dazed(float fDeltaTime); // hp �����̻� ���� ���� 
	void AI_FacePunch(float fDeltaTime); // �󱼿� �¾������ 

	// ���� 
	void AI_Chopped(float fDeltaTime); // �ڿ��� �÷��̾ �������������� Sleep���� �� 
	void AI_Headless(float fDeltaTime); // �ѷ��� �Ӹ��� �¾������ 
	void AI_Death(float fDeltaTime); // hp 0�λ��� 

	// ����
	void AI_Reconnaissance(float fDeltaTime); // �÷��̾� ���ļ� �ֺ� �����ϴ��� 
	void AI_GoHome(float fDeltaTime);		 // ������ġ�� ����ġ ������ 

#pragma endregion

#pragma region �ൿ : AI ���� �Ѿ�°� 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);		// AI_Run + AI_Walk
	void Moving(float fDeltaTime);			// AI_InchForward + AI_Strafing
	void Attack(float fDeltaTime);			// AI_NORMALATTACK + AI_HeavyAttack
	void GoHome(float fDeltaTime);			// Gohome
#pragma endregion

};

/*
����� ���� 

OutputDebugString(L"�� : �浹 ���� ����� 
OutputDebugString(L"�� : ���¸ӽ� ���� ����� 

// �������̶� ���� 
swprintf_s(debugString, L"Brown - ���� Ȯ�� m_fAwareness = %f\n", m_fAwareness);
OutputDebugStringW(debugString);
*/