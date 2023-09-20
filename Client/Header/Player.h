#pragma once

#include "GameObject.h"

#include "Engine_Macro.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCameraComponent;
class CCalculatorComponent;
class CSphereColComp;

END

class CDynamicCamera;

class CPlayer : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CPlayer)
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

public:
	static CPlayer*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	GETSET_EX2(CDynamicCamera*, m_pCamera, Camera, GET, SET)

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pLeftHandTextureComp, LeftHandTextureComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pRightHandTextureComp, RightHandTextureComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(CSphereColComp*, m_pColliderComp, SphereColComponent, GET, SET)


public:// 플레이어 상태 값
	// 플레이어
	enum class STATE_PLAYER { NONE, IDLE, MOVE, RUN, DOWN, ATTACK, DIE };
	// 왼손
	enum class STATE_LEFTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, RIGHTER, RUN_RIHGTER, STEELPIPE };
	// 오른손
	enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE };
	
// TEST
	enum class OBJECT_TYPE { NONE, GUN, THOMPSON, STEELPIPE  }; // 테스트(오브젝트)
// TEST
private: // 플레이어의 상태 머신
	STATE_SET<STATE_PLAYER, void(CPlayer*, float)> m_tPlayer_State;

	void Idle(float fTimeDelta);
	void Move(float fTimeDelta);
	void Run(float fTimeDelta);
	void Down(float fTimeDelta);
	void Attack(float fTimeDelta);
	void Die(float fTimeDelta);

private: // 플레이어의 왼손 상태 머신
	STATE_SET<STATE_LEFTHAND, void(CPlayer*, float)> m_tLeftHand_State;

	void	Left_Hand(float fTimeDelta);
	void	Left_Gun(float fTimeDelta);
	void	Left_Thompson(float fTimeDelta);
	void	Left_Steelpipe(float fTimeDelta);
	void	Left_Righter(float fTimeDelta);

private: // 플레이어의 오른손 상태 머신
	STATE_SET<STATE_RIGHTHAND, void(CPlayer*, float)> m_tRightHand_State;

	void	Right_Hand(float fTimeDelta);
	void	Right_Gun(float fTimeDelta);
	void	Right_Thompson(float fTimeDelta);
	void	Right_Steelpipe(float fTimeDelta);

private:
	HRESULT				Add_Component();							// 컴포넌트 추가
	bool				Keyboard_Input(const _float& fTimeDelta);	// 키보드 입력
	bool				Attack_Input(const _float& fTimeDelta);		// 공격 입력(마우스)
	void				Mouse_Move();								// 마우스 움직임
	void				Height_On_Terrain();						// 지형타기
	void				Hand_Check();								// 플레이어 손 상태 체크

private:
	void State_Update(float fTimeDelta);	// 상태 업데이트(상태 체크)

private:
	CRcBufferComp*				m_pBufferComp = nullptr;			// Rc버퍼
	CTransformComponent*		m_pTransformComp = nullptr;			// 이동행렬 버퍼

	// 플레이어 손
	CTextureComponent*			m_pLeftHandComp = nullptr;			// 왼손 텍스처(담을곳)
	CTextureComponent*			m_pRightHandComp = nullptr;			// 오른손 텍스처(담을곳)


	CTextureComponent*			m_pAttackTextureComp = nullptr;		// 공격(총) 텍스처
	CTextureComponent*			m_pAttackSpinTextureComp = nullptr;	// 총 회전 텍스처
	CCalculatorComponent*		m_pCalculatorComp = nullptr;		// 지형 타기 컴포넌트
	CSphereColComp*				m_pColliderComp = nullptr;			// 구 충돌 콜라이더

	// Test
	CTextureComponent*			m_pLeftHandTextureComp = nullptr;			// 왼손(이미지) 텍스처
	CTextureComponent*			m_pRightHandTextureComp = nullptr;			// 오른손(이미지) 텍스처
	CTextureComponent*			m_pRightSteelPipeTextureComp = nullptr;		// 쇠파이프(이미지) 텍스처
	CTextureComponent*			m_pLeftZippoTextureComp = nullptr;			// 라이터(이미지) 텍스처

private:
	_bool		bAttackOn = false;		// 공격
	_bool		bLeftFrameOn = false;
	_bool		bRightFrameOn = false;
	_bool		bGunOn = false;
	_bool		bSpinOn = false;
	_bool		bDead = false;
	_bool		bMove = false;
	_bool		bMouse_Button = false;
	_bool		bFootAttack = false;
	_bool		bRighter = false;
	_bool		bRunOn = false;

	_float		fSpeed = 5.f;

	// Test
	OBJECT_TYPE m_eObjectType;

private:
	_long			dwMouseMove = 0;
	_float			m_fLeftFrame = 0.f;
	_float			m_fRightFrame = 0.f;
	_float			m_fMaxFrame = 0.f;

private:
	virtual void		Free();

protected:
	CDynamicCamera*		m_pCamera = nullptr;

protected:
	_vec3		m_vEye, m_vAt, m_vUp;
	_matrix		m_matRot;


	//빛
private:
	//HRESULT			SetUp_Material();
	_bool			bTorch = false;

};