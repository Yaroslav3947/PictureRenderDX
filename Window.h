#pragma once

#include "WinInclude.h"
//#include "App.h"


class Window {
 public:
  Window();
  HWND getHandle() { return m_hwnd; }

 private:
  void CreateWnd();

 private:
  HWND m_hwnd;
};