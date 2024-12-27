//
// Created by Robert F. Dickerson on 12/26/24.
//

#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include "event_controller.h"

namespace braque {
class Engine;
  class AppController: public EventController {
   public:
    AppController() = default;

    void SetEngine(Engine* engine) { engine_ = engine; }

    void OnKeyPressed(int key) override;

    void OnEvent(Event event) override;

   private:
    Engine *engine_ = nullptr;
  };
}

#endif //APP_CONTROLLER_H
