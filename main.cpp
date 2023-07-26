#include "Window.h"
#include "Renderer.h"
#include "Picture.h"
#include "App.h"
#include "VideoPlayer.h"


int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance,
                     LPSTR cmdLine, int cmdCount) {

  //std::unique_ptr<Application> app = std::make_unique<Application>();
  VideoPlayer *pVideoPlayer = nullptr;

  Window window;
  
  HWND hwnd = window.getHandle();

  HRESULT hr = VideoPlayer::CreateInstance(hwnd, &pVideoPlayer);
  const WCHAR* filePath = L"C:/Users/Yaroslav/Desktop/video.mp4";

  pVideoPlayer->OpenURL(filePath);
  
  MSG msg = {0};
  while (true) {
    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if (msg.message == WM_QUIT) {
        break;
      }
    }
  }

  delete pVideoPlayer;

  return 0;
}
