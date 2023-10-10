#pragma once

#include "Serialize_Core.h"



using namespace rapidjson;

/// <summary>
/// 컴포넌트가 가지는 직렬화 데이터
/// </summary>
class FSerialize_Component
{
public:
	FSeialize_Header	tHeader;
	//string
};



/// <summary>
/// 오브젝트가 가지는 직렬화 데이터
/// </summary>
class FSerialize_GameObject
{
public:
	FSeialize_Header				tHeader;
	set<string>						bTag;						// 태그
	_uint							iState;						// 상태 플래그

	_float							fPriority_Update;			// 업데이트 우선도
	_bool							bUsePriority_Update;		// 업데이트 우선도 사용
	_float							fPriority_LateUpdate;
	_bool							bUsePriority_LateUpdate;
	_float							fPriority_Render;
	_bool							bUsePriority_Render;

	vector<FSerialize_Component>	vecComponent;
};



/// <summary>
/// 레이어가 가지는 직렬화 데이터
/// </summary>
class FSerialize_Layer
{
public:
	FSeialize_Header				tHeader;
	_float							fPriority;		// 우선도
	vector<FSerialize_GameObject>	vecGameObject;	// 오브젝트 정보
};



/// <summary>
/// 터레인이 가지는 직렬화 데이터
/// </summary>
class FSerialize_Terrain
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
/// 씬이 가지는 직렬화 데이터
/// </summary>
class FSerialize_Scene
{
public:
	FSeialize_Header			tHeader;
	string						refTerrainPath;		// 씬의 지형 정보가 담긴, 터레인 자료에 대한 참조
	vector<string>				vecLayerPath;		// 씬의 레이어 참조 정보 

	void Parse_RapidJSON(Document& doc, StringBuffer& strBuf, _bool bPrettyWriter = false) const
	{
		// JSON 문서 구성
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


