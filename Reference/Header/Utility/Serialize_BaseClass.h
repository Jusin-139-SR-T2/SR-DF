#pragma once

#include "Serialize_Core.h"

using namespace rapidjson;


BEGIN(Engine)

/// <summary>
/// ������Ʈ�� ������ ����ȭ ������
/// </summary>
class ENGINE_DLL FSerialize_Component
{
public:
	FSeialize_Header	tHeader;
	//string
};



/// <summary>
/// ������Ʈ�� ������ ����ȭ ������
/// </summary>
class ENGINE_DLL FSerialize_GameObject
{
public:
	FSeialize_Header				tHeader;
	EGO_CLASS						strID;						// ���� ������Ʈ�� ���� �±�

	set<string>						bTag;						// �±�

	_float							fPriority_Update;			// ������Ʈ �켱��
	_bool							bUsePriority_Update;		// ������Ʈ �켱�� ���
	_float							fPriority_LateUpdate;
	_bool							bUsePriority_LateUpdate;
	_float							fPriority_Render;
	_bool							bUsePriority_Render;

	_vec3							vPos;
	_vec3							vRotation;
	_vec3							vScale;

	vector<FSerialize_Component>	vecComponent;
};


class ENGINE_DLL FSerialize_Proto
{
public:
	FSeialize_Header				tHeader;
	EGO_CLASS						eID;

	_vec3							vPos;
	_vec3							vRot;
	_vec3							vScale;

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf,
		const ESERIALIZE_PROCESS eProcess,
		const _bool bPrettyWriter = false) const;
	
	_bool Receive_ByRapidJSON(string& strJSON, _bool bParseRewriteAble = false);
};




/// <summary>
/// �ͷ����� ������ ����ȭ ������
/// </summary>
class ENGINE_DLL FSerialize_Terrain
{
	
public:
	FSeialize_Header			tHeader;
	_vec3						vVertexCount = { 128.f, 128.f, 128.f };	// ���ؽ� ����
	_vec3						vScale = { 1.f, 1.f, 1.f };				// ������, ������ �ʺ�
	_vec3						vInvOffset = { 0.f, 0.f, 0.f };			// ����, ������
	

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf, _bool bPrettyWriter = false) const;
	_bool Receive_ByRapidJSON(string& strJSON, _bool bParseRewriteAble = false);
	
};

/// <summary>
/// ���̾ ������ ����ȭ ������
/// </summary>
class ENGINE_DLL FSerialize_Layer
{
public:
	FSeialize_Header				tHeader;
	_float							fPriority;		// �켱��
	vector<FSerialize_GameObject>	vecGameObject;	// ������Ʈ ����
};

/// <summary>
/// ���� ������ ����ȭ ������
/// </summary>
class ENGINE_DLL FSerialize_Scene
{
public:
	FSeialize_Header			tHeader;
	string						refTerrainName;		// ���� ���� ������ ���, �ͷ��� �ڷῡ ���� ����
	vector<FSerialize_Layer>	vecLayer;			// ���� ���̾� ���� 

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf,
		const ESERIALIZE_PROCESS eProcess,
		const _bool bPrettyWriter = false) const;
	

	_bool Receive_ByRapidJSON(string& strJSON, _bool bParseRewriteAble = false);
	
};


END