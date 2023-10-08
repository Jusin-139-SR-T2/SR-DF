#pragma once

#include "UI.h"
#include "Engine_Macro.h"

#include "BlackBoard_Player.h"
#include "BlackBoardPtr.h"
#include "BlackBoardMgr.h"

#include "Export_System.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

/// <summary>
/// 플레이어의 정보를 표시하는 UI 클래스
/// </summary>
class CUI_Player : public CUI
{
	DERIVED_CLASS(CUI, CUI_Player)
protected:
	explicit CUI_Player(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI_Player(const CUI_Player& rhs);
	virtual ~CUI_Player();

public:
	static CUI_Player* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private: 
	virtual void Free();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject() override;
	virtual void Render_GameObject() override;

//---------------------------------------------------------------------------------------------

private:
	HRESULT Add_Component();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)

private:
	CRcBufferComp*			m_pBufferComp = nullptr;
	CTransformComponent*	m_pTransformComp = nullptr;
	CTextureComponent*		m_pTextureComp = nullptr;


//---------------------------------------------------------------------------------------------

private:
	void	Update_InternalData();

protected:
	FBlackBoardPtr<CBlackBoard_Player>	m_wpBlackBoard_Player;	// 블랙보드 플레이어

private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_matrix				m_ViewMatrix, m_ProjMatrix;
	_float				m_fHp;
};

