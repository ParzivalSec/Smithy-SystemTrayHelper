#include "Config.h"

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <fstream>
#include <rapidjson/document.h>

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
	rapidjson::Value& UEHome = dom["UE4Home"];
	rapidjson::Value& P4Home = dom["P4Home"];
	rapidjson::Value& ProjectHome = dom["ProjectHome"];
	rapidjson::Value& LauncherHome = dom["LauncherHome"];

	if (UEHome.GetStringLength() != 0)
	{
		configResult.configFlags |= CONFIG_UE_EXISTS;
		configResult.UEHome = UEHome.GetString();
	}

	if (P4Home.GetStringLength() != 0)
	{
		configResult.configFlags |= CONFIG_P4_EXISTS;
		configResult.P4Home = P4Home.GetString();
	}

	if (ProjectHome.GetStringLength() != 0)
	{
		configResult.ProjectHome = ProjectHome.GetString();
	}

	if (LauncherHome.GetStringLength() != 0)
	{
		configResult.LauncherHome = LauncherHome.GetString();
	}
}