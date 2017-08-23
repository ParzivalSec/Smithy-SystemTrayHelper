#include <windows.h>
#include <tchar.h>

#include "resource.h"
#include "Config.h"
#include "Command.h"
#include "ContextMenu.h"

UINT WM_TASKBAR = 0;
HWND Hwnd;
HMENU Hmenu;
NOTIFYICONDATA notifyIconData;

char szClassName[] = "Smithy System Tray App";
TCHAR szTIP[64] = TEXT("Smithy - Titan Twins Tooling Helper");


LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void minimize();
void restore();
void InitNotifyIconData();
std::vector<smithy::cmd::Command> commands;


int WINAPI WinMain(HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpszArgument,
	int nCmdShow)
{
	MSG messages;
	WNDCLASSEX wincl;
	WM_TASKBAR = RegisterWindowMessageA("SmithyCreated");
	/** Window structure **/
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof(WNDCLASSEX);

	/** Use default icon and mouse pointer **/
	wincl.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
	wincl.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL; /* No menu */
	wincl.cbClsExtra = 0; /* No extra bytes after window class */
	wincl.cbWndExtra = 0; /* structure or the window instance */
	wincl.hbrBackground = static_cast<HBRUSH>(CreateSolidBrush(RGB(255, 255, 255))); // was c-style cast in tutorial, check gere if error occurs

	// Register the class and exit progrema if it fails
	if (!RegisterClassEx(&wincl))
	{
		return 0;
	}

	// Check for main config file and load application homes if exist
	smithy::cfg::ConfigValues settings = smithy::cfg::CheckConfigurationFile();
	if (settings.configFlags & smithy::cfg::CONFIG_EXISTS)
	{
		if (smithy::cmd::ParseCommandsFile(settings.configPath, commands))
		{
			// Now we have all commands loaded and create the menus
			SubstituteConfigParameters(settings, commands);
		}
	}
	else
	{
		// Notify the user and exit the application if config file is missing
		std::string msg = "Configfile " + settings.configPath + " is missing!";
		MessageBox(Hwnd, TEXT(msg.c_str()), NULL, MB_OK);
		return 0;
	}

	/* The class is registered, let;s create the program */
	Hwnd = CreateWindowEx(
		0,						// Extended possibilities for variation
		szClassName,			// ClassName
		szClassName,			// Title text
		WS_OVERLAPPEDWINDOW,	// Default window
		CW_USEDEFAULT,			// Windows decide the position
		CW_USEDEFAULT,			// where the window ends up in the screen
		544,					// The programs witdth
		375,					// and height in pixels
		HWND_DESKTOP,			// Window is a child-window to desktop
		NULL,					// No menu
		hThisInstance,			// Program instance handler
		NULL					// No window creation data
	);

	// Init the NOTIFYICONDATA structure only once
	InitNotifyIconData();
	// Make window visible
	ShowWindow(Hwnd, nCmdShow);

	// Message pump
	while (GetMessage(&messages, NULL, 0, 0))
	{
		// Translate virtual key msgs into character messages
		TranslateMessage(&messages);
		// Send messages to WindowProcedure
		DispatchMessage(&messages);
	}

	return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_TASKBAR && !IsWindowVisible(Hwnd))
	{
		minimize();
		return 0;
	}

	switch (message)
	{
		case WM_ACTIVATE:
			ShowWindow(Hwnd, SW_HIDE);
			Shell_NotifyIcon(NIM_ADD, &notifyIconData);
			break;
		case WM_CREATE:
			ShowWindow(Hwnd, SW_HIDE);
			Hmenu = smithy::CreateContextMenu(commands);
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit the demo"));
			break;
		case WM_SYSCOMMAND:
			switch (wParam & 0xFFF0)
			{
				case SC_MINIMIZE:
				case SC_CLOSE:
					minimize();
					return 0;
			}
			break;

		case WM_SYSICON:
		{
			switch (wParam)
			{
				case ID_TRAY_APP_ICON:
					SetForegroundWindow(Hwnd);
					break;
			}

			if (lParam == WM_RBUTTONDOWN || lParam == WM_LBUTTONDOWN)
			{
				// Get current mouse position
				POINT curPoint;
				GetCursorPos(&curPoint);
				SetForegroundWindow(Hwnd);

				// Track popup window blocks the app until TrackPopupMenu returns
				UINT clicked = TrackPopupMenu(Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hwnd, NULL);

				SendMessage(hwnd, WM_NULL, 0, 0); // Send begin message to window to make sure menu goes away
				
				for (const smithy::cmd::Command& cmd : commands)
				{
					// INFO: If the id matches, execute the commands job
					if (cmd.cmdId == clicked)
					{
						smithy::cmd::ExecuteCommand(cmd);
					}
				}
				
				if (clicked == ID_TRAY_EXIT)
				{
					Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
					PostQuitMessage(0);
				}
			}
			break;
		}

		case WM_NCHITTEST:
		{
			UINT uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
			if (uHitTest == HTCLIENT)
			{
				return HTCAPTION;
			}
			else
			{
				return uHitTest;
			}
		}

		case WM_CLOSE:
			minimize();
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}


	return DefWindowProc(hwnd, message, wParam, lParam);
}

void InitNotifyIconData()
{
	memset(&notifyIconData, 0, sizeof(NOTIFYICONDATA));

	notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	notifyIconData.hWnd = Hwnd;
	notifyIconData.uID = ID_TRAY_APP_ICON;
	notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	notifyIconData.uCallbackMessage = WM_SYSICON; // Setup our invented Windows Message
	notifyIconData.hIcon = static_cast<HICON>(LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1)));

	strncpy_s(notifyIconData.szTip, szTIP, sizeof(szTIP));
}

void minimize()
{
	ShowWindow(Hwnd, SW_HIDE);
}

void restore()
{
	ShowWindow(Hwnd, SW_SHOW);
}