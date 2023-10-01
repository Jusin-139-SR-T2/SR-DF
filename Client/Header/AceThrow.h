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
		// Paper (6) 
		PAPERBOX, SCRUNCHEDPAPER,FEDORA, ROPE, SACK, HANDBAG,

		// Wood (13)
		BARSTOOL, CAFESIGN, CAFETABLE, COATHANGER, TOOLBOX, UMBRELLASTAND, WOODENBARREL,
		WOODENSTOOL,  MATCHES, BOARD, MANNEQUIN, CABLEREEL, CHAIR,

		// Glass (10)
		BEERGLASS, BEERMUG, WHISKEYGlASS, POTPLANT, TABLEPLANTLEAF, TABLEPLANTFLOWER,
		MARTINIGLASS, PERFUME, GENERICBOTTLE, LAMP,
		// Steel (10)
		BUCKET, TRAFFICCONE, TRASHBIN, TRASHCAN, MICROPHONE, CHAMPAGNEBUCKET, OILBARREL, 
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
	static CAceThrow*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, 
		const _float _fx, const _float _fy, const _float _fz, const _float iCnt = 1.f);

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
	_bool m_bThrow = false; // BlackBoard에서 해당 bool값 가져가기 = 피격시 true로 바뀌고 hp 하나 감소 

	_float		m_fFrame = 0.f;						// 이미지 돌리기위한 프레임변수 
	_float		m_fFrameEnd;						// 이미지마다 변수 넣어줘야함 
	_float		m_fFrameSpeed;						// 프레임 돌리는 속도

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
};
