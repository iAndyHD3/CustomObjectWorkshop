#pragma once

#include <json.hpp>
#include <string>

struct CustomObjectAuthor
{
	int id = 0;
	std::string name{};
};

struct CustomObjectData
{
	int object_id = 0;
	std::string name{};
	std::string desc{};
	std::string format{};
	int objects = 0;
	int views	= 0;
	std::string object_string{};
	CustomObjectAuthor author{};
	bool local = false;
};

// clang-format off

template <>
struct json::Serialize<CustomObjectAuthor>
{
	static CustomObjectAuthor from_json(const json::Value& value)
	{
		return CustomObjectAuthor
		{
			.id   = value["id"]  .as_int(),
			.name = value["name"].as_string()
		};
	}
};

template <>
struct json::Serialize<CustomObjectData>
{
	static CustomObjectData from_json(const json::Value& value)
	{
		return CustomObjectData
		{
			.object_id = value["id"].as_int(),
			.name	   = value["name"].as_string(),
			.desc	   = value["description"].as_string(),
			.format	   = value["format"].as_string(),
			.objects   = value["objects"].is_number() ? value["objects"].as_int() : 0,
			.views	   = value["views"].as_int(),
			.object_string = value["object_string"].as_string(),
			.author    = value["author"].as<CustomObjectAuthor>()
		};
	}
};
