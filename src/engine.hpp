//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef ENGINE_HPP
#define ENGINE_HPP

namespace braque {

class Window;
class Renderer;
class Swapchain;

class Engine {
public:
    Engine();
    ~Engine();

    void run();

private:
    Window *window;
    Renderer *renderer;
    Swapchain *swapchain;
};

} // braque

#endif //ENGINE_HPP
