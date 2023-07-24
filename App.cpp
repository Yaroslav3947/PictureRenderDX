#include "App.h"

Application::Application() { Run(); }

Application::~Application() {}

void Application::Run() {
  std::unique_ptr<Window> window = std::make_unique<Window>();
  HWND hwnd = window->getHandle();
  std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>(hwnd);
  std::unique_ptr<Picture> picture = std::make_unique<Picture>(*renderer);

  MSG msg = {0};
  while (true) {
    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if (msg.message == WM_QUIT) {
        break;
      }
    }



    renderer->beginFrame();

    picture->draw(*renderer);

    renderer->endFrame();
    
  }
}

