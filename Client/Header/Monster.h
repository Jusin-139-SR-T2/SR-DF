#pragma once
#include "GameObject.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

class CMonster : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CMonster)

private:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CMonster*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CRcBufferComp*			m_pBufferComp = nullptr;
	CTextureComponent*		m_pTextureComp = nullptr;
	CTransformComponent*	m_pTransformComp = nullptr;
	CTransformComponent*	m_pPlayerTransformcomp = nullptr;
	CCalculatorComponent*	m_pCalculatorComp = nullptr;
		
private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();

// Get, Set �Լ� ����� ---------------------------------------------------------------
public: 
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

// ���¸ӽ� ���� --------------------------------------------------
private:
	_bool		Monster_Capture();  // ���� �þ߰����� �÷��̾ �ִ��� üũ
	_float		m_fDistance();		// ���Ϳ� �÷��̾� ������ �Ÿ� üũ�ϴ� �Լ� 
	HRESULT		Chase_Player(const _float& fTimeDelta, _float fSpeed);

	_int		m_iHP;				// ���� hp 
	_int		m_iAttack = 15;		// ���� ���ݷ�

	_float		m_fFrame = 0.f;		// �̹��� ���������� �����Ӻ��� 
	_float		m_fFrameEnd;		// �̹������� ���� �־������ 
	_float		m_fFrameSpeed;		// ������ ������ �ӵ�

	_float		m_fAwareness = 0;	// �ǽɰ����� ���� 
	_float		m_fMaxAwareness = 20.f; // �ǽɰ����� max -> �߰����� ���� 
	
	_float		m_fRunSpeed = 100.f; // �ȴ¼ӵ�
	_float		m_fWalkSpeed = 30.f;  // �پ���� �ӵ� 
	_float		m_fInchSpeed = 30.f;  // �پ���� �ӵ� 
	_float		m_fStrafyingSpeed = 3.f;  // �پ���� �ӵ� 
	_float		m_fBasicAttackSpeed = 3.f;  // �پ���� �ӵ� 
	_float		m_fHeavAttackSpeed = 3.f;  // �پ���� �ӵ� 

	_float		m_fCheck = 0; //Taunt �� ������ ������Ƚ�� ����

	// ��Ÿ� , �þ߰� ���� 
	_float		m_fMonsterFov = 90;		//�þ߰� - �ݰ� ����
	_float		m_fMonsterSightDistance = 12.f; // ���Ͱ� �����ϴ� ��Ÿ� 
	_float		m_fRunDistance = 8.f;
	_float		m_fWalkDistance = 7.f;
	_float		m_fInchDistance = 4.f;


public: 
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ { IDLE, SUSPICIOUS, TAUNT, CHASE, RUN, WALK, INCHFORWARD, STRAFYING, BASICATTACK, HEAVYATTACK };
	// �ൿ ���¸ӽ�
	enum class STATE_ACT { IDLE, APPROACH };
	// �ൿŰ
	enum class ACTION_KEY { IDLE, RUN, WALK, INCH, STRAFING, JUMP, BASIC_ATTACK, HEAVY_ATTACK,   };

private:
	STATE_SET<STATE_OBJ, void(CMonster*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CMonster*, float)> m_tState_Act; // �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey; //���� ����Ű 

#pragma region AI 

	void AI_Idle(float fDeltaTime); // idle <-> suspicious
	void AI_Suspicious(float fDeltaTime); // idle <-> sus <-> detect
	void AI_Taunt(float fDeltaTime);
	void AI_Chase(float fDeltaTime);

	void AI_Run(float fDeltaTime);
	void AI_Walk(float fDeltaTime);
	void AI_InchForward(float fDeltaTime);
	void AI_Strafing(float fDeltaTime);
	void AI_BasicAttack(float fDeltaTime);
	void AI_HeavyAttack(float fDeltaTime);

#pragma endregion

#pragma region �ൿ : AI ���� �Ѿ�°� 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);

	//void Walk(float fDeltaTime);
	//void Inch(float fDeltaTime);

	//void Heavy_Attack(float fDeltaTime);
	//void Prepare_Atk(float fDeltaTime);
	//void Attack(float fDeltaTime);
	//void Parrying(float fDeltaTime);
	//
	//void Jump(float fDeltaTime);
	//void Falling(float fDeltaTime);
	//void Dead(float fDeltaTime);
#pragma endregion
	// �׼�Ű�� CPP�ʿ� ������
};

