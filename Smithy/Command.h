#pragma once
#include <string>
#include <vector>
#include "Config.h"

namespace smithy
{
	namespace cmd
	{
		static const uint32_t MAX_CMDS = 20;
		static const char commandFile[] = "\\Config\\commands.json";

		struct Command
		{
			std::string name;
			std::string job;
			uint32_t cmdId;
			uint8_t category;
		};

		bool ParseCommandsFile(const std::string& configPath, std::vector<Command>& commands);
		void ParseCommands(const std::string& commandsContent, std::vector<Command>& commands);
		void SubstituteConfigParameters(const cfg::ConfigValues& settings, std::vector<Command>& commands);
		void ExecuteCommand(const Command& cmd);
	}
}