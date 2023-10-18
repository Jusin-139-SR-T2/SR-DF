#include "Serialize_BaseClass.h"

void FSerialize_Proto::Parse_RapidJSON(Document& doc, StringBuffer& strBuf, const ESERIALIZE_PROCESS eProcess, const _bool bPrettyWriter) const
{
	auto& allocator = doc.GetAllocator();

	// JSON 문서 구성
	/*if (eProcess == ESERIALIZE_PROCESS_INIT
		|| eProcess == ESERIALIZE_PROCESS_IMMEDIATE)*/
	doc.SetObject();

	// 씬 헤더
	Value header(kObjectType);
	header.AddMember("type", Value().SetInt(tHeader.eType), allocator);
	header.AddMember("name", Value().SetString(tHeader.strName.c_str(), allocator), allocator);
	doc.AddMember("header", header, allocator);

	// 클래스 타입, 이제 안쓸거임
	doc.AddMember("class", Value().SetInt(static_cast<_int>(eID)), allocator);
	// 클래스 이름
	doc.AddMember("class_name", Value().SetString(strClassName.c_str(), allocator), allocator);

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

	doc.AddMember("group_key", Value().SetString(strGroupKey.c_str(), allocator), allocator);
	doc.AddMember("texture_key", Value().SetString(strTextureKey.c_str(), allocator), allocator);

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

_bool FSerialize_Proto::Receive_ByRapidJSON(string& strJSON, _bool bParseRewriteAble)
{
	Document doc;
	if (bParseRewriteAble)
		doc.ParseInsitu(const_cast<char*>(strJSON.c_str()));
	else
		doc.Parse(strJSON.c_str());

	if (doc.HasParseError())
		return false;

	if (doc.HasMember("header"))
	{
		const Value& header = doc["header"];
		header.HasMember("type") ? tHeader.eType = static_cast<ESERIALIZE_TYPE>(header["type"].GetInt()) : 0;
		header.HasMember("name") ? tHeader.strName = header["name"].GetString() : 0;
	}

	doc.HasMember("class") ? eID = static_cast<EGO_CLASS>(doc["class"].GetInt()) : 0;
	doc.HasMember("class_name") ? strClassName = doc["class_name"].GetString() : "";

	if (doc.HasMember("pos"))
	{
		const Value& pos = doc["pos"];
		pos.HasMember("x") ? vPos.x = pos["x"].GetFloat() : 0;
		pos.HasMember("y") ? vPos.y = pos["y"].GetFloat() : 0;
		pos.HasMember("z") ? vPos.z = pos["z"].GetFloat() : 0;
	}

	if (doc.HasMember("rot"))
	{
		const Value& rot = doc["rot"];
		rot.HasMember("x") ? vRot.x = rot["x"].GetFloat() : 0;
		rot.HasMember("y") ? vRot.y = rot["y"].GetFloat() : 0;
		rot.HasMember("z") ? vRot.z = rot["z"].GetFloat() : 0;
	}

	if (doc.HasMember("scale"))
	{
		const Value& scale = doc["scale"];
		scale.HasMember("x") ? vScale.x = scale["x"].GetFloat() : 0;
		scale.HasMember("y") ? vScale.y = scale["y"].GetFloat() : 0;
		scale.HasMember("z") ? vScale.z = scale["z"].GetFloat() : 0;
	}

	doc.HasMember("group_key") ? strGroupKey = doc["group_key"].GetString() : 0;
	doc.HasMember("texture_key") ? strTextureKey = doc["texture_key"].GetString() : 0;

	return true;
}

void FSerialize_Terrain::Parse_RapidJSON(Document& doc, StringBuffer& strBuf, _bool bPrettyWriter) const
{
	// JSON 문서 구성
	doc.SetObject();

	// FSer_Header 구성
	Value header(kObjectType);
	header.AddMember("type", Value().SetInt(tHeader.eType), doc.GetAllocator());
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

_bool FSerialize_Terrain::Receive_ByRapidJSON(string& strJSON, _bool bParseRewriteAble)
{
	Document doc;
	if (bParseRewriteAble)
		doc.ParseInsitu(const_cast<char*>(strJSON.c_str()));
	else
		doc.Parse(strJSON.c_str());

	if (doc.HasParseError())
		return false;

	tHeader.eType = static_cast<ESERIALIZE_TYPE>(doc["header"]["type"].GetInt());
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

void FSerialize_Scene::Parse_RapidJSON(Document& doc, StringBuffer& strBuf, const ESERIALIZE_PROCESS eProcess, const _bool bPrettyWriter) const
{
	auto& allocator = doc.GetAllocator();

	// JSON 문서 구성
	if (eProcess == ESERIALIZE_PROCESS_INIT
		|| eProcess == ESERIALIZE_PROCESS_IMMEDIATE)
		doc.SetObject();

	// 씬 헤더
	Value header(kObjectType);
	header.AddMember("type", Value().SetInt(tHeader.eType), allocator);
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
		layer_header.AddMember("type", Value().SetInt(vecLayer[i].tHeader.eType), allocator);
		layer_header.AddMember("name", Value().SetString(vecLayer[i].tHeader.strName.c_str(), allocator), allocator);
		layer.AddMember("header", layer_header, allocator);

		// 우선도
		layer.AddMember("priority", Value().SetFloat(vecLayer[i].fPriority), allocator);

		Value arrObject(kArrayType);
		for (size_t j = 0; j < vecLayer[i].vecGameObject.size(); j++)
		{
			Value gameobject(kObjectType);
			const FSerialize_GameObject& tObjectSerial = vecLayer[i].vecGameObject[j];

			// 오브젝트 헤더
			Value gameobject_header(kObjectType);
			gameobject_header.AddMember("type", Value().SetInt(tObjectSerial.tHeader.eType), allocator);
			gameobject_header.AddMember("name", Value().SetString(tObjectSerial.tHeader.strName.c_str(), allocator), allocator);
			gameobject.AddMember("header", gameobject_header, allocator);


			// 오브젝트 식별자
			Value gameobject_ID(kObjectType);
			gameobject.AddMember("ID", Value().SetInt(tObjectSerial.eID), allocator);

			// 클래스 이름
			gameobject.AddMember("class_name", Value().SetString(tObjectSerial.strClassName.c_str(), allocator), allocator);


			// 태그 이름 배열로 넣기
			Value arrtag(kArrayType);
			for (const auto& tagItem : tObjectSerial.bTag)
			{
				Value tag(kObjectType);
				tag.AddMember("name", Value().SetString(tagItem.c_str(), allocator), allocator);
				arrtag.PushBack(tag, allocator);
			}
			gameobject.AddMember("tags", arrtag, allocator);

			// 우선도 설정
			gameobject.AddMember("priority_update", Value().SetFloat(tObjectSerial.fPriority_Update), allocator);
			gameobject.AddMember("use_priority_update", Value().SetBool(tObjectSerial.bUsePriority_Update), allocator);
			gameobject.AddMember("priority_late_update", Value().SetFloat(tObjectSerial.fPriority_LateUpdate), allocator);
			gameobject.AddMember("use_priority_late_update", Value().SetBool(tObjectSerial.bUsePriority_LateUpdate), allocator);
			gameobject.AddMember("priority_render", Value().SetFloat(tObjectSerial.fPriority_Render), allocator);
			gameobject.AddMember("use_priority_render", Value().SetBool(tObjectSerial.bUsePriority_Render), allocator);


			Value gameobject_pos(kObjectType);
			gameobject_pos.AddMember("x", Value().SetFloat(tObjectSerial.vPos.x), allocator);
			gameobject_pos.AddMember("y", Value().SetFloat(tObjectSerial.vPos.y), allocator);
			gameobject_pos.AddMember("z", Value().SetFloat(tObjectSerial.vPos.z), allocator);
			gameobject.AddMember("pos", gameobject_pos, allocator);

			Value gameobject_rot(kObjectType);
			gameobject_rot.AddMember("x", Value().SetFloat(tObjectSerial.vRotation.x), allocator);
			gameobject_rot.AddMember("y", Value().SetFloat(tObjectSerial.vRotation.y), allocator);
			gameobject_rot.AddMember("z", Value().SetFloat(tObjectSerial.vRotation.z), allocator);
			gameobject.AddMember("rot", gameobject_rot, allocator);

			Value gameobject_scale(kObjectType);
			gameobject_scale.AddMember("x", Value().SetFloat(tObjectSerial.vScale.x), allocator);
			gameobject_scale.AddMember("y", Value().SetFloat(tObjectSerial.vScale.y), allocator);
			gameobject_scale.AddMember("z", Value().SetFloat(tObjectSerial.vScale.z), allocator);
			gameobject.AddMember("scale", gameobject_scale, allocator);

			gameobject.AddMember("group_key", Value().SetString(tObjectSerial.strGroupKey.c_str(), allocator), allocator);
			gameobject.AddMember("texture_key", Value().SetString(tObjectSerial.strTextureKey.c_str(), allocator), allocator);

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

_bool FSerialize_Scene::Receive_ByRapidJSON(string& strJSON, _bool bParseRewriteAble)
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

	if (doc.HasMember("header"))
	{
		const Value& header = doc["header"];
		header.HasMember("type") ? tHeader.eType = static_cast<ESERIALIZE_TYPE>(header["type"].GetInt()) : 0;
		header.HasMember("name") ? tHeader.strName = header["name"].GetString() : 0;
	}

	doc.HasMember("terrain") ? refTerrainName = doc["terrain"].GetString() : "";

	// 레이어 로드
	if (doc.HasMember("layers") && doc["layers"].IsArray())
	for (SizeType i = 0; i < doc["layers"].Size(); i++)
	{
		const Value& layer = doc["layers"][i];
		FSerialize_Layer layerSR;
		if (layer.HasMember("header"))
		{
			const Value& layer_header = layer["header"];
			layer_header.HasMember("type") ? layerSR.tHeader.eType = static_cast<ESERIALIZE_TYPE>(layer_header["type"].GetInt()) : 0;
			layer_header.HasMember("name") ? layerSR.tHeader.strName = layer_header["name"].GetString() : 0;
		}

		layer.HasMember("priority") ? layerSR.fPriority = layer["priority"].GetFloat() : 0.f;

		// 오브젝트 로드
		if (layer.HasMember("objects") && layer["objects"].IsArray())
		for (SizeType j = 0; j < layer["objects"].Size(); j++)
		{
			const Value& object = layer["objects"][j];
			FSerialize_GameObject gameobjectSR;

			if (object.HasMember("header"))
			{
				const Value& object_header = object["header"];
				gameobjectSR.tHeader.eType = static_cast<ESERIALIZE_TYPE>(RPJSON_RECIEVE_INT(object_header, "type"));
				gameobjectSR.tHeader.strName = RPJSON_RECIEVE_STRING(object_header, "name");
			}

			gameobjectSR.eID = static_cast<EGO_CLASS>(RPJSON_RECIEVE_INT(object, "ID"));
			gameobjectSR.strClassName = RPJSON_RECIEVE_STRING(object, "class_name");

			if (object.HasMember("pos"))
			{
				const Value& object_pos = object["pos"];
				gameobjectSR.vPos.x = RPJSON_RECIEVE_FLOAT(object_pos, "x");
				gameobjectSR.vPos.y = RPJSON_RECIEVE_FLOAT(object_pos, "y");
				gameobjectSR.vPos.z = RPJSON_RECIEVE_FLOAT(object_pos, "z");
			}

			if (object.HasMember("rot"))
			{
				const Value& object_rot = object["rot"];
				gameobjectSR.vRotation.x = RPJSON_RECIEVE_FLOAT(object_rot, "x");
				gameobjectSR.vRotation.y = RPJSON_RECIEVE_FLOAT(object_rot, "y");
				gameobjectSR.vRotation.z = RPJSON_RECIEVE_FLOAT(object_rot, "z");
			}

			if (object.HasMember("scale"))
			{
				const Value& object_scale = object["scale"];
				gameobjectSR.vScale.x = RPJSON_RECIEVE_FLOAT(object_scale, "x");
				gameobjectSR.vScale.y = RPJSON_RECIEVE_FLOAT(object_scale, "y");
				gameobjectSR.vScale.z = RPJSON_RECIEVE_FLOAT(object_scale, "z");
			}

			gameobjectSR.fPriority_Update = RPJSON_RECIEVE_FLOAT(object, "priority_update");
			gameobjectSR.fPriority_LateUpdate = RPJSON_RECIEVE_FLOAT(object, "priority_late_update");
			gameobjectSR.fPriority_Render = RPJSON_RECIEVE_FLOAT(object, "priority_render");

			gameobjectSR.bUsePriority_Update = RPJSON_RECIEVE_BOOL(object, "use_priority_update");
			gameobjectSR.bUsePriority_LateUpdate = RPJSON_RECIEVE_BOOL(object, "use_priority_late_update");
			gameobjectSR.bUsePriority_Render = RPJSON_RECIEVE_BOOL(object, "use_priority_render");

			gameobjectSR.strGroupKey = RPJSON_RECIEVE_STRING(object, "group_key");
			gameobjectSR.strTextureKey = RPJSON_RECIEVE_STRING(object, "texture_key");

			layerSR.vecGameObject.push_back(gameobjectSR);
		}
		vecLayer.push_back(layerSR);
	}

	return true;
}