// ConsoleTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

using namespace std;

void DisplayAvailableColors();
void DisplayCurrentFontDetails(HANDLE);
void PrintError(string);

int main()
{
	// Set the Console's codepage to 65001 (UTF-8)
	HANDLE hConsole{ GetStdHandle(STD_OUTPUT_HANDLE) };
	DWORD dwMode{ 0 };
	if (GetConsoleMode(hConsole, &dwMode))
	{
		if (SetConsoleMode(hConsole, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
		{
			cout << VT_RED << "Console Tool" << VT_RESET << endl;

			DWORD dwNewMode{};
			GetConsoleMode(hConsole, &dwNewMode);
			if (ENABLE_VIRTUAL_TERMINAL_PROCESSING && dwMode)
			{
				DisplayAvailableColors();
			}
			else
			{
				PrintError("Can't display ANSI/VT-Sequences - you may need to upgrade to a recent Windows 10 build");
			}
		

			UINT nCodepage = GetConsoleOutputCP();
			if (SetConsoleOutputCP(65001))
			{
				DisplayCurrentFontDetails(hConsole);

				// Restore Console codepage
				SetConsoleOutputCP(nCodepage);
			}
			else
			{
				cout << "Error: Could not set codepage to UTF-8 (65001)" << endl;
			}

			//	Restore Console output mode
			SetConsoleMode(hConsole, dwMode);
		}
		else
		{
			cout << "Error: Could not enable VT Processing - please update to a recent Windows 10 build";
		}
	}

	return EXIT_SUCCESS;
}

void DisplayAvailableColors()
{
	cout << "Available Colors:" << endl;
	for (short c = 0; c < VT_MAX_COLOR; c++)
	{
		if (c > 0) { cout << "\x1b[" << to_string(30 + c) << 'm'; }
		cout << "0x" << hex << c << dec << ": " << VT_COLOR_NAMES[c] << VT_RESET << endl;
	}
	cout << endl;
}

void DisplayCurrentFontDetails(HANDLE hConsole)
{
	CONSOLE_FONT_INFOEX fontInfo = { sizeof(CONSOLE_FONT_INFOEX) };

	cout << "Current Font:" << endl;
	
	if (GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo))
	{
		wcout << "Font face: " << fontInfo.FaceName << endl;
		cout << "Font weight: " << fontInfo.FontWeight << " (Normal = 400)" << endl;
		cout << "Font size: [" << fontInfo.dwFontSize.X << " x " << fontInfo.dwFontSize.Y << ']' << endl;
	}
	else
	{
		PrintError("Cannot find current font info!");
		GetLastError();
	}
}

void PrintError(string msg)
{
	HANDLE hConsole{ GetStdHandle(STD_OUTPUT_HANDLE) };
	DWORD dwNewMode{};
	GetConsoleMode(hConsole, &dwNewMode);
	bool fVTEnabled = (ENABLE_VIRTUAL_TERMINAL_PROCESSING && dwNewMode);

	if (fVTEnabled) 
	{
		cout << VT_RED;
	}

	cout << "Error: " << msg << endl;

	LPTSTR lpMessage{};

	DWORD dwErrCode = GetLastError();

	DWORD nChars = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, // no source buffer needed
		dwErrCode, // error code for this message
		NULL, // default language ID
		(LPTSTR)&lpMessage, // allocated by fcn
		NULL, // minimum size of buffer
		NULL); // no inserts

	wcout << lpMessage;
	
	if (fVTEnabled)
	{
		cout << VT_RESET;
	}

	cout << endl;

	LocalFree(lpMessage); 

}
