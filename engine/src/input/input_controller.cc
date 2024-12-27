#include <braque/input/input_controller.h>
#include <braque/window.h>

#include <spdlog/spdlog.h>

namespace braque {

void InputController::RegisterWindow(Window* window) {
  window_ = window;
}

void InputController::RegisterObserver(EventController* observer) {
  observers_.push_back(observer);
}

void InputController::PollEvents() {
  if (window_ == nullptr) {
    return;
  }

  Event event;
  window_->PollEvents();

  // get the events from the window
  auto mouseChange = window_->GetMouseChange();

  event.type = EventType::MouseMoved;
  event.mouse_position_x = mouseChange.x;
  event.mouse_position_y = mouseChange.y;
  events_.push_back(event);

  for (const auto& key : window_->GetPressedKeys()) {
    event.type = EventType::KeyPressed;
    event.key = key;
    events_.push_back(event);
  }

  if (window_->ShouldClose()) {
    event.type = EventType::AppQuit;
    events_.push_back(event);
  }

  for (const auto& observer : observers_) {
    for (const auto& event : events_) {
      switch (event.type) {
        case EventType::MouseMoved:
          observer->OnMouseMoved(event.mouse_position_x, event.mouse_position_y);
          break;
        case EventType::KeyPressed:
          observer->OnKeyPressed(event.key);
          break;
        case EventType::AppQuit:
          observer->OnEvent(event);
          break;
        default:
          spdlog::warn("Could not handle event type");
      }
    }
  }

  events_.clear();
}

}  // namespace braque