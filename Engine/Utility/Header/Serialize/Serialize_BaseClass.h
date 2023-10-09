#pragma once

#include "Serialize_Core.h"

using namespace rapidjson;

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
	set<string>						bTag;						// �±�
	_uint							iState;						// ���� �÷���

	_float							fPriority_Update;			// ������Ʈ �켱��
	_bool							bUsePriority_Update;		// ������Ʈ �켱�� ���
	_float							fPriority_LateUpdate;
	_bool							bUsePriority_LateUpdate;
	_float							fPriority_Render;
	_bool							bUsePriority_Render;

	vector<FSerialize_Component>	vecComponent;
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
/// �ͷ����� ������ ����ȭ ������
/// </summary>
class ENGINE_DLL FSerialize_Terrain
{
	
public:
	FSeialize_Header			tHeader;
	FSeialize_Vector3			vVertexCount;	// ���ؽ� ����
	FSeialize_Vector3			vScale;			// ������, ������ �ʺ�
	FSeialize_Vector3			vInvOffset;		// ����, ������
	

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf, _bool bPrettyWriter = false) const
	{
		// JSON ���� ����
		doc.SetObject();

		// FSer_Header ����
		Value header(kObjectType);
		header.AddMember("type", Value().SetString(to_string(tHeader.strType).c_str(), doc.GetAllocator()), doc.GetAllocator());
		header.AddMember("name", Value().SetString(tHeader.strName.c_str(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember("header", header, doc.GetAllocator());

		Value vertex(kObjectType);
		vertex.AddMember("x", Value().SetString(to_string(vVertexCount.x).c_str(), doc.GetAllocator()), doc.GetAllocator());
		vertex.AddMember("y", Value().SetString(to_string(vVertexCount.y).c_str(), doc.GetAllocator()), doc.GetAllocator());
		vertex.AddMember("z", Value().SetString(to_string(vVertexCount.z).c_str(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember("vertex", vertex, doc.GetAllocator());

		Value scale(kObjectType);
		scale.AddMember("x", Value().SetString(to_string(vScale.x).c_str(), doc.GetAllocator()), doc.GetAllocator());
		scale.AddMember("y", Value().SetString(to_string(vScale.y).c_str(), doc.GetAllocator()), doc.GetAllocator());
		scale.AddMember("z", Value().SetString(to_string(vScale.z).c_str(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember("scale", scale, doc.GetAllocator());

		Value invoffset(kObjectType);
		invoffset.AddMember("x", Value().SetString(to_string(vInvOffset.x).c_str(), doc.GetAllocator()), doc.GetAllocator());
		invoffset.AddMember("y", Value().SetString(to_string(vInvOffset.y).c_str(), doc.GetAllocator()), doc.GetAllocator());
		invoffset.AddMember("z", Value().SetString(to_string(vInvOffset.z).c_str(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember("invoffset", invoffset, doc.GetAllocator());

		if (bPrettyWriter)
		{
			PrettyWriter<StringBuffer> writer(strBuf);
			doc.Accept(writer);
		}
		else
		{
			Writer<StringBuffer> writer(strBuf);
			doc.Accept(writer);
		}
	}

	_bool Receive_ByRapidJSON(const string& strJSON)
	{
		Document doc;
		doc.Parse(strJSON.c_str());

		tHeader.strName = doc["name"].GetString();
		tHeader.strType = static_cast<ESERIALIZE_TYPE>(doc["type"].GetInt());

		vVertexCount.x = doc["vertex"]["x"].GetFloat();
		vVertexCount.y = doc["vertex"]["y"].GetFloat();
		vVertexCount.z = doc["vertex"]["z"].GetFloat();

		vScale.x = doc["scale"]["x"].GetFloat();
		vScale.y = doc["scale"]["y"].GetFloat();
		vScale.z = doc["scale"]["z"].GetFloat();

		vInvOffset.x = doc["invoffset"]["x"].GetFloat();
		vInvOffset.y = doc["invoffset"]["y"].GetFloat();
		vInvOffset.z = doc["invoffset"]["z"].GetFloat();
	}
};



/// <summary>
/// ���� ������ ����ȭ ������
/// </summary>
class ENGINE_DLL FSerialize_Scene
{
public:
	FSeialize_Header			tHeader;
	string						refTerrainPath;		// ���� ���� ������ ���, �ͷ��� �ڷῡ ���� ����
	vector<string>				vecLayerPath;		// ���� ���̾� ���� ���� 

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf, _bool bPrettyWriter = false) const
	{
		// JSON ���� ����
		doc.SetObject();



		if (bPrettyWriter)
		{
			PrettyWriter<StringBuffer> writer(strBuf);
			doc.Accept(writer);
		}
		else
		{
			Writer<StringBuffer> writer(strBuf);
			doc.Accept(writer);
		}
	}
};


