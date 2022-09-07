#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>

/* Pushes all visible windows into a vector whose address is specified
   in <vec_addr>. */
BOOL CALLBACK PushActiveWindows(HWND hwnd, LPARAM vec_addr)
{
	if (IsWindowVisible(hwnd))
		reinterpret_cast<std::vector<HWND>*>(vec_addr)->push_back(hwnd);

	return TRUE;
}

/* Pushes all (including hidden and system windows) into a vector whose
   address is specified in <vec_addr>. */
BOOL CALLBACK PushAllWindows(HWND hwnd, LPARAM vec_addr)
{
	reinterpret_cast<std::vector<HWND>*>(vec_addr)->push_back(hwnd);

	return TRUE;
}