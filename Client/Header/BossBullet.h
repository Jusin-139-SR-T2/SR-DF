//#pragma once
//#include "AceUnit.h"
//
//#include "Export_System.h"
//#include "Export_Utility.h"
//#include "Engine_Enum.h"
//
//BEGIN(Engine)
//
//class CRcBufferComp;
//class CTextureComponent;
//class CTransformComponent;
//
//END
//
//class CPlayer;
//class CBoss;
//
//class CBossBullet : public CAceUnit
//{
//	DERIVED_CLASS(CAceUnit, CBossBullet)
//
//private:
//	explicit CBossBullet(LPDIRECT3DDEVICE9 pGraphicDev);
//	explicit CBossBullet(const CBossBullet& rhs);
//	virtual ~CBossBullet();
//
//public:
//	virtual HRESULT		Ready_GameObject() override;
//	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
//	virtual void		LateUpdate_GameObject() override;
//	virtual void		Render_GameObject() override;
//
//	static CBossBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev,
//		_float _x, _float _y, _float _z, MonsterPhase _CurrPhase);
//
//private:
//	HRESULT				Add_Component();
//	virtual void		Free();
//	HRESULT				Billboard();
//	void				Value_Setting(_float _x, _float _y, _float _z, MonsterPhase _phase);
//
//	CRcBufferComp* m_pBufferComp = nullptr;
//	CTextureComponent* m_pTextureComp = nullptr;
//	CTransformComponent* m_pTransformComp = nullptr;
//
//public:
//	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
//		GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
//		GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
//
//private:
//	_float m_fFrame;
//	_float m_fFrameEnd;
//	_float m_fFrameSpeed;
//
//	_bool m_bBossDead = FALSE;
//};
