//#pragma once
//
//#include "Attack.h" // ���� Attack Ŭ������ �����մϴ�.
////#include "Player.h"
//
//class CBullet : public CAttack
//{
//    DERIVED_CLASS(CAttack, CBullet)
//
//private:
//    explicit CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
//    explicit CBullet(const CBullet& rhs);
//    virtual ~CBullet();
//
//public:
//    virtual HRESULT Ready_GameObject() override;
//    virtual _int Update_GameObject(const _float& fTimeDelta) override;
//    virtual void LateUpdate_GameObject() override;
//    virtual void Render_GameObject() override;
//
//public:
//    CBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, CGameObject* Onwer);
//
//private:
//    //CGameObject* m_pPlayer;
//
//};