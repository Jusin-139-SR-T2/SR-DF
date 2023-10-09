#pragma once

#include "BlackBoard.h"

class CPlayer;

/// <summary>
/// 이니셜라이즈용 구조체
/// </summary>
struct FBlackBoard_PlayerInit
{
	CPlayer* pPlayer;
};


/// <summary>
/// 플레이어 전용 블랙보드
/// 플레이어에게 필요한 정보라면 여기에 업데이트 하자
/// </summary>
class CBlackBoard_Player : public CBlackBoard
{
	DERIVED_CLASS(CBlackBoard, CBlackBoard_Player)
	enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };

protected:
	explicit CBlackBoard_Player();
	explicit CBlackBoard_Player(const CBlackBoard_Player& rhs);
	virtual ~CBlackBoard_Player();

public:
	static CBlackBoard_Player* Create();

private:
	virtual void		Free();

protected:
	virtual HRESULT Ready_BlackBoard() override { return S_OK; }
	// 초기화용 구조체를 만들어 매개변수로 받게 한다.
	virtual HRESULT Ready_BlackBoard(const FBlackBoard_PlayerInit& tInit);

public:		// 플레이어에 대한 정보만 얻을 때는 
	GETSET_EX1(CPlayer*, m_pPlayer, Player, GET)

private:
	CPlayer*			m_pPlayer = nullptr;

public:		// 데이터 가공의 성격을 띌때는 해당 함수를 직접 만들것.
	GETSET_EX2(GAUGE<_float>, m_fHP, HP, GET_C_REF, GET_REF)
	GETSET_EX2(GAUGE<_float>, m_fStamina, Stamina, GET_C_REF, GET_REF)

private:
	GAUGE<_float>			m_fHP;
	GAUGE<_float>			m_fStamina;
	//CPlayer::STATE_RIGHTHAND m_ePlayer;
	//CPlayer::TATE_RIGHTHAND m_ePlayerRightHand;
		
};

