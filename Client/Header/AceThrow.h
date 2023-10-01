#pragma once

#include "AceObjectFactory.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;

END


class CAceThrow : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CAceThrow)

		enum class THROW_NAME {
		// Paper
		PaperBox, SCRUNCHEDPAPER,FEDORA, ROPE, SACK, HANDBAG, CHAIR,

		// Wood
		BARSTOOL, CAFESIGN, CAFETABLE, COATHANGER, TOOLBOX, UMBRELLASTAND, WOODENBARREL,
		WOODENSTOOL, LAMP, MATCHES, BOARD, MANNEQUIN, CABLEREEL,

		// Glass
		BEERGLASS, BEERMUG, WhiskeyGlass, POTPLANT, TABLEPLANT, MARTINGLASS, PERFUME, GENERICBOTTLE,
		
		// Steel
		BUCKET, TRAFFICCORN,TRASHBIN, TRASHCAN, MICROPHONE, CHAMPAGNEBUCKET, OILBARREL, 
		TRAFFICBOLLARD, PAINTCAN, METALBARREL, 

		THROW_END };

private:
	explicit CAceThrow(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceThrow(const CAceThrow& rhs);
	virtual ~CAceThrow();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	static CAceThrow*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz);

private: // 함수 
	virtual void		Free();
	HRESULT				Add_Component();
	HRESULT				BillBoard(const _float& fTimeDelta); // 플레이어쪽으로 향하는 함수 
	void				Height_On_Terrain();
	void				ThrowName(const _tchar* pObjTag); //각각의 ENUM별 사이즈 정리해서 한번에 정리 
	void				Change_Texture(THROW_NAME eReceiveName);

private: // 변수 
	THROW_NAME			m_pCurName;
	_float m_fHp = 2.f;
	_float m_fBrokenHp = 0.f;
	_float m_fCrackedHp = 1.f;
	_float m_fMaxHP = 2.f;

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
};
