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
class CSphereColComp;

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
	CColliderComponent*		m_pColliderComp = nullptr;				// �� �浹 �ݶ��̴�

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();

	//�ൿ���� �˾Ƴ��°�
public:
	_bool m_bAwareness = false;
	
	// Get, Set �Լ� ����� --------------------------------------------------
public: 
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET) // �浹 �ʼ� 
	GETSET_EX2(CPlayer*, m_pPlayer, Player, GET, SET)

protected: // �浹 onoff
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst) ;
	virtual void	OnCollisionExited(CGameObject* pDst) ;




	// ���¸ӽ� ���� ---------------------------------------------------------
private:
	// �Լ� -----------------------------------------------------------------
	_bool		Detect_Player();					// ���� �þ߰����� �÷��̾ �ִ��� üũ
	_float		m_fDistance();						// ���Ϳ� �÷��̾� ������ �Ÿ� üũ�ϴ� �Լ� 
	void		Billboard(const _float& fTimeDelta); // �÷��̾������� ���ϴ� �Լ� 
	HRESULT     Get_PlayerPos(const _float& fTimeDelta); // �÷��̾� dynamic_cast�뵵 

	// ���� -----------------------------------------------------------------
	_float		m_fCheck = 0;						// ������ ������Ƚ�� ����
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
	_vec3		m_vFirstPos;
	//����ġ on/off 
	_bool		Dead = false;
	_bool		DeadSpin = true;
	_bool		m_bGoHome = false;


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

	void AI_Idle(float fDeltaTime); // SOUTH �ٶ󺸴��� 
	void AI_Suspicious(float fDeltaTime); // �� ? �ǽ���
	void AI_Taunt(float fDeltaTime); // �ٰ��ͺ� �ϰ� ���߻��� 
	void AI_Chase(float fDeltaTime); // �Ÿ� üũ�ؼ� ���� ���� ���������� ������ ��꿪�� 
	void AI_Rest(float fDeltaTime); // ���ڸ����� ���Ÿ�

	void AI_Run(float fDeltaTime); // �޸��� 
	void AI_Walk(float fDeltaTime); // �ȱ�
	void AI_InchForward(float fDeltaTime); // ������ �� �ٰ����� ���� �ڼ� 
	void AI_Strafing(float fDeltaTime); // �¿�� ���鼭 ��, �� ���� �ϳ��� �̵� 
	void AI_BasicAttack(float fDeltaTime); // �Ϲ� ����
	void AI_HeavyAttack(float fDeltaTime); // ������ 
	void AI_Jump(float fDeltaTime); // �����ϴ»��� - �÷��̾ �����ϸ� �������� �ڷ� ������ 

	void AI_Hit(float fDeltaTime); // ���� ��Ʈ���� 
	void AI_FacePunch(float fDeltaTime); // �󱼿� �¾������ 
	void AI_HitByPitchedBall(float fDeltaTime); // �ϴܿ� ����������� 

	void AI_Dazed(float fDeltaTime); // hp �����̻� ���� ���� 
	void AI_Chopped(float fDeltaTime); // �ڿ��� �÷��̾ �������������� Sleep���� �� 
	void AI_Headless(float fDeltaTime); // �ѷ��� �Ӹ��� �¾������ 
	void AI_Death(float fDeltaTime); // hp 0�λ��� 

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

