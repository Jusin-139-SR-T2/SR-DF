#pragma once
#include "AceObjectFactory.h"
#include "GameObject.h"


BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CAceFood : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CAceFood)

enum class FOOD_NAME { APPLE, BANANA, COLA, MEDIKIT, 
					EATENAPPLE, BANANAPEEL, 
					FOOD_END };

private:
	explicit CAceFood(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceFood(const CAceFood& rhs);
	virtual ~CAceFood();

public:
	PRIVATE virtual HRESULT		Ready_GameObject() override { return S_OK; }
	PUBLIC	virtual HRESULT		Ready_GameObject(const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz);
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	static CAceFood*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz);

protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);

private: 
	virtual void		Free();

	HRESULT				Add_Component(); // ������Ʈ �߰� 
	HRESULT				BillBoard(const _float& fTimeDelta); // �÷��̾������� ���ϴ� �Լ� 
	void				Height_On_Terrain(); // ����Ÿ�� 

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;

private:// �Լ�
	void				FoodName(const _tchar* pObjTag); // TCHAR�� ENUM�� �°� ��ġ�� 
	void				Eat_Food(FOOD_NAME eCurName, const _float& fTimeDelta);

private: //����
	FOOD_NAME			m_pCurName;
	_bool				m_bEat = false;   //blackboard �����ؼ� �÷��̾� E��ȣ�ۿ� OR ��ŷ ���� Ȯ��
	_bool				m_bDead = false;  // �״°� ����� �װɷ� ���� 
	_vec3				CurPos;			  // ���� ��ġ 
	_float				m_fChangeTextureSpeed = 0.001f;
};

