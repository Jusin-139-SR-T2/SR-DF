
#pragma once
#include "GameObject.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

#include "ThrowPipe.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CGray : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CGray)

private:
	explicit CGray(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGray(const CGray& rhs);
	virtual ~CGray();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CGray* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);

private:
	CRcBufferComp*			m_pBufferComp = nullptr;
	CTextureComponent*		m_pTextureComp = nullptr;
	CColliderComponent*		m_pColliderComp = nullptr;
	CTransformComponent*	m_pTransformComp = nullptr;
	CCalculatorComponent*	m_pCalculatorComp = nullptr;
	CTransformComponent*	m_pPlayerTransformcomp = nullptr;

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();

	// Get, Set �Լ� ����� ---------------------------------------------------------------
public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

	// �浹
protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);

	// ���¸ӽ� ���� --------------------------------------------------
private:
	// �Լ� ----------					
	_bool		Detect_Player();					// ���� �þ߰����� �÷��̾ �ִ��� üũ
	_float		Calc_Distance();					// ���Ϳ� �÷��̾� ������ �Ÿ� üũ�ϴ� �Լ� 
	void		Billboard(const _float& fTimeDelta); // �÷��̾������� ���ϴ� �Լ� 
	HRESULT     Get_PlayerPos(const _float& fTimeDelta); // �÷��̾� dynamic_cast�뵵 
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

	// ���� �ν� ���� 
	_float		m_fAwareness = 0;					// �ǽɰ����� ���� 
	_float		m_fMaxAwareness = 8.f;				// �ǽɰ����� max -> �߰����� ���� 
	_float		m_fConsider = 10.f;					// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 
	_float		m_fMaxConsider = 10.f;				// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 

	// �ӵ����� 
	_float		m_fRunSpeed = 2.0f;					// �پ���� �ӵ�
	_float		m_fWalkSpeed = 1.0f;				// �ȴ¼ӵ�
	_float		m_fKeepEyeSpeed = 2.f;				// ������ �����ϴ�  �ӵ� 
	_float		m_fSideWalkSpeed = 3.f;				// ������ �����ϴ�  �ӵ� 

	_float		m_fUprightSpeed = 2.5f;				// ��������� �޷����� �ӵ� 
	_float		m_fThrowSpeed = 4.f;				// ����ü ���ǵ� 

	_float		m_fBasiCPlayerLighterSpeed = 3.f;			// �Ϲݰ��ݶ� �پ���� �ӵ� 
	_float		m_fHeavAttackSpeed = 4.f;			// �����ݶ� �پ���� �ӵ� 

	// ��Ÿ� , �þ߰�
	_float		m_fMonsterFov = 80;					//�þ߰� - �ݰ� ����
	_float		m_fMonsterSightDistance = 13.f;		// ���Ͱ� �����ϴ� ��Ÿ� 
	_float		m_fRunDistance = 8.f;				// ��Ÿ� ~ Run ���� =  run
	_float		m_fWalkDistance = 7.5f;				// run~walk ���� = walk
	_float		m_fEyesOnYouDistance = 6.f;			// Eyes ~ Walk = ������ ���� 
	_float		m_fCloseToYouDistance = 2.f;		// Close ~ Eyes = 

	// ��ġ ���� 
	_vec3		vPlayerPos;							// �÷��̾� ��ġ ����
	_vec3		vDir;								// ���Ͱ� �÷��̾� �ٶ󺸴� ����  
	_vec3		vPatrolPointZero;
	
	//����ġ on/off 
	_bool		m_bGoHome = false;

public:
	// ��ǥ ���¸ӽ�(AI)
	enum class STATE_OBJ { 
		IDLE,		SUSPICIOUS,     REST,		CHASE,	  RECONNAISSANCE,
		YOUDIE,		TAUNT,			SIDEWALK,   KEEPEYE,  RUN,				 WALK, 
		THROW,		ATTACK,			UPRIGHTRUN, FRIGHTEN, HEAVYATTACK,		 BLOCK,
		CROTCHHIT,  FACEPUNCH,		FALLING,	DAZED,	  CHOPPED,
		HEADSHOT,	HEADLESS,		DEATH, 
		GOHOME};

	// �ൿ ���¸ӽ�
	enum class STATE_ACT { IDLE, APPROACH, SUDDENATTACK, SIDEMOVING, ATTACK,
						   GOHOME};

	// �ൿŰ
	enum class ACTION_KEY {
		IDLE, RUN, WALK, KEEPEYE, SIDEWALK, 
		UPRIGHT, FRIGHTEN, BASIC_ATTACK, HEAVY_ATTACK, 
		GOHOME
	};

private:
	STATE_SET<STATE_OBJ, void(CGray*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CGray*, float)> m_tState_Act;				// �ൿ
	ACTION_SET<ACTION_KEY> m_mapActionKey;								//���� ����Ű 

#pragma region AI 

	void AI_Idle(float fDeltaTime); // ó�� ���ִ� �뵵 

	void AI_Suspicious(float fDeltaTime); // ������ �߰��� 
	void AI_Taunt(float fDeltaTime); //����
	void AI_YouDie(float fDeltaTime); //����
	void AI_Chase(float fDeltaTime); //  �Ÿ��� ������ ����
	void AI_Rest(float fDeltaTime); // idle ready���� - �߰��߰� �ֱ�� 

	void AI_Run(float fDeltaTime); //������ �޷�����
	void AI_Walk(float fDeltaTime); //�Ϲ����� �ȱ� 
	void AI_KeepEye(float fDeltaTime); //�÷��̾� �ֽ���ä�� �齺�ǹ�� ������ 
	void AI_SideWalk(float fDeltaTime); //�÷��̾� �ֽ���ä�� ������ ���� - ���� ������ walk 
	void AI_UpRightRun(float fDeltaTime); //������ ��� �����Ϸ� �޷����»���
	void AI_Frighten(float fDeltaTime); //�����ؼ� ���� - �����ϴ°� ���⿡  ������ �ٷ� ��Ÿ�Ǿ���ҵ�

	void AI_Throw(float fDeltaTime); //������ ���� 
	void AI_Attack(float fDeltaTime); // ������ ��� �밢������ �� �����°�
	void AI_HeavyAttack(float fDeltaTime); //������ - ������ ���Ž�

	void AI_Reconnaissance(float fDeltaTime); // �÷��̾� ���ļ� �ֺ� �����ϴ��� 
	void AI_GoHome(float fDeltaTime); // ������ ��� �밢������ �� �����°�
	//void AI_Block(float fDeltaTime); //�÷��̾ �����ϸ� ������

	//void AI_CrotchHit( float fDeltaTime); // �ɱ� + �ǰ�
	//void AI_FacePunch(float fDeltaTime); //�� �ǰ�
	//void AI_Falling(float fDeltaTime); //�������� �Ͼ�°�
	//void AI_Dazed(float fDeltaTime); //������ ���ϰ� �Ǹ� �����»���
	//void AI_Chopped(float fDeltaTime); //�ڵ��� ������ �ճ�ġ�� -> sleep 
	//void AI_HeadShot(float fDeltaTime); // �ָ��̳� ��Ÿ ����� ��弦
	//void AI_Headless(float fDeltaTime); //������ ������
	//void AI_Death(float fDeltaTime); //�Ϲݰ������� �״»��� 

#pragma endregion

#pragma region �ൿ : AI ���� �Ѿ�°� 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);
	void SuddenAttack(float fDeltaTime);
	void SideMoving(float fDeltaTime);
	void Attack(float fDeltaTime);
	void GoHome(float fDeltaTime);

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

