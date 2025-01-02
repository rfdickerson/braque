#include <braque/image_loader.h>

#include <spdlog/spdlog.h>
#include <gli/gli.hpp>

namespace braque
{

Texture::Texture(std::string name, TextureType texture_type, std::string path)
  : name_(name), texture_type_(texture_type), path_(path)
{
  texture_ = gli::load_dds(path);
  if (texture_.empty()) {
    spdlog::error("Failed to load texture: {}", path);
    throw std::runtime_error("Failed to load texture: " + path);
  }

  size = texture_.size();

  spdlog::info("Texture size: {}", size);
  data = new unsigned char[size];
  memcpy(data, texture_.data(), size);

  spdlog::info("Loaded texture: {}", path);

  // show size and number of mipmaps
  spdlog::info("Texture size: {} x {} x {}", texture_.extent().x, texture_.extent().y, texture_.extent().z);

  spdlog::info("Texture mipmaps: {}", texture_.levels());
}

Texture::Texture(Texture&& other) noexcept:
texture_(other.texture_)
{
  other.data = nullptr;
}


Texture::~Texture() {
  if (data == nullptr) {
    delete [] data;
  }

  data = nullptr;
}

void Texture::CreateImage(Engine& engine) {
  Buffer staging_buffer(engine, BufferType::staging, texture_.size());

  staging_buffer.CopyData(texture_.data(), texture_.size());

  vk::Extent3D extent {
    static_cast<uint32_t>(texture_.extent().x),
    static_cast<uint32_t>(texture_.extent().y),
    1};

  Image texture_image(engine, extent, vk::Format::eR8G8B8A8Unorm);

  auto cmd = engine.getRenderer().CreateCommandBuffer();
  cmd.begin(vk::CommandBufferBeginInfo{});

  texture_image.TransitionLayout(vk::ImageLayout::eTransferDstOptimal, cmd, {});

  vk::BufferImageCopy region{};
  region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.layerCount = 1;
  region.imageExtent = extent;

  cmd.copyBufferToImage(
      staging_buffer.GetBuffer(),
      texture_image.GetImage(),
      vk::ImageLayout::eTransferDstOptimal,
      region);

  spdlog::info("Copying texture to image");

  texture_image.TransitionLayout(vk::ImageLayout::eShaderReadOnlyOptimal, cmd, {});
  cmd.end();

  engine.getRenderer().SubmitAndWait(cmd);

}

} // namespace braque