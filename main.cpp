#include "Window.h"
#include "Renderer.h"
#include "Picture.h"
#include "VideoPlayer.h"


int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance,
                     LPSTR cmdLine, int cmdCount) {

  if (Window::Get().Init()) {
    while (!Window::Get().ShouldClose()) {
      Window::Get().Update();

      // Handle resizing
      if (Window::Get().ShouldResize()) {
        Window::Get().Resize();
      }

      Window::Get().BeginFrame();

      Window::Get().EndFrame();
    }
    Window::Get().Shutdown();
  }

  return 0;
}
