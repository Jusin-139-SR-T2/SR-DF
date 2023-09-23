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
	_float		m_fCheck = 0;		//Taunt �� ������ ������Ƚ�� ����

	_int		m_iHP;				// ���� hp 
	_int		m_iAttack = 15;		// ���� ���ݷ�

	_float		m_fFrame = 0.f;		// �̹��� ���������� �����Ӻ��� 
	_float		m_fFrameEnd;		// �̹������� ���� �־������ 
	_float		m_fFrameSpeed;		// ������ ������ �ӵ�

	_float		m_fAwareness = 0;	// �ǽɰ����� ���� 
	_float		m_fMaxAwareness = 10.f; // �ǽɰ����� max -> �߰����� ���� 

	// �ӵ����� 
	_float		m_fRunSpeed = 1.5f; // �پ���� �ӵ�
	_float		m_fWalkSpeed = 0.7f;  // �ȴ¼ӵ�
	_float		m_fInchSpeed = 3.f;  // ������ �����ϸ� �����ϴ� �ӵ� 
	_float		m_fStrafingSpeed = 5.f;  // ������ �����ϴ�  �ӵ� 
	_float		m_fBasicAttackSpeed = 3.f;  // �Ϲݰ��ݶ� �پ���� �ӵ� 
	_float		m_fHeavAttackSpeed = 4.f;  // �����ݶ� �پ���� �ӵ� 

	// ��Ÿ� , �þ߰�
	_float		m_fMonsterFov = 90;		//�þ߰� - �ݰ� ����
	_float		m_fMonsterSightDistance = 12.f; // ���Ͱ� �����ϴ� ��Ÿ� 
	_float		m_fRunDistance = 8.f;
	_float		m_fWalkDistance = 7.5f;
	_float		m_fInchDistance = 3.f;

	// ��ġ ���� 
	_vec3	vPlayerPos, vDir;

	//����ġ on/off 
public: 
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ { IDLE, SUSPICIOUS, TAUNT, CHASE, RUN, WALK, INCHFORWARD, STRAFING, BASICATTACK, HEAVYATTACK };
	// �ൿ ���¸ӽ�
	enum class STATE_ACT { IDLE, APPROACH , MOVING};
	// �ൿŰ
	enum class ACTION_KEY { IDLE, RUN, WALK, INCHFORWARD, STRAFING, JUMP, BASIC_ATTACK, HEAVY_ATTACK,  };

private:
	STATE_SET<STATE_OBJ, void(CBrown*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CBrown*, float)> m_tState_Act; // �ൿ
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

	void Moving(float fDeltaTime);
#pragma endregion
	// �׼�Ű�� CPP�ʿ� ������
};

