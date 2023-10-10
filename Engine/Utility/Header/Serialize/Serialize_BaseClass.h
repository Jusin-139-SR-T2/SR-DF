#pragma once

#include "Serialize_Core.h"



using namespace rapidjson;

/// <summary>
/// ������Ʈ�� ������ ����ȭ ������
/// </summary>
class FSerialize_Component
{
public:
	FSeialize_Header	tHeader;
	//string
};



/// <summary>
/// ������Ʈ�� ������ ����ȭ ������
/// </summary>
class FSerialize_GameObject
{
public:
	FSeialize_Header				tHeader;
	string							strID;						// ���� ������Ʈ�� ���� �̸�

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







/// <summary>
/// �ͷ����� ������ ����ȭ ������
/// </summary>
class FSerialize_Terrain
{
	
public:
	FSeialize_Header			tHeader;
	_vec3						vVertexCount;	// ���ؽ� ����
	_vec3						vScale;			// ������, ������ �ʺ�
	_vec3						vInvOffset;		// ����, ������
	

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf, _bool bPrettyWriter = false) const
	{
		// JSON ���� ����
		doc.SetObject();

		// FSer_Header ����
		Value header(kObjectType);
		header.AddMember("type", Value().SetInt(tHeader.strType), doc.GetAllocator());
		header.AddMember("name", Value().SetString(tHeader.strName.c_str(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember("header", header, doc.GetAllocator());

		Value vertex(kObjectType);
		vertex.AddMember("x", Value().SetFloat(vVertexCount.x), doc.GetAllocator());
		vertex.AddMember("y", Value().SetFloat(vVertexCount.y), doc.GetAllocator());
		vertex.AddMember("z", Value().SetFloat(vVertexCount.z), doc.GetAllocator());
		doc.AddMember("vertex", vertex, doc.GetAllocator());

		Value scale(kObjectType);
		scale.AddMember("x", Value().SetFloat(vScale.x), doc.GetAllocator());
		scale.AddMember("y", Value().SetFloat(vScale.y), doc.GetAllocator());
		scale.AddMember("z", Value().SetFloat(vScale.z), doc.GetAllocator());
		doc.AddMember("scale", scale, doc.GetAllocator());

		Value invoffset(kObjectType);
		invoffset.AddMember("x", Value().SetFloat(vInvOffset.x), doc.GetAllocator());
		invoffset.AddMember("y", Value().SetFloat(vInvOffset.y), doc.GetAllocator());
		invoffset.AddMember("z", Value().SetFloat(vInvOffset.z), doc.GetAllocator());
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

	_bool Receive_ByRapidJSON(string& strJSON, _bool bParseRewriteAble = false)
	{
		Document doc;
		if (bParseRewriteAble)
			doc.ParseInsitu(const_cast<char*>(strJSON.c_str()));
		else
			doc.Parse(strJSON.c_str());

		if (doc.HasParseError())
			return false;

		tHeader.strType = static_cast<ESERIALIZE_TYPE>(doc["header"]["type"].GetInt());
		tHeader.strName = doc["header"]["name"].GetString();

		vVertexCount.x = doc["vertex"]["x"].GetFloat();
		vVertexCount.y = doc["vertex"]["y"].GetFloat();
		vVertexCount.z = doc["vertex"]["z"].GetFloat();

		vScale.x = doc["scale"]["x"].GetFloat();
		vScale.y = doc["scale"]["y"].GetFloat();
		vScale.z = doc["scale"]["z"].GetFloat();

		vInvOffset.x = doc["invoffset"]["x"].GetFloat();
		vInvOffset.y = doc["invoffset"]["y"].GetFloat();
		vInvOffset.z = doc["invoffset"]["z"].GetFloat();

		return true;
	}
};

/// <summary>
/// ���̾ ������ ����ȭ ������
/// </summary>
class FSerialize_Layer
{
public:
	FSeialize_Header				tHeader;
	_float							fPriority;		// �켱��
	vector<FSerialize_GameObject>	vecGameObject;	// ������Ʈ ����
};

/// <summary>
/// ���� ������ ����ȭ ������
/// </summary>
class FSerialize_Scene
{
public:
	FSeialize_Header			tHeader;
	string						refTerrainName;		// ���� ���� ������ ���, �ͷ��� �ڷῡ ���� ����
	vector<FSerialize_Layer>	vecLayer;			// ���� ���̾� ���� 

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf, 
		const ESERIALIZE_PROCESS eProcess, 
		const _bool bPrettyWriter = false) const
	{
		Document::AllocatorType& allocator = doc.GetAllocator();

		// JSON ���� ����
		if (eProcess == ESERIALIZE_PROCESS_INIT
			|| eProcess == ESERIALIZE_PROCESS_IMMEDIATE)
			doc.SetObject();

		// �� ���
		Value header(kObjectType);
		header.AddMember("type", Value().SetInt(tHeader.strType), allocator);
		header.AddMember("name", Value().SetString(tHeader.strName.c_str(), allocator), allocator);
		doc.AddMember("header", header, allocator);

		// �ͷ���
		Value terrain(kObjectType);
		doc.AddMember("terrain", Value().SetString(refTerrainName.c_str(), allocator), allocator);

		Value arrLayer(kArrayType);
		for (size_t i = 0; i < vecLayer.size(); i++)
		{
			Value layer(kObjectType);
			
			// ���̾� ���
			Value layer_header(kObjectType);
			layer_header.AddMember("type", Value().SetInt(vecLayer[i].tHeader.strType), allocator);
			layer_header.AddMember("name", Value().SetString(vecLayer[i].tHeader.strName.c_str(), allocator), allocator);
			layer.AddMember("header", layer_header, allocator);

			// �켱��
			layer.AddMember("priority", Value().SetFloat(vecLayer[i].fPriority), allocator);


			for (size_t j = 0; j < vecLayer[i].vecGameObject.size(); j++)
			{
				Value gameobject(kObjectType);


				// ������Ʈ ���
				Value gameobject_header(kObjectType);
				gameobject_header.AddMember("type", Value().SetInt(vecLayer[i].vecGameObject[j].tHeader.strType), allocator);
				gameobject_header.AddMember("name", Value().SetString(vecLayer[i].vecGameObject[j].tHeader.strName.c_str(), allocator), allocator);
				gameobject.AddMember("header", gameobject_header, allocator);


				// ������Ʈ �ĺ���
				Value gameobject_ID(kObjectType);
				gameobject.AddMember("ID", Value().SetString(vecLayer[i].vecGameObject[j].strID.c_str(), allocator), allocator);


				// �±� �̸� �迭�� �ֱ�
				Value arrtag(kArrayType);
				for (const auto& tagItem : vecLayer[i].vecGameObject[j].bTag)
				{
					Value tag(kObjectType);
					tag.AddMember("name", Value().SetString(tagItem.c_str(), allocator), allocator);
					arrtag.PushBack(tag, allocator);
				}
				gameobject.AddMember("tags", arrtag, allocator);


				// �켱�� ����
				Value gameobject_priority1(kObjectType);
				gameobject_priority1.AddMember("priority_update", Value().SetFloat(vecLayer[i].vecGameObject[j].fPriority_Update), allocator);

				Value gameobject_priority2(kObjectType);
				gameobject_priority2.AddMember("use_priority_update", Value().SetBool(vecLayer[i].vecGameObject[j].bUsePriority_Update), allocator);

				Value gameobject_priority3(kObjectType);
				gameobject_priority3.AddMember("priority_late_update", Value().SetFloat(vecLayer[i].vecGameObject[j].fPriority_LateUpdate), allocator);

				Value gameobject_priority4(kObjectType);
				gameobject_priority4.AddMember("use_priority_late_update", Value().SetBool(vecLayer[i].vecGameObject[j].bUsePriority_LateUpdate), allocator);

				Value gameobject_priority5(kObjectType);
				gameobject_priority5.AddMember("priority_render", Value().SetFloat(vecLayer[i].vecGameObject[j].fPriority_Render), allocator);

				Value gameobject_priority6(kObjectType);
				gameobject_priority6.AddMember("use_priority_render", Value().SetBool(vecLayer[i].vecGameObject[j].bUsePriority_Render), allocator);
			}

			arrLayer.PushBack(layer, allocator);
		}
		doc.AddMember("layers", arrLayer, allocator);

		if (eProcess == ESERIALIZE_PROCESS_END
			|| eProcess == ESERIALIZE_PROCESS_IMMEDIATE)
		{
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
	}

	_bool Receive_ByRapidJSON(string& strJSON, _bool bParseRewriteAble = false)
	{
		Document doc;
		if (bParseRewriteAble)
			doc.ParseInsitu(const_cast<char*>(strJSON.c_str()));
		else
			doc.Parse(strJSON.c_str());

		if (doc.HasParseError())
			return false;

		tHeader.strType = static_cast<ESERIALIZE_TYPE>(doc["header"]["type"].GetInt());
		tHeader.strName = doc["header"]["name"].GetString();

		refTerrainName = doc["terrain"].GetString();

		return true;
	}
};


