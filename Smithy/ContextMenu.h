#pragma once
#include <windows.h>

#include "Command.h"

namespace smithy
{
	HMENU CreateContextMenu(const std::vector<cmd::Command>& commands);
}