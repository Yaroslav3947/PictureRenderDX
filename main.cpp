#include "D2D/DXHelper.h"
#include "Picture.h"
#include "VideoPlayer.h"
#include "Window.h"

int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance,
                     LPSTR cmdLine, int cmdCount) {
  VideoPlayer* videoPlayer = nullptr;

  if (Window::Get().Init()) {
    std::unique_ptr<DXHelper> dxhelper = std::make_unique<DXHelper>();

    Window::Get().SetFullscreen(true);
    HWND hwnd = Window::Get().GetWindow();
    videoPlayer->CreateInstance(hwnd, &videoPlayer);

    const WCHAR* filePath = L"Resources/Videos/video.mp4";

    videoPlayer->OpenURL(filePath);

    while (!Window::Get().ShouldClose()) {
      Window::Get().Update();

      // Handle resizing
      if (Window::Get().ShouldResize()) {
        Window::Get().Resize();
      }
    }
    delete videoPlayer;

    Window::Get().Shutdown();
  }

  return 0;
}
