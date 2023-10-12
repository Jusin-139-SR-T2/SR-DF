#pragma once
#include "AceUnit.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"
#include "Player.h"

// Team Agent 추가예정  

typedef struct tag_Monster_Info
{
	//m_tStat
	_float		fAttackDistance;					// 몬스터 시야사거리
	_float		fAttackFov;							// 몬스터 시야 사이 반각 (90 = 시야각도 180도)

	_float		fConsider = 0.f;					// 플레이어 놓친뒤에 주변정찰 게이지 (감소예정)
	_float		fMaxConsider = 10.f;				// 플레이어 놓친뒤에 주변정찰 게이지 

	_float		fAwareness = 0;						// 의심게이지 숫자 
	_float		fMaxAwareness = 15.f;				// 의심게이지 max -> 추격으로 변함 

	_int		iDazedHP = 25;						// 몬스터 기절하는 hp

	_vec3		vPatrolPointZero;					//초기 시작포인트 
}MONSTER;

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CColliderComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CPlayer;

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
	CPlayer::STATE_RIGHTHAND	ePlayerRighthand;
	CPlayer::STATE_PLAYER		ePlayerState;
	MONSTER						m_tStat;
	FRAME						m_tFrame;
	_vec3						vPlayerPos;			
	_tchar						debugString[100];

protected:
	CRcBufferComp*				m_pBufferComp = nullptr; 
	CTextureComponent*			m_pTextureComp = nullptr;
	CColliderComponent*			m_pColliderComp = nullptr;
	CTransformComponent*		m_pTransformComp = nullptr;
	CCalculatorComponent*		m_pCalculatorComp = nullptr;
	CTransformComponent*		m_pPlayerTransformcomp = nullptr;

public:
	_float						Calc_Distance();
	_bool						Detect_Player();
	HRESULT						Get_PlayerPos();
	void						Height_On_Terrain();
	void						Billboard(const _float& fTimeDelta); 

	//제작함수 리스트 - 스킬 셋팅할때 사용중 
public:
	//low~high 사이의 값으로 return 
	_float Get_RandomFloat(_float lowBound, _float hightBound)
	{
		if (lowBound >= hightBound)
			return lowBound;

		_float f = (rand() % 10000) * 0.0001f;

		return (f * (hightBound - lowBound)) + lowBound;
	}

	// Get_RandomFloat의 vector형 
	void Get_RandomVector(_vec3* out, _vec3* min, _vec3* max)
	{
		out->x = Get_RandomFloat(min->x, max->x);
		out->y = Get_RandomFloat(min->y, max->y);
		out->z = Get_RandomFloat(min->z, max->z);
	}

	// center기준 radius 내의 "랜덤위치" out에 저장 
	void GetRandomPointInCircle(_vec3* out, _vec3* center, float radius)
	{
		_float angle = static_cast<_float>(rand()) / RAND_MAX * 2 * D3DX_PI;
		_float r = static_cast<_float>(rand()) / RAND_MAX * radius;

		// 극좌표를 직교 좌표로 변환
		out->x = center->x + r * cosf(angle);
		out->y = center->y;
		out->z = center->z + r * sinf(angle);
	}

	// Center기준 radius만큼 떨어진위치에 원을 그리면서 위치하게 하는데 number갯수만큼 만든다. 
	// ex. number4, radius2일경우 반지름이2인 원을 그리고 각각 동서남북으로 위치하게된다. 
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
		//운영체제 단에서 제공하는 진짜 난수 - 컴퓨터가 주변의 환경과 무작위적으로 상호작용하면서 만드는것
		// 의사난수보다 난수를 생성하는 속도가 매우 느리다. 
		random_device rd;

		// 메르센 트위스터라는 알고리즘을 사용해서 난수 생성 엔진을 초기화
		mt19937 gen(rd());

		// 0 부터 99 까지 연속확률분포 생성   (min, max)
		uniform_int_distribution<_int> iDistribution(0, 99);

		//랜덤값 생성 
		_int RandomValue = iDistribution(gen);

		//매개변수가 5일경우 5프로 확률로 true가 반환된다
		return (RandomValue <= _winning);
	}

};