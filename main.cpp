#include "Window.h"
#include "Renderer.h"
#include "Picture.h"
#include "App.h"


int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance,
                     LPSTR cmdLine, int cmdCount) {

  std::unique_ptr<Application> app = std::make_unique<Application>();
  
  return 0;
}
