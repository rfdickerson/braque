//
// Created by Robert F. Dickerson on 12/26/24.
//

#include <braque/engine.h>
#include <braque/input/app_controller.h>

#include <GLFW/glfw3.h>

namespace braque {

void AppController::OnKeyPressed(int key) {
  if (engine_ == nullptr) {
    return;
  }

  if (key == GLFW_KEY_ESCAPE) {
    engine_->Quit();
  }

  if (key == GLFW_KEY_F) {
    engine_->getWindow().ShowCursor();
  }

  if (key == GLFW_KEY_G) {
    engine_->getWindow().HideCursor();
  }
}

void AppController::OnEvent(Event event) {
  if (event.type == EventType::AppQuit) {
    engine_->Quit();
  }
}

}