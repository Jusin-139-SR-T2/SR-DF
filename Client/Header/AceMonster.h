#pragma once
#include "AceUnit.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"
#include "Player.h"

#include "Effect_HitBlood.h"
#include "Effect_HitPow.h"
#include "Effect_HitDust.h"
#include "Effect_FallingDust.h"
#include "Effect_DazeSwirl.h"

// Team Agent �߰�����  

typedef struct tag_Monster_Info
{
	//m_tStat
	_float		fAttackDistance;					// ���� �þ߻�Ÿ�
	_float		fAttackFov;							// ���� �þ� ���� �ݰ� (90 = �þ߰��� 180��)

	_float		fConsider = 0.f;					// �÷��̾� ��ģ�ڿ� �ֺ����� ������ (���ҿ���)
	_float		fMaxConsider = 10.f;				// �÷��̾� ��ģ�ڿ� �ֺ����� ������ 

	_float		fAwareness = 0;						// �ǽɰ����� ���� 
	_float		fMaxAwareness = 15.f;				// �ǽɰ����� max -> �߰����� ���� 

	_int		iDazedHP = 25;						// ���� �����ϴ� hp

	_vec3		vPatrolPointZero;					//�ʱ� ��������Ʈ 
}MONSTER;

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CColliderComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CAceMonster : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CAceMonster)

protected:
	explicit CAceMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceMonster(const CAceMonster& rhs);
	virtual ~CAceMonster();

protected:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() PURE;
	virtual void		Free() override;

protected:
	STATE_RIGHTHAND				ePlayerRighthand;
	MONSTER						m_tStat;
	FRAME						m_tFrame;
	_vec3						vPlayerPos;			
	_tchar						debugString[100];
	_bool						m_bDeadState; // ��������
	_bool						m_bDazedState; // ���ϴ� ���߿� ����°� �뵵 
	_bool						m_bDazeToHeal; // ����� daze���̻� ����x
	FCollisionBox*				pBoxShape;

protected:
	CRcBufferComp*				m_pBufferComp = nullptr; 
	CTextureComponent*			m_pTextureComp = nullptr;
	CColliderComponent*			m_pColliderComp = nullptr;
	CTransformComponent*		m_pTransformComp = nullptr;
	CCalculatorComponent*		m_pCalculatorComp = nullptr;
	CTransformComponent*		m_pPlayerTransformcomp = nullptr;

public: // ���� �߰� : ���� ���� Get,Set
	PLAYER_ATTACK_STATE m_ePlayer_AttackState;
	GETSET_EX2(GAUGE<_float>, m_gHp, MonsterHP, GET, SET)   // ���� HP 
	GETSET_EX2(PLAYER_ATTACK_STATE, m_ePlayer_AttackState, Player_AttackState, GET, SET)   // �ǰݴ��� ������ ���� (ex : ����ä�� ����, ���� ����, 2���� ����...���)
	
	GETSET_EX2(_bool, m_bDazedState, IsMonsterDazed, GET, SET)   
	GETSET_EX2(_bool, m_bDeadState, IsMonsterDeath, GET, SET)   
	GETSET_EX2(_float, m_tStat.fAwareness, Awareness, GET, SET)
	GETSET_EX2(_vec3, m_vPos, MonsterPos, GET, SET)
		
public:
	_float						Calc_Distance();
	_bool						Detect_Player();
	HRESULT						Get_PlayerPos();
	void						Height_On_Terrain();
	void						Billboard(const _float& fTimeDelta); 
	_bool						m_bPlayerAttakBool;
	_vec3						m_vPos;
	void						Add_BasicEffect(CGameObject* pOwner);
	//�����Լ� ����Ʈ - ��ų �����Ҷ� ����� 
public:
	//low~high ������ ������ return 
	_float Get_RandomFloat(_float lowBound, _float hightBound)
	{
		if (lowBound >= hightBound)
			return lowBound;

		_float f = (rand() % 10000) * 0.0001f;

		return (f * (hightBound - lowBound)) + lowBound;
	}

	// Get_RandomFloat�� vector�� 
	void Get_RandomVector(_vec3* out, _vec3* min, _vec3* max)
	{
		out->x = Get_RandomFloat(min->x, max->x);
		out->y = Get_RandomFloat(min->y, max->y);
		out->z = Get_RandomFloat(min->z, max->z);
	}

	// center���� radius ���� "������ġ" out�� ���� 
	// radius�� ��� ����������. 
	void GetRandomPointInCircle(_vec3* out, _vec3* center, float radius)
	{
		_float angle = static_cast<_float>(rand()) / RAND_MAX * 2 * D3DX_PI;
		_float r = static_cast<_float>(rand()) / RAND_MAX * radius;

		// ����ǥ�� ���� ��ǥ�� ��ȯ
		out->x = center->x + r * cosf(angle);
		out->y = center->y;
		out->z = center->z + r * sinf(angle);
	}

	// Center���� radius��ŭ ��������ġ�� ���� �׸��鼭 ��ġ�ϰ� �ϴµ� number������ŭ �����. 
	// ex. number4, radius2�ϰ�� ��������2�� ���� �׸��� ���� ������������ ��ġ�ϰԵȴ�. 
	void GetCirclePos(_vec3* Out, _vec3* Center, _int _number, _float _radius)
	{
	    for (_int i = 0; i < _number; ++i)
	    {
	        _float angle = static_cast<_float>(i) / static_cast<_float>(_number) * D3DX_PI * 2.0f;
	        Out->x = Center->x + _radius * cosf(angle);
	        Out->y = Center->y;
	        Out->z = Center->z + _radius * sinf(angle);
	
	    }
	}

	_bool Random_variable(_int _winning)
	{
		//�ü�� �ܿ��� �����ϴ� ��¥ ���� - ��ǻ�Ͱ� �ֺ��� ȯ��� ������������ ��ȣ�ۿ��ϸ鼭 ����°�
		// �ǻ糭������ ������ �����ϴ� �ӵ��� �ſ� ������. 
		random_device rd;

		// �޸��� Ʈ�����Ͷ�� �˰����� ����ؼ� ���� ���� ������ �ʱ�ȭ
		mt19937 gen(rd());

		// 0 ���� 99 ���� ����Ȯ������ ����   (min, max)
		uniform_int_distribution<_int> iDistribution(0, 99);

		//������ ���� 
		_int RandomValue = iDistribution(gen);

		//�Ű������� 5�ϰ�� 5���� Ȯ���� true�� ��ȯ�ȴ�
		return (RandomValue <= _winning);
	}

};