#pragma once
#include <string>

namespace smithy
{
	namespace cfg
	{
		class ConfigSettingString
		{
		public: 
			ConfigSettingString(std::string name, std::string value);
			ConfigSettingString& operator=(std::string value);
			inline operator std::string(void) const;
			inline std::string& GetName(void) { return m_name; }
			inline std::string& GetValue(void) { return m_value; }

			static ConfigSettingString* FindSetting(std::string name);
			
		private:
			ConfigSettingString* m_next;
			std::string m_name;
			std::string m_value;
		};

		enum CONFIG_STATE
		{
			INVALID_CONFIG =	0x0,
			CONFIG_EXISTS =		0x1,
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

namespace
{
	smithy::cfg::ConfigSettingString* head = nullptr;
	smithy::cfg::ConfigSettingString* tail = nullptr;
}