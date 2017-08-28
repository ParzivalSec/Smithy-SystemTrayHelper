#include "ContextMenu.h"
#include <algorithm>

HMENU smithy::CreateContextMenu(void)
{
	HMENU menu = CreatePopupMenu();
	return menu;
}

void smithy::AddMenuItems(HMENU menu, const std::vector<cmd::Command>& commands)
{
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
}