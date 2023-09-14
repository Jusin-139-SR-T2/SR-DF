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
		IDLE_READY,//�ȱ�
		WALK, 
		RUN,
		
		BASIC_ATTACK, //���ݻ���
		HEAVY_ATTACK,
		DAZED,

		FACE_PUNCH,	//��Ʈ����
		FALLING,
		GETUP,
		HIT,

		TAUNT,	//���߻���
		STRAFING,
		INCH,
		STANDOFF,
		POOING,		
		
		SHOCKED,	//��������
		CHOPPED,
		DEAD,
		THROW_DEAD,
		HEADLESS,
		MONSTERSTATE_END };

   typedef struct MONSTER_INFO
   {
	   MONSTERSTATE state; //����
	   MONSTER_DIRECTION dir; //���⿡ ���� �ٸ��̹����� ���Ⱚ 
	   // ���� 2�� �� �߰��ؾ���

	   //state = idle, ���, ����, ����, die �� ����
	   // �� state�� �´� ���� state�� ���� 
	   float m_fCnt; // �ش��ϴ� �̹����� �� ����

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
	_float				m_fFrame = 0.f; // �̹��� ���������� �����Ӻ��� 
	void				state_check(MONSTERSTATE _CurrState, MONSTER_DIRECTION _Direction);
	MONSTER_INFO		m_MonsterInfo;
	_float				m_fsuspicious;

	void				Monster_Eyerange();
	float  m_fEyetheta = D3DX_PI / 4.f; // ���� �þ߰� 

public: //Get, Set �Լ� ����� 
	GETSET_EX2(CRcTex*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTexture*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransform*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculator*, m_pCalculatorComp, CalculatorComponent, GET, SET)

};

