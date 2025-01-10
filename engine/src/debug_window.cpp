//
// Created by Robert F. Dickerson on 12/15/24.
//

#include "braque/debug_window.h"

#include "braque/engine.h"
#include "braque/renderer.h"
#include "braque/rendering_stage.h"
#include "braque/swapchain.h"
#include "braque/window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "spdlog/spdlog.h"

namespace braque
{

  DebugWindow::DebugWindow( Engine & engine ) : engine( engine )
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    //const auto format = engine.getSwapchain().getFormat();
    constexpr auto format = vk::Format::eR16G16B16A16Sfloat;
    constexpr auto depthFormat = vk::Format::eD32Sfloat;

    vk::PipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo;

    pipelineRenderingCreateInfo.setColorAttachmentFormats({format});
    //pipelineRenderingCreateInfo.setDepthAttachmentFormat(depthFormat);

    ImGui_ImplGlfw_InitForVulkan( engine.getWindow().GetNativeWindow(), true );

    const auto & renderer = engine.getRenderer();

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance                    = renderer.getInstance();
    initInfo.PhysicalDevice              = renderer.getPhysicalDevice();
    initInfo.Device                      = renderer.getDevice();
    initInfo.QueueFamily                 = renderer.getGraphicsQueueFamilyIndex();
    initInfo.Queue                       = renderer.getGraphicsQueue();
    initInfo.PipelineCache               = nullptr;
    initInfo.DescriptorPool              = engine.getRenderingStage().getDescriptorPool();
    initInfo.Allocator                   = nullptr;
    initInfo.MinImageCount               = 2;
    initInfo.ImageCount                  = engine.getSwapchain().getImageCount();
    initInfo.MSAASamples                 = VK_SAMPLE_COUNT_1_BIT;
    initInfo.CheckVkResultFn             = nullptr;
    initInfo.UseDynamicRendering         = true;
    initInfo.PipelineRenderingCreateInfo = pipelineRenderingCreateInfo;

    ImGui_ImplVulkan_Init( &initInfo );
    spdlog::info( "Initialized ImGui" );

    initAssets();
    spdlog::info( "Set up ImGUI assets" );
  }

  DebugWindow::~DebugWindow()
  {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void DebugWindow::createFrame(FrameStats& stats) const
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // ImGui::ShowDemoWindow();

    // show memory stats
    const auto report = engine.getMemoryAllocator().getReport();
    ImGui::Begin( "Engine" );

    ImGui::PlotLines( "Frame Latencies",
      stats.LatencyData().data(),
      stats.LatencyData().size(),
      stats.LatencyDataOffset(),
      nullptr, 0.0F, 0.1F, ImVec2( 0, 80 ) );

    ImGui::Text( "Allocations: %d", report.allocations );
    ImGui::Text( "Total memory: %llu", report.totalMemory );
    ImGui::Text( "Used memory: %llu", report.usedMemory );
    ImGui::Separator();
    ImGui::End();
  }

  void DebugWindow::renderFrame( const vk::CommandBuffer & commandBuffer )
  {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData( ImGui::GetDrawData(), commandBuffer );
  }

  void DebugWindow::initAssets()
  {
    ImGui_ImplVulkan_CreateFontsTexture();
  }

void DebugWindow::BeginRendering(vk::CommandBuffer buffer, const Image& image) const {

    constexpr vk::ClearColorValue clearColor{0.0F, 0.0F, 0.0F, 0.0F};

    vk::RenderingAttachmentInfo renderingAttachmentInfo{};
    renderingAttachmentInfo.setClearValue(clearColor);
    renderingAttachmentInfo.setLoadOp(vk::AttachmentLoadOp::eLoad);
    renderingAttachmentInfo.setStoreOp(vk::AttachmentStoreOp::eStore);
    renderingAttachmentInfo.setImageLayout(
        vk::ImageLayout::eColorAttachmentOptimal);
    renderingAttachmentInfo.setImageView(image.GetImageView());

    auto extent = image.GetExtent();
    const auto renderArea = vk::Rect2D{{0, 0}, {extent.width, extent.height}};

    vk::RenderingInfo renderingInfo{};
    renderingInfo.setFlags(vk::RenderingFlags());
    renderingInfo.setLayerCount(1);
    renderingInfo.setColorAttachments(renderingAttachmentInfo);
    renderingInfo.setRenderArea(renderArea);

    buffer.beginRenderingKHR(renderingInfo);

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData( ImGui::GetDrawData(), buffer );

    buffer.endRendering();


}

}  // namespace braque