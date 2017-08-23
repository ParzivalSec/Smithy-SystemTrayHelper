#pragma once
#include <string>

namespace smithy
{
	namespace cfg
	{
		enum CONFIG_STATE
		{
			INVALID_CONFIG =	0x0,
			CONFIG_EXISTS =		0x1,
			CONFIG_UE_EXISTS =	0x2,
			CONFIG_P4_EXISTS =	0x4,
		};

		struct ConfigValues
		{
			std::string configPath;
			std::string UEHome;
			std::string P4Home;
			std::string ProjectHome;
			std::string LauncherHome;
			int configFlags;
		};

		const char configFile[] = "\\Config\\smithy_config.json";

		ConfigValues CheckConfigurationFile();
		void ParseConfig(const std::string& config, ConfigValues& configResult);
	}
}