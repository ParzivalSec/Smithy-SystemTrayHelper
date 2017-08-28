#pragma once
#include <windows.h>

#include "Command.h"

namespace smithy
{
	HMENU CreateContextMenu(void);
	void AddMenuItems(HMENU menu, const std::vector<cmd::Command>& commands);
}