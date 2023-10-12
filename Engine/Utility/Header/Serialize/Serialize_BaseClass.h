#pragma once

#include "Serialize_Core.h"



using namespace rapidjson;

/// <summary>
/// 컴포넌트가 가지는 직렬화 데이터
/// </summary>
class ENGINE_DLL FSerialize_Component
{
public:
	FSeialize_Header	tHeader;
	//string
};



/// <summary>
/// 오브젝트가 가지는 직렬화 데이터
/// </summary>
class ENGINE_DLL FSerialize_GameObject
{
public:
	FSeialize_Header				tHeader;
	EGO_CLASS						strID;						// 실제 오브젝트의 원본 태그

	set<string>						bTag;						// 태그

	_float							fPriority_Update;			// 업데이트 우선도
	_bool							bUsePriority_Update;		// 업데이트 우선도 사용
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
		const _bool bPrettyWriter = false) const
	{
		Document::AllocatorType& allocator = doc.GetAllocator();

		// JSON 문서 구성
		if (eProcess == ESERIALIZE_PROCESS_INIT
			|| eProcess == ESERIALIZE_PROCESS_IMMEDIATE)
			doc.SetObject();

		// 씬 헤더
		Value header(kObjectType);
		header.AddMember("type", Value().SetInt(tHeader.strType), allocator);
		header.AddMember("name", Value().SetString(tHeader.strName.c_str(), allocator), allocator);
		doc.AddMember("header", header, allocator);

		// 클래스 타입
		doc.AddMember("class", Value().SetInt(eID), allocator);

		// 위치
		Value pos(kObjectType);
		pos.AddMember("x", Value().SetFloat(vPos.x), allocator);
		pos.AddMember("y", Value().SetFloat(vPos.y), allocator);
		pos.AddMember("z", Value().SetFloat(vPos.z), allocator);
		doc.AddMember("pos", pos, allocator);

		// 회전
		Value rot(kObjectType);
		rot.AddMember("x", Value().SetFloat(vRot.x), allocator);
		rot.AddMember("y", Value().SetFloat(vRot.y), allocator);
		rot.AddMember("z", Value().SetFloat(vRot.z), allocator);
		doc.AddMember("rot", rot, allocator);

		// 크기
		Value scale(kObjectType);
		scale.AddMember("x", Value().SetFloat(vScale.x), allocator);
		scale.AddMember("y", Value().SetFloat(vScale.y), allocator);
		scale.AddMember("z", Value().SetFloat(vScale.z), allocator);
		doc.AddMember("scale", scale, allocator);


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

		eID = static_cast<EGO_CLASS>(doc["class"].GetInt());

		vPos.x = doc["pos"]["x"].GetFloat();
		vPos.y = doc["pos"]["y"].GetFloat();
		vPos.z = doc["pos"]["z"].GetFloat();

		vRot.x = doc["rot"]["x"].GetFloat();
		vRot.y = doc["rot"]["y"].GetFloat();
		vRot.z = doc["rot"]["z"].GetFloat();

		vScale.x = doc["scale"]["x"].GetFloat();
		vScale.y = doc["scale"]["y"].GetFloat();
		vScale.z = doc["scale"]["z"].GetFloat();

		return true;
	}
};




