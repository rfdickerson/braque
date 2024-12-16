//
// Created by Robert F. Dickerson on 12/15/24.
//

#include "debug_window.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "spdlog/spdlog.h"

#include "engine.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "swapchain.hpp"
#include "rendering_stage.hpp"

namespace braque {

DebugWindow::DebugWindow(Engine& engine): engine(engine) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    auto format = engine.getSwapchain()->getFormat();

    vk::PipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo;
    pipelineRenderingCreateInfo.setColorAttachmentCount(1);
    pipelineRenderingCreateInfo.setPColorAttachmentFormats(&format);

    ImGui_ImplGlfw_InitForVulkan(engine.getWindow()->getNativeWindow(), true);

    const auto renderer = engine.getRenderer();

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = renderer->getInstance();
    initInfo.PhysicalDevice = renderer->getPhysicalDevice();
    initInfo.Device = renderer->getDevice();
    initInfo.QueueFamily = renderer->getGraphicsQueueFamilyIndex();
    initInfo.Queue = renderer->getGraphicsQueue();
    initInfo.PipelineCache = nullptr;
    initInfo.DescriptorPool = engine.getRenderingStage()->getDescriptorPool();
    initInfo.Allocator = nullptr;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = engine.getSwapchain()->getImageCount();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.CheckVkResultFn = nullptr;
    initInfo.UseDynamicRendering = true;
    initInfo.PipelineRenderingCreateInfo = pipelineRenderingCreateInfo;

    ImGui_ImplVulkan_Init(&initInfo);
    spdlog::info("Initialized ImGui");

    initAssets();
    spdlog::info("Set up ImGUI assets");
}

DebugWindow::~DebugWindow() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DebugWindow::createFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
}

void DebugWindow::renderFrame(vk::CommandBuffer commandBuffer) {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

void DebugWindow::initAssets() {
    ImGui_ImplVulkan_CreateFontsTexture();
}

} // braque