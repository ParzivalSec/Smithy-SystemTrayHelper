#pragma once
#include <windows.h>

#include "Command.h"

namespace smithy
{
	HMENU CreateContextMenu(void);
	void AddMenuItems(cfg::ConfigValues& settings, HMENU menu, const std::vector<cmd::Command>& commands);
}