/// <summary>
/// 터레인이 가지는 직렬화 데이터
/// </summary>
class ENGINE_DLL FSerialize_Terrain
{
	
public:
	FSeialize_Header			tHeader;
	_vec3						vVertexCount;	// 버텍스 개수
	_vec3						vScale;			// 스케일, 정점간 너비
	_vec3						vInvOffset;		// 중점, 역버전
	

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf, _bool bPrettyWriter = false) const
	{
		// JSON 문서 구성
		doc.SetObject();

		// FSer_Header 구성
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
/// 레이어가 가지는 직렬화 데이터
/// </summary>
class ENGINE_DLL FSerialize_Layer
{
public:
	FSeialize_Header				tHeader;
	_float							fPriority;		// 우선도
	vector<FSerialize_GameObject>	vecGameObject;	// 오브젝트 정보
};

/// <summary>
/// 씬이 가지는 직렬화 데이터
/// </summary>
class ENGINE_DLL FSerialize_Scene
{
public:
	FSeialize_Header			tHeader;
	string						refTerrainName;		// 씬의 지형 정보가 담긴, 터레인 자료에 대한 참조
	vector<FSerialize_Layer>	vecLayer;			// 씬의 레이어 정보 

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf, 
		const ESERIALIZE_PROCESS eProcess, 
		const _bool bPrettyWriter = false) const
	{
		Document::AllocatorType& allocator = doc.GetAllocator();

		// JSON 문서 구성
		if (eProcess == ESERIALIZE_PROCESS_INIT
			|| eProcess == ESERIALIZE_PROCESS_IMMEDIATE)
			doc.SetObject();

		// 씬 헤더
		Value header(kObjectType);
		header.AddMember("type", Value().SetInt(tHeader.strType), allocator);
		header.AddMember("name", Value().SetString(tHeader.strName.c_str(), allocator), allocator);
		doc.AddMember("header", header, allocator);

		// 터레인
		Value terrain(kObjectType);
		doc.AddMember("terrain", Value().SetString(refTerrainName.c_str(), allocator), allocator);

		Value arrLayer(kArrayType);
		for (size_t i = 0; i < vecLayer.size(); i++)
		{
			Value layer(kObjectType);
			
			// 레이어 헤더
			Value layer_header(kObjectType);
			layer_header.AddMember("type", Value().SetInt(vecLayer[i].tHeader.strType), allocator);
			layer_header.AddMember("name", Value().SetString(vecLayer[i].tHeader.strName.c_str(), allocator), allocator);
			layer.AddMember("header", layer_header, allocator);

			// 우선도
			layer.AddMember("priority", Value().SetFloat(vecLayer[i].fPriority), allocator);

			Value arrObject(kArrayType);
			for (size_t j = 0; j < vecLayer[i].vecGameObject.size(); j++)
			{
				Value gameobject(kObjectType);


				// 오브젝트 헤더
				Value gameobject_header(kObjectType);
				gameobject_header.AddMember("type", Value().SetInt(vecLayer[i].vecGameObject[j].tHeader.strType), allocator);
				gameobject_header.AddMember("name", Value().SetString(vecLayer[i].vecGameObject[j].tHeader.strName.c_str(), allocator), allocator);
				gameobject.AddMember("header", gameobject_header, allocator);


				// 오브젝트 식별자
				Value gameobject_ID(kObjectType);
				gameobject.AddMember("ID", Value().SetInt(vecLayer[i].vecGameObject[j].strID), allocator);


				// 태그 이름 배열로 넣기
				Value arrtag(kArrayType);
				for (const auto& tagItem : vecLayer[i].vecGameObject[j].bTag)
				{
					Value tag(kObjectType);
					tag.AddMember("name", Value().SetString(tagItem.c_str(), allocator), allocator);
					arrtag.PushBack(tag, allocator);
				}
				gameobject.AddMember("tags", arrtag, allocator);


				// 우선도 설정
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
				
				// 배열에 넣기
				arrObject.PushBack(gameobject, allocator);
			}
			layer.AddMember("objects", arrObject, allocator);

			// 배열에 넣기
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

		if (!doc.IsObject())
			return false;

		tHeader.strType = static_cast<ESERIALIZE_TYPE>(doc["header"]["type"].GetInt());
		tHeader.strName = doc["header"]["name"].GetString();

		refTerrainName = doc["terrain"].GetString();

		// 레이어 로드
		if (doc.HasMember("layers") && doc["layers"].IsArray())
		for (SizeType i = 0; i < doc["layers"].Size(); i++)
		{
			Value& layer = doc["layers"][i];
			FSerialize_Layer layerSR;
			layerSR.tHeader.strType = static_cast<ESERIALIZE_TYPE>(layer["header"]["type"].GetInt());
			layerSR.tHeader.strName = layer["header"]["name"].GetString();

			layerSR.fPriority = layer["priority"].GetFloat();

			// 오브젝트 로드
			if (layer.HasMember("objects") && layer["objects"].IsArray())
			for (SizeType j = 0; j < layer["objects"].Size(); j++)
			{
				Value& object = layer["objects"][j];
				FSerialize_GameObject gameobjectSR;
				gameobjectSR.tHeader.strType = static_cast<ESERIALIZE_TYPE>(object["header"]["type"].GetInt());
				gameobjectSR.tHeader.strName = object["header"]["name"].GetString();

				gameobjectSR.vPos.x = object["pos"]["x"].GetFloat();
				gameobjectSR.vPos.y = object["pos"]["y"].GetFloat();
				gameobjectSR.vPos.z = object["pos"]["z"].GetFloat();

				gameobjectSR.vRotation.x = object["rot"]["x"].GetFloat();
				gameobjectSR.vRotation.y = object["rot"]["y"].GetFloat();
				gameobjectSR.vRotation.z = object["rot"]["z"].GetFloat();

				gameobjectSR.vScale.x = object["scale"]["x"].GetFloat();
				gameobjectSR.vScale.y = object["scale"]["y"].GetFloat();
				gameobjectSR.vScale.z = object["scale"]["z"].GetFloat();

				gameobjectSR.fPriority_Update = object["priority_update"].GetFloat();
				gameobjectSR.fPriority_LateUpdate = object["priority_late_update"].GetFloat();
				gameobjectSR.fPriority_Render = object["priority_render"].GetFloat();

				gameobjectSR.bUsePriority_Update = object["use_priority_update"].GetBool();
				gameobjectSR.bUsePriority_LateUpdate = object["use_priority_late_update"].GetBool();
				gameobjectSR.bUsePriority_Render = object["use_priority_render"].GetBool();

				layerSR.vecGameObject.push_back(gameobjectSR);
			}
			vecLayer.push_back(layerSR);
		}

		return true;
	}
};


