#pragma once

#include "AceObjectFactory.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcBufferComp;
class CCubeBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;

END

class CAceBuilding : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CAceBuilding)

	enum class BUILDING_NAME { 
		A, // 2����
		B, // �Ķ�����Ʈ 
		C, // ������ 3��
		D, // 1���� �����ִ� ���ǽ��� 
		E, // �������� 3�� ���� 
		F, // �б�
		BUILDING_END};
	
private:
	explicit CAceBuilding(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceBuilding(const CAceBuilding& rhs);
	virtual ~CAceBuilding();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	static CAceBuilding* Create(LPDIRECT3DDEVICE9 pGraphicDev, 
		const _tchar* pObjTag, 
		const _float _fx, 
		const _float _fy, 
		const _float _fz,
		CAceObjectFactory::OBJECT_DIRECTION pDirection = CAceObjectFactory::OBJECT_DIRECTION::SOUTH);

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CCubeBufferComp* m_pCubeBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CCubeBufferComp*, m_pCubeBufferComp, CubeBufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

private: // �Լ� 
	virtual void		Free();											// Release
	HRESULT				Add_Component();								// ������Ʈ �߰� 
	void				Height_On_Terrain();							// ����Ÿ�� 
	void				ObjectName(const _tchar* pObjTag);				// ó�� �־��� wchar�� enum���� ���� 

	BUILDING_NAME		m_pCurName;
	_float m_fHeight;
	HRESULT				Set_BuildingDir(CAceObjectFactory::OBJECT_DIRECTION _eDir);
};
