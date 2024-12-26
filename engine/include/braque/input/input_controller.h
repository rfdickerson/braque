#ifndef BRAQUE_INPUT_CONTROLLER_H_
#define BRAQUE_INPUT_CONTROLLER_H_

#include <glm/glm.hpp>
#include "event_controller.h"
#include "event.h"

#include <deque>

namespace braque {
class Window;

class InputController {
 public:
  void RegisterWindow(Window* window);
  void RegisterObserver(EventController* observer);
  void PollEvents();

 private:
  Window* window_ = nullptr;

  // create event queue
  std::deque<Event> events_;

  std::vector<EventController*> observers_;
};
}  // namespace braque

#endif  // BRAQUE_INPUT_CONTROLLER_H_