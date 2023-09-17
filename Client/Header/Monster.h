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

private:
	CRcBufferComp*			m_pBufferComp = nullptr;
	CTransformComponent*	m_pTransformComp = nullptr;
	CTransformComponent*	m_pPlayerTransformcomp = nullptr;
	CTextureComponent*		m_pTextureComp = nullptr;
	CCalculatorComponent*	m_pCalculatorComp = nullptr;
	CGameObject*	m_pTarget = nullptr;

private:
	HRESULT			Add_Component();
	virtual void	Free();

private:
	void				Height_On_Terrain();
	_float				m_fFrame = 0.f; // �̹��� ���������� �����Ӻ��� 
	_float				m_fsuspicious;
	float				m_fEyetheta = D3DX_PI / 2.f; // ���� �þ߰� 

public:
	void				Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }
	static CMonster*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	_matrix				m_matRotAxis;

private:
	_bool				Monster_Capture(); // ���� �þ߰����� �÷��̾ �ִ��� üũ
	float					m_fDistance(); // ���Ϳ� �÷��̾� ������ �Ÿ� üũ�ϴ� �Լ� 

//---------------------------------------------------------------

public: //Get, Set �Լ� ����� 
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

// ���¸ӽ� ���� --------------------------------------------------
public:
	// info ���� (ü, ��, ��x)
	int m_iHP; // ���� hp 
	int m_iAttack = 15; // ���� ���ݷ�
	int m_iAwareness = 0; // �ǽɰ����� ���� 
	int m_iMaxAwareness = 15; // 

public: 
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ { IDLE, SUSPICIOUS, CHASE, ATTACK, JUMP };
	// �ൿ ���¸ӽ�
	enum class STATE_ACT { IDLE, STAND_OFF, SUSPICIOUS, DETECT, WALK, RUN, INCH, JUMP, 
						  PRE_ATTACK, ATTACK, HEAVY, PARRYING, FALLING, LANDING, DEAD };
	// �ൿŰ
	enum class ACTION_KEY { LEFT, RIGHT, UP, DOWN };

private:
	STATE_SET<STATE_OBJ, void(CMonster*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CMonster*, float)> m_tState_Act; // �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey; //���� ����Ű 

#pragma region AI : �Ǵ��ϴ¾� 

	void Obj_Idle(float fDeltaTime); // idle <-> suspicious
	void Obj_Suspicious(float fDeltaTime); // idle <-> sus <-> detect
	void Obj_Chase(float fDeltaTime);
	void Obj_Attack(float fDeltaTime);

#pragma endregion

#pragma region �ൿ : AI ���� �Ѿ�°� 
	void Idle(float fDeltaTime);
	void Stand_Off(float fDeltaTime);
	void Suspicious(float fDeltaTime);
	void Detect(float fDeltaTime);

	void Walk(float fDeltaTime);
	void Run(float fDeltaTime);
	void Inch(float fDeltaTime);
	void Jump(float fDeltaTime);
	
	void Prepare_Atk(float fDeltaTime);
	void Attack(float fDeltaTime);
	void Heavy_Attack(float fDeltaTime);
	void Parrying(float fDeltaTime);
	void Falling(float fDeltaTime);
	
	void Dead(float fDeltaTime);
#pragma endregion
};

