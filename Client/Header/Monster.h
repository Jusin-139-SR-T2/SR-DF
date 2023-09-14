#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CMonster : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CMonster)

	enum MONSTER_DIRECTION {
		EAST, 
		SOUTH_EAST, 
		SOUTH,
		SOUTH_WEST, 
		WEST,
		NORTH_WEST,
		NORTH,
		NORTH_EAST,
		DIRECTION_END };

   enum MONSTERSTATE {
		IDLE_READY,//걷기
		WALK, 
		RUN,
		
		BASIC_ATTACK, //공격상태
		HEAVY_ATTACK,
		DAZED,

		FACE_PUNCH,	//히트상태
		FALLING,
		GETUP,
		HIT,

		TAUNT,	//도발상태
		STRAFING,
		INCH,
		STANDOFF,
		POOING,		
		
		SHOCKED,	//죽은상태
		CHOPPED,
		DEAD,
		THROW_DEAD,
		HEADLESS,
		MONSTERSTATE_END };

   typedef struct MONSTER_INFO
   {
	   MONSTERSTATE state; //상태
	   MONSTER_DIRECTION dir; //방향에 따라 다른이미지라서 방향값 
	   // 상태 2개 더 추가해야함

	   //state = idle, 경계, 포착, 공격, die 로 구성
	   // 각 state에 맞는 하위 state가 있음 
	   float m_fCnt; // 해당하는 이미지의 총 갯수

   }MONSTER_INFO;

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
	CRcTex*			m_pBufferComp = nullptr;
	CTransform*		m_pTransformComp = nullptr;
	CTexture*		m_pTextureComp = nullptr;
	CCalculator*	m_pCalculatorComp = nullptr;
	CGameObject*	m_pTarget = nullptr;

private:
	HRESULT			Add_Component();
	virtual void	Free();

private:
	void				Height_On_Terrain();

public:
	void				Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }
	static CMonster*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	_matrix				m_matRotAxis;

//---------------------------------------------------------------
public:
	_float				m_fFrame = 0.f; // 이미지 돌리기위한 프레임변수 
	void				state_check(MONSTERSTATE _CurrState, MONSTER_DIRECTION _Direction);
	MONSTER_INFO		m_MonsterInfo;
	_float				m_fsuspicious;

	void				Monster_Eyerange();
	float  m_fEyetheta = D3DX_PI / 4.f; // 몬스터 시야각 

public: //Get, Set 함수 만들기 
	GETSET_EX2(CRcTex*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTexture*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransform*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculator*, m_pCalculatorComp, CalculatorComponent, GET, SET)

};

