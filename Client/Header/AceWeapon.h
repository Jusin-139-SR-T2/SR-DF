#pragma once

#include "AceObjectFactory.h"
#include "GameObject.h"

#include "Serialize_BaseClass.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;

END


class CAceWeapon : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CAceWeapon)

enum class WEAPON_NAME { BOTTLE, PIPE, WALLPIPE, FRYINGPAN, PISTOL, TOMMYGUN, GASCANISTER, WEAPON_END };

private:
	explicit CAceWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceWeapon(const CAceWeapon& rhs);
	virtual ~CAceWeapon();

public:
	static CAceWeapon* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz);
	static CAceWeapon* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial );

private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual HRESULT		Ready_GameObject(const FSerialize_GameObject& tObjectSerial);
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	

private: // �Լ� 
	HRESULT				Add_Component();
	HRESULT				BillBoard(const _float& fTimeDelta); // �÷��̾������� ���ϴ� �Լ� 
	void				Height_On_Terrain();
	void				WeaponName(const _tchar* pObjTag);
	void				Change_Texture(WEAPON_NAME eReceiveName); //��� ���⿡ �������� ���� �̹��� ���� 
	
private: // ���� 
	WEAPON_NAME			m_pCurName;
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

