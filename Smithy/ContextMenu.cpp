#include "ContextMenu.h"
#include <algorithm>
#include <winuser.h>

HMENU smithy::CreateContextMenu(void)
{
	HMENU menu = CreatePopupMenu();
	return menu;
}

void smithy::AddMenuItems(cfg::ConfigValues& settings, HMENU menu, const std::vector<cmd::Command>& commands)
{
	std::string currentCategory = "";
	HMENU SubMenu = menu;
	for (const smithy::cmd::Command& cmd : commands)
	{
		if (cmd.category == "")
		{
			AppendMenu(menu, MF_STRING, cmd.cmdId, cmd.name.c_str());
			return;
		}

		if (strcmp(currentCategory.c_str(), cmd.category.c_str()) != 0)
		{
			SubMenu = CreateMenu();
			AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(SubMenu), cmd.category.c_str());

			// Check if there is an icon for the category, if not do nothing
			std::string iconFile = settings.configPath + "\\Config\\icons\\" + cmd.category + ".bmp";
			HBITMAP hNewIcon = static_cast<HBITMAP>(LoadImage(GetModuleHandle(nullptr), iconFile.c_str(), IMAGE_BITMAP, 16, 16,
				LR_LOADFROMFILE | LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS));

			uint32_t err = GetLastError();

			if (hNewIcon != nullptr)
			{
				MENUITEMINFO mii;
				mii.cbSize = sizeof(MENUITEMINFO);
				mii.fMask = MIIM_BITMAP;
				mii.hbmpItem = hNewIcon;
				SetMenuItemInfo(menu, reinterpret_cast<UINT_PTR>(SubMenu), 0, &mii);
			}

			currentCategory = cmd.category;
		}
		AppendMenu(SubMenu, MF_STRING, cmd.cmdId, cmd.name.c_str());
	}
}