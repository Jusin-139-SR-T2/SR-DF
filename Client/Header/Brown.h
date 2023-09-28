#pragma once
#include "GameObject.h"
#include "Player.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

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

	static CBrown*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CRcBufferComp*			m_pBufferComp = nullptr;
	CTextureComponent*		m_pTextureComp = nullptr;
	CTransformComponent*	m_pTransformComp = nullptr;
	CTransformComponent*	m_pPlayerTransformcomp = nullptr;
	CCalculatorComponent*	m_pCalculatorComp = nullptr;
	CPlayer*				m_pPlayer = nullptr;

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();

	// Get, Set �Լ� ����� --------------------------------------------------
public: 
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(CPlayer*, m_pPlayer, Player, GET, SET)

	// ���¸ӽ� ���� ---------------------------------------------------------
private:
	// �Լ� -----------------------------------------------------------------
	_bool		Detect_Player();					// ���� �þ߰����� �÷��̾ �ִ��� üũ
	_float		m_fDistance();						// ���Ϳ� �÷��̾� ������ �Ÿ� üũ�ϴ� �Լ� 
	void		FaceTurn(const _float& fTimeDelta); // �÷��̾������� ���ϴ� �Լ� 
	HRESULT     Get_PlayerPos(const _float& fTimeDelta); // �÷��̾� dynamic_cast�뵵 
	//TestBoard -------------------------------------------------------------
	void		Monster_Jump(const _float& fTimeDelta); // 
	_bool		m_bJump = false;

	// ���� -----------------------------------------------------------------
	_float		m_fCheck = 0;						//Taunt �� ������ ������Ƚ�� ����
	_int		m_iHP;								// ���� ���� hp 
	_int		m_iPreHP;							// ���� HP ����뵵 
	_int		m_iDazedHP = 25;					// ���� �����ϴ� hp
	_int		m_iMaxHP = 100;						// ���� �ִ� hp 
	_int		m_iAttack = 15;						// ���� ���ݷ�

	_float		m_fFrame = 0.f;						// �̹��� ���������� �����Ӻ��� 
	_float		m_fFrameEnd;						// �̹������� ���� �־������ 
	_float		m_fFrameSpeed;						// ������ ������ �ӵ�

	// ���� �ν� ���� 
	_float		m_fAwareness = 0;					// �ǽɰ����� ���� 
	_float		m_fMaxAwareness = 10.f;				// �ǽɰ����� max -> �߰����� ���� 
	_float		m_fConsider = 10.f;					// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 
	_float		m_fMaxConsider = 10.f;				// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 

	// �ӵ����� 
	_float		m_fRunSpeed = 2.0f;					// �پ���� �ӵ�
	_float		m_fWalkSpeed = 1.0f;				// �ȴ¼ӵ�
	_float		m_fInchSpeed = 4.f;					// ������ �����ϸ� �����ϴ� �ӵ� 
	_float		m_fStrafingSpeed = 8.f;				// ������ �����ϴ�  �ӵ� 
	_float		m_fBasicAttackSpeed = 3.f;			// �Ϲݰ��ݶ� �پ���� �ӵ� 
	_float		m_fHeavAttackSpeed = 4.f;			// �����ݶ� �پ���� �ӵ� 

	// ��Ÿ� , �þ߰�
	_float		m_fMonsterFov = 90;					//�þ߰� - �ݰ� ����
	_float		m_fMonsterSightDistance = 12.f;		// ���Ͱ� �����ϴ� ��Ÿ� 
	_float		m_fRunDistance = 7.f;				// ��Ÿ� ~ Run ���� =  run
	_float		m_fWalkDistance = 6.5f;				// run~walk ���� = walk
	_float		m_fInchDistance = 1.5f;				// inch ~ walk ���� = inch/strafy & 0 ~inch = attack 

	// ��ġ ���� 
	_vec3		vPlayerPos;							// �÷��̾� ��ġ ����
	_vec3		vDir;								// ���Ͱ� �÷��̾� �ٶ󺸴� ����  

	//����ġ on/off 
	_bool Dead = false;
	_bool DeadSpin = true;
	_bool m_bGoHome = false;

	// ���¸ӽ� enum --------------------------------------------------
public: 
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, TAUNT, CHASE, REST,			// intro 
		RUN, WALK, INCHFORWARD, STRAFING, BASICATTACK, HEAVYATTACK, JUMP,	// �Ÿ���ºκ�
		HIT, FACEPUNCH, CROTCHHIT,											// �ǰ����� 
		DAZED, FALLING, CHOPPED, DEATH, HEADLESS,							// ������
		RECONNAISSANCE, GOHOME
	};

	// �ൿ ���¸ӽ�
	enum class STATE_ACT {
		IDLE, APPROACH, MOVING, ATTACK, GOHOME
	};

	// �ൿŰ
	enum class ACTION_KEY { IDLE, RUN, WALK, INCHFORWARD, STRAFING, JUMP, 
		BASIC_ATTACK, HEAVY_ATTACK, GOHOME };

private:
	STATE_SET<STATE_OBJ, void(CBrown*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CBrown*, float)> m_tState_Act;				// �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey;									//���� ����Ű 

#pragma region AI 

	void AI_Idle(float fDeltaTime); 
	void AI_Suspicious(float fDeltaTime); 
	void AI_Taunt(float fDeltaTime);
	void AI_Chase(float fDeltaTime);
	void AI_Rest(float fDeltaTime);

	void AI_Run(float fDeltaTime);
	void AI_Walk(float fDeltaTime);
	void AI_InchForward(float fDeltaTime);
	void AI_Strafing(float fDeltaTime);
	void AI_BasicAttack(float fDeltaTime);
	void AI_HeavyAttack(float fDeltaTime);
	void AI_Jump(float fDeltaTime);

	void AI_Hit(float fDeltaTime);
	void AI_FacePunch(float fDeltaTime);
	void AI_HitByPitchedBall(float fDeltaTime);

	void AI_Dazed(float fDeltaTime);
	void AI_Chopped(float fDeltaTime);
	void AI_Headless(float fDeltaTime);
	void AI_Death(float fDeltaTime);

	void AI_Reconnaissance(float fDeltaTime); // �÷��̾� ���ļ� �ֺ� �����ϴ��� 
	void AI_GoHome(float fDeltaTime);		 // ������ġ�� ����ġ ������ 

#pragma endregion

#pragma region �ൿ : AI ���� �Ѿ�°� 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);		// AI_Run + AI_Walk
	void Moving(float fDeltaTime);			// AI_InchForward + AI_Strafing
	void Attack(float fDeltaTime);			// AI_BasicAttack + AI_HeavyAttack
	void GoHome(float fDeltaTime);			// Gohome
#pragma endregion
};

