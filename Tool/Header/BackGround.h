#pragma once

#include "GameObject.h"
#include "ImguiAnimationTool.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

//// Ű������ ����ü
//struct Keyframe
//{
//	char name[64];			// Ű������ �̸� (ǥ�ÿ�)
//
//	bool isEaseIn;			// Ease In ���� (True �Ǵ� False)
//	bool isEaseOut;			// Ease Out ���� (True �Ǵ� False)
//
//	float time;				// Ű�������� �ð� (0.0f ~ MaxTime ����)
//	float value;			// �ִϸ��̼� �� (ũ��, ȸ��, �̵� ��)
//	float color[3];			// Ű������ ���� (R, G, B)
//
//	OBJ_TYPE m_eObjectType; // Ÿ���� �ο��� �׸� (ex : �Ѽ�, ���)
//	OBJ_NAME m_eObjectName; // �̸��� �ο��� �׸� (ex : ����, ��������)
//
//	int type;				// �ִϸ��̼� Ÿ�� (0: ũ��, 1: ȸ��, 2: �̵�)
//
//	_vec3	vScale = { 0.f, 0.f, 0.f };			// ũ�⸦ ���� �׸�
//	_vec3	vRot = { 0.f, 0.f, 0.f };			// ȸ���� ���� �׸�
//	_vec3	vPos = { 0.f, 0.f, 0.f };			// ��ġ�� ���� �׸�
//};

class CBackGround : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CBackGround)

private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBackGround(const CBackGround& rhs);
	virtual ~CBackGround();

public:
	static CBackGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

private:
	HRESULT				Add_Component();

private:
	// �ִϸ��̼� �ҷ�����
	void LoadAnimationFromFile(const char* fileName);

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	GETSET_EX2(CTransformComponent*,	m_pTransformComp,	TransformComponent,	GET, SET)
	GETSET_EX2(CRcBufferComp*,			m_pBufferComp,		BufferComponent,	GET, SET)
	GETSET_EX2(CTextureComponent*,		m_pTextureComp,		TextureComponent,	GET, SET)

private:
	CTransformComponent*			m_pTransformComp = nullptr;
	CRcBufferComp*					m_pBufferComp = nullptr;
	CTextureComponent*				m_pTextureComp = nullptr;
	CTextureComponent*				m_pBackTextureComp = nullptr;

	_matrix		m_ViewMatrix, m_ProjMatrix;
private:
	_float	m_fX, m_fY, m_fSizeX, m_fSizeY;

	// �ִϸ��̼� ������
	_float m_fAniamtionFrame = 0.f;
	_float m_fAnimationSpeed = 7.f;
	_float m_fMaxFrame = 0.f;

	int m_iFrameCount = 0;

	CImguiAnimationTool* m_pAnimationTool;
	std::vector<Keyframe>*  m_vecAnimationInfo;

	Keyframe m_eAnimationInfo;
	_bool bFrameOn = false;
};

