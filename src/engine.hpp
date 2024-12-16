//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef ENGINE_HPP
#define ENGINE_HPP

namespace braque {

// forward declarations
class Renderer;
class Window;
class Swapchain;
class RenderingStage;
class DebugWindow;

class Engine {
public:
    Engine();
    ~Engine();

    Renderer* getRenderer() const { return renderer; }
    Window* getWindow() const { return window; }
    Swapchain* getSwapchain() const { return swapchain; }
    RenderingStage* getRenderingStage() const { return renderingStage; }

    void run();

private:
    Window *window;
    Renderer *renderer;
    Swapchain *swapchain;
    RenderingStage *renderingStage;
    DebugWindow *debugWindow;
};

} // braque

#endif //ENGINE_HPP
