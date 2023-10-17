#pragma once

#include "BlackBoard.h"

class CBrown;
class CGray;
class CBoss;

/// <summary>
/// 이니셜라이즈용 구조체
/// </summary>

struct FBlackBoard_MonsterInit
{
	CBrown* pBrown;
	CGray* pGray;
	CBoss* pBoss;
};


/// <summary>
/// 플레이어 전용 블랙보드
/// 플레이어에게 필요한 정보라면 여기에 업데이트 하자
/// </summary>
class CBlackBoard_Monster : public CBlackBoard
{
	DERIVED_CLASS(CBlackBoard, CBlackBoard_Monster)

protected:
	explicit CBlackBoard_Monster();
	explicit CBlackBoard_Monster(const CBlackBoard_Monster& rhs);
	virtual ~CBlackBoard_Monster();

public:
	static CBlackBoard_Monster* Create();

private:
	virtual void		Free();

protected:
	virtual HRESULT Ready_BlackBoard() override { return S_OK; }
	// 초기화용 구조체를 만들어 매개변수로 받게 한다.
	virtual HRESULT Ready_BlackBoard(const FBlackBoard_MonsterInit& tInit);

public:		// 데이터 가공의 성격을 띌때는 해당 함수를 직접 만들것.
	GETSET_EX2(_bool, m_bLightOn, ControlLight, GET, SET)


private:
	_bool				m_bLightOn;
	_bool				m_bDeadState;

};

