#include "Command.h"
#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <rapidjson/document.h>

#include "resource.h"

bool smithy::cmd::ParseCommandsFile(const std::string& configPath, std::vector<Command>& commands)
{
	std::string commandFilePath = configPath + commandFile;

	DWORD dwAttrib = GetFileAttributes(commandFilePath.c_str());

	if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
	{
		// Read the file into memory
		std::string line;
		std::string commandsContent;
		std::ifstream commandsFileStream(commandFilePath.c_str());

		if (commandsFileStream.is_open())
		{
			while (getline(commandsFileStream, line))
			{
				commandsContent += line;
			}
			commandsFileStream.close();
		}

		ParseCommands(commandsContent, commands);
		return true;
	}

	return false;
}

void smithy::cmd::ParseCommands(const std::string& commandsContent, std::vector<Command>& commands)
{
	// Parse the json DOM
	rapidjson::Document dom;
	dom.Parse(commandsContent.c_str());
	rapidjson::Value& commandsArray = dom["commands"];

	for (size_t i = 0; i < dom["commands"].Size(); i++)
	{
		assert(i < MAX_CMDS && "Number of supplied commands exceed allowed number of cmds (20)");
		// ID_TRAY_EXIT + (i + 1) creates a new menu uid for every command specified in the config, needs to be unique
		commands.push_back({ dom["commands"][i]["name"].GetString(),
			dom["commands"][i]["job"].GetString(),
			ID_TRAY_EXIT + (i + 1), 
			static_cast<uint8_t>(dom["commands"][i]["category"].GetInt())
		});
	}

	std::sort(commands.begin(), commands.end(), [](const smithy::cmd::Command& lhs, const smithy::cmd::Command& rhs) {
		return lhs.category < rhs.category;
	});
}

void InPlaceStringReplace(std::string& subject, const std::string& token, const std::string& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(token, pos)) != std::string::npos)
	{
		subject.replace(pos, token.length(), replace);
		pos += replace.length();
	}
}

void smithy::cmd::SubstituteConfigParameters(const cfg::ConfigValues& settings, std::vector<Command>& commands)
{
	for (Command& cmd : commands)
	{
		// TODO [LV] Make this more dynamic and less hardcoded with globally accessible config values
		InPlaceStringReplace(cmd.job, "%UEHOME%", settings.UEHome);
		InPlaceStringReplace(cmd.job, "%P4HOME%", settings.P4Home);
		InPlaceStringReplace(cmd.job, "%PROJECT%", settings.ProjectHome);
		InPlaceStringReplace(cmd.job, "%LAUNCHER%", settings.LauncherHome);
	}
}

void smithy::cmd::ExecuteCommand(const Command& cmd)
{
	// Start process
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(NULL,
		(LPSTR)cmd.job.c_str(),
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi)
		)
	{
		std::string a = "LastError: " + GetLastError();
		OutputDebugString(a.c_str());
		PostQuitMessage(0);
	}

	// TODO [LV] Start the job in another thread
	//WaitForSingleObject(pi.hProcess, INFINITE);
	//CloseHandle(pi.hProcess);
	//CloseHandle(pi.hThread);
}