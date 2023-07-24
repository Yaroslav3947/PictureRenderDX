#pragma once

#include "WinInclude.h"


class Window {
 public:
  Window(int width, int height);
  HWND getHandle() { return m_hwnd; }

 private:
  void CreateWnd(int width, int height);

 private:
  HWND m_hwnd;
};