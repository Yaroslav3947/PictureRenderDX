#include "D2D/DXHelper.h"
#include "VideoPlayer.h"
#include "Window.h"

int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance,
                     LPSTR cmdLine, int cmdCount) {

  if (Window::Get().Init()) {
    Window::Get().SetFullscreen(true);

    HWND hwnd = Window::Get().GetWindow();

    VideoPlayer& videoPlayer = VideoPlayer::Get();
    videoPlayer.Initialize(hwnd);

    const WCHAR* filePath = L"Resources/Videos/video.mp4";

    videoPlayer.OpenURL(filePath);

    while (!Window::Get().ShouldClose()) {
      Window::Get().Update();

      // Handle resizing
      if (Window::Get().ShouldResize()) {
        Window::Get().Resize();
      }
      //Window::Get().Preset();
    }

    Window::Get().Shutdown();
  }

  return 0;
}
