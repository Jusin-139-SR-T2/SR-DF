#pragma once

#include "BlackBoard.h"

class CBrown;
class CGray;
class CBoss;

/// <summary>
/// �̴ϼȶ������ ����ü
/// </summary>

struct FBlackBoard_MonsterInit
{
	CBrown* pBrown;
	CGray* pGray;
	CBoss* pBoss;
};


/// <summary>
/// �÷��̾� ���� ������
/// �÷��̾�� �ʿ��� ������� ���⿡ ������Ʈ ����
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
	// �ʱ�ȭ�� ����ü�� ����� �Ű������� �ް� �Ѵ�.
	virtual HRESULT Ready_BlackBoard(const FBlackBoard_MonsterInit& tInit);

public:		// ������ ������ ������ �鶧�� �ش� �Լ��� ���� �����.
	GETSET_EX2(_bool, m_bLightOn, ControlLight, GET, SET)


private:
	_bool				m_bLightOn;
	_bool				m_bDeadState;

};

