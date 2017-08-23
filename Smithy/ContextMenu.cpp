#include "ContextMenu.h"
#include <algorithm>

HMENU smithy::CreateContextMenu(const std::vector<smithy::cmd::Command>& commands)
{
	HMENU menu = CreatePopupMenu();
	uint8_t currentCategory = commands[0].category;
	for (const smithy::cmd::Command& cmd : commands)
	{
		if (currentCategory != cmd.category)
		{
			AppendMenu(menu, MF_SEPARATOR, NULL, NULL);
			currentCategory = cmd.category;
		}
		AppendMenu(menu, MF_STRING, cmd.cmdId, cmd.name.c_str());
	}

	return menu;
}