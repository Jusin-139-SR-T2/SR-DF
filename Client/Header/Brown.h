#pragma once

#include "AceMonster.h"

#include "BlackBoard_Monster.h"
#include "BlackBoardPtr.h"

#include "MonsterPunch.h"
#include "Awareness.h"

//������ 
#include "RedLaser.h"
#include "FallingStone.h"
#include "SlowThunder.h"
#include "EnergyBall.h"
#include "SpawnFire.h"
#include "BlueBuff.h"
#include "RedThunder.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CBrown : public CAceMonster
{
	DERIVED_CLASS(CAceMonster, CBrown)

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
	HRESULT				Add_Component();
	virtual void		Free();

	// Get, Set �Լ� ����� 
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

	//������
private:
	void	Update_InternalData();

protected:  
	FBlackBoardPtr<CBlackBoard_Monster>		m_wpBlackBoard_Monster; // ������ ���� 
	FBlackBoardPtr<CBlackBoard_Player>		m_wpBlackBoard_Player;	// ������ �÷��̾�

private:
	// �Ÿ�
	_float		m_fRunDistance = 6.f;				// ��Ÿ� ~ Run ���� =  run
	_float		m_fWalkDistance = 5.5f;				// run~walk ���� = walk
	_float		m_fInchDistance = 1.5f;  

	// ��ġ ���� 
	_vec3		vDir;								// ���Ͱ� �÷��̾� �ٶ󺸴� ����  
	
	//����ġ on/off 
	_bool		m_bArrive = false;
	_bool		m_AttackOnce = false;

#pragma region ���¸ӽ� enum����
	
public: 
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, TAUNT, REST, CHASE, 		// ���	 
		RUN, WALK, STRAFING, INCHFORWARD, 			// �߰�
		NORMALATTACK, HEAVYATTACK, 					// ����
		HIT, FACEPUNCH, CROTCHHIT,					// �ǰ�
		DAZED, FALLING, CHOPPED, DEATH, HEADLESS,	// ����
		RECONNAISSANCE, GOHOME,						// ����

		//�̱��� ����Ʈ 
		JUMP
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

#pragma endregion

#pragma region AI 

	// ��� 
	void AI_Idle(float fDeltaTime); // SOUTH �ٶ󺸴��� 
	void AI_Suspicious(float fDeltaTime); // �� ? �ǽ���
	void AI_Taunt(float fDeltaTime); // �ٰ��ͺ� �ϰ� ���߻��� 
	void AI_Chase(float fDeltaTime); // �Ÿ� üũ�ؼ� ���� ���� ���������� ������ ��꿪�� 
	void AI_Rest(float fDeltaTime); // ���ڸ����� ���Ÿ�

	// �߰� 
	void AI_Run(float fDeltaTime); // �޸��� 
	void AI_Walk(float fDeltaTime); // �ȱ�
	void AI_Strafing(float fDeltaTime); // �¿�� ���鼭 ��, �� ���� �ϳ��� �̵� 
	void AI_InchForward(float fDeltaTime); // ������ �� �ٰ����� ���� �ڼ� 
	
	// ����
	void AI_HeavyAttack(float fDeltaTime); // ������ 
	void AI_NormalATTACK(float fDeltaTime); // �Ϲ� ����
	
	// �ǰ�
	void AI_Hit(float fDeltaTime); // ���� ��Ʈ���� 
	void AI_Falling(float fDeltaTime); // ������ �¾������ 
	void AI_FacePunch(float fDeltaTime); // �󱼿� �¾������ 
	void AI_CrotchHit(float fDeltaTime); // �ϴ� �¾������  

	// ���� 
	void AI_Dazed(float fDeltaTime); // hp �����̻� ���� ���� 
	void AI_Death(float fDeltaTime); //ok // hp 0�λ��� 
	void AI_Chopped(float fDeltaTime); // �ڿ��� �÷��̾ �������������� Sleep���� �� 
	void AI_Headless(float fDeltaTime); //ok // �ѷ��� �Ӹ��� �¾������ 

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

*/

/*
����� ���� 

OutputDebugString(L"�� : �浹 ���� ����� 
OutputDebugString(L"�� : ���¸ӽ� ���� ����� 

swprintf_s(debugString, L"Brown - ���� Ȯ�� m_fAwareness = %f\n", m_fAwareness);
OutputDebugStringW(debugString);

*/