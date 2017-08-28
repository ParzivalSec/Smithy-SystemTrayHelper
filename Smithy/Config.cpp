#include "Config.h"

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <fstream>
#include <rapidjson/document.h>
#include <iostream>

smithy::cfg::ConfigValues smithy::cfg::CheckConfigurationFile()
{
	ConfigValues configResult;
	configResult.configFlags = INVALID_CONFIG;
	// Check if the config file exists, if not return -1
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string buf(buffer);
	std::string::size_type pos = buf.find_last_of("\\/");
	buf = buf.substr(0, pos);
	configResult.configPath = buf;
	buf += configFile;

	DWORD dwAttrib = GetFileAttributes(buf.c_str());

	if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		configResult.configFlags = CONFIG_EXISTS;
		// Read the file into memory
		std::string line;
		std::string configContent;
		std::ifstream configFileStream(buf.c_str());

		if (configFileStream.is_open())
		{
			while (getline(configFileStream, line))
			{
				configContent += line;
			}
			configFileStream.close();
		}

		ParseConfig(configContent, configResult);
	}

	return configResult;
}

void smithy::cfg::ParseConfig(const std::string& config, ConfigValues& configResult)
{
	// Parse the json DOM
	rapidjson::Document dom;
	dom.Parse(config.c_str());

	for (auto& m : dom.GetObject())
	{
		// TODO [LV]: Remove these dynamic allocations and create them in the scope of an enclosing ConfigSettings clas
		ConfigSettingString* cVar = new ConfigSettingString(m.name.GetString(), m.value.GetString());
	}
} 


smithy::cfg::ConfigSettingString::ConfigSettingString(std::string name, std::string value)
	: m_next(nullptr)
	, m_name(name)
	, m_value(value)
{
	if (head)
	{
		tail->m_next = this;
		tail = this;
	}
	else
	{
		head = this;
		tail = this;
	}
}

smithy::cfg::ConfigSettingString& smithy::cfg::ConfigSettingString::operator=(std::string value)
{
	m_value = value;
	return *this;
}

inline smithy::cfg::ConfigSettingString::operator std::string(void) const
{
	return m_value;
}

smithy::cfg::ConfigSettingString* smithy::cfg::ConfigSettingString::FindSetting(std::string name)
{
	smithy::cfg::ConfigSettingString* tmp = head;
	while (tmp != nullptr)
	{
		if (tmp->m_name == name)
		{
			return tmp;
		}

		tmp = tmp->m_next;
	}
	return nullptr;
}
