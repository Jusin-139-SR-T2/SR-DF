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

class CBoss : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CBoss)

private:
	explicit CBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBoss(const CBoss& rhs);
	virtual ~CBoss();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
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
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

	// ���¸ӽ� ���� --------------------------------------------------
private:
private:
	// �Լ� ----------
	_bool		Detect_Player();					// ���� �þ߰����� �÷��̾ �ִ��� üũ
	_float		Calc_Distance();					// ���Ϳ� �÷��̾� ������ �Ÿ� üũ�ϴ� �Լ� 
	void		FaceTurn(const _float& fTimeDelta); // �÷��̾������� ���ϴ� �Լ� 

	// ���� ----------
	_float		m_fCheck = 0;						//Taunt �� ������ ������Ƚ�� ����
	_int		m_iHP;								// ���� ���� hp 
	_int		m_iPreHP;							// ���� HP ����뵵 
	_int		m_iDazedHP = 25;					// ���� �����ϴ� hp
	_int		m_iMaxHP = 100;						// ���� �ִ� hp 
	_int		m_iAttack = 15;						// ���� ���ݷ�

	_float		m_fFrame = 0.f;						// �̹��� ���������� �����Ӻ��� 
	_float		m_fFrameEnd;						// �̹������� ���� �־������ 
	_float		m_fFrameSpeed;						// ������ ������ �ӵ�

	_float		m_fAwareness = 0;					// �ǽɰ����� ���� 
	_float		m_fMaxAwareness = 10.f;				// �ǽɰ����� max -> �߰����� ���� 
	_float		m_fConsider = 10.f;					// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 
	_float		m_fMaxConsider = 10.f;				// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 

	// �ӵ����� 
	_float		m_fRunSpeed = 2.0f;					// �پ���� �ӵ�
	_float		m_fWalkSpeed = 1.0f;				// �ȴ¼ӵ�
	_float		m_fKeepEyeSpeed = 2.f;				// ������ �����ϴ�  �ӵ� 
	_float		m_fSideWalkSpeed = 3.f;				// ������ �����ϴ�  �ӵ� 

	_float		m_fUprightSpeed = 2.5f;				// ��������� �޷����� �ӵ� 
	_float		m_fThrowSpeed = 4.f;				// ����ü ���ǵ� 

	_float		m_fBasicAttackSpeed = 3.f;			// �Ϲݰ��ݶ� �پ���� �ӵ� 
	_float		m_fHeavAttackSpeed = 4.f;			// �����ݶ� �پ���� �ӵ� 

	// ��Ÿ� , �þ߰�
	_float		m_fMonsterFov = 90;					//�þ߰� - �ݰ� ����
	_float		m_fMonsterSightDistance = 13.f;		// ���Ͱ� �����ϴ� ��Ÿ� 
	_float		m_fRunDistance = 8.f;				// ��Ÿ� ~ Run ���� =  run
	_float		m_fWalkDistance = 7.5f;				// run~walk ���� = walk
	_float		m_fEyesOnYouDistance = 6.f;			// Eyes ~ Walk = ������ ���� 
	_float		m_fCloseToYouDistance = 3.f;		// Close ~ Eyes = 

	// ��ġ ���� 
	_vec3		vPlayerPos;							// �÷��̾� ��ġ ����
	_vec3		vDir;								// ���Ͱ� �÷��̾� �ٶ󺸴� ����  

	//����ġ on/off 

public:
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ { 
		IDLE, SUSPICIOUS, TEST };

	// �ൿ ���¸ӽ�
	enum class STATE_ACT { IDLE, APPROACH };

	// �ൿŰ
	enum class ACTION_KEY { IDLE };

private:
	STATE_SET<STATE_OBJ, void(CBoss*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CBoss*, float)> m_tState_Act; // �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey; //���� ����Ű 

#pragma region AI 

	void AI_Idle(float fDeltaTime); // idle <-> suspicious
	void AI_Suspicious(float fDeltaTime); // idle <-> sus <-> detect
	//void AI_Taunt(float fDeltaTime);
	//void AI_Chase(float fDeltaTime);

	//void AI_Run(float fDeltaTime);
	//void AI_Walk(float fDeltaTime);
	//void AI_InchForward(float fDeltaTime);
	//void AI_Strafing(float fDeltaTime);
	//void AI_BasicAttack(float fDeltaTime);
	//void AI_HeavyAttack(float fDeltaTime);

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

