#include <braque/texture.h>

#include <spdlog/spdlog.h>
#include <gli/gli.hpp>

namespace braque {

Texture::Texture(EngineContext& engine, std::string name, TextureType texture_type,
                 std::string path)
    : name_(name),
      texture_type_(texture_type),
      path_(path),
      texture_{gli::load(path)},
      texture_image_(engine, CreateImageInfo(texture_),
                     CreateAllocationInfo(texture_)) {
  spdlog::info("Loaded texture: {}", path);
  spdlog::info("Texture size: {} x {} x {}", texture_.extent().x,
               texture_.extent().y, texture_.extent().z);
  spdlog::info("Texture mipmaps: {}", texture_.levels());
}

Texture::Texture(Texture&& other) noexcept
    : name_(std::move(other.name_)),
      texture_type_(other.texture_type_),
      path_(std::move(other.path_)),
      texture_(std::move(other.texture_)),
      texture_image_(std::move(other.texture_image_)) {
  // Clear the moved-from object
  other.texture_type_ = TextureType::eUnknown;

  // If there are any other member variables that need to be moved or reset, do it here

  spdlog::debug("Texture moved: {}", name_);
}

Texture& Texture::operator=(Texture&& other) noexcept {
  if (this != &other) {
    name_ = std::move(other.name_);
    texture_ = std::move(other.texture_);
    texture_type_ = other.texture_type_;
    path_ = std::move(other.path_);
    texture_image_ = std::move(other.texture_image_);

    // Reset the moved-from object
    other.texture_type_ = TextureType::eUnknown;
    // If there are any other member variables that need to be reset, do it here

    spdlog::debug("Texture move assigned: {}", name_);
  }
  return *this;
}

void Texture::CreateImage(EngineContext& engine) {

  auto imageSize = texture_.size();
  auto levels = texture_.levels();

  Buffer staging_buffer(engine, BufferType::staging, imageSize);
  staging_buffer.CopyData(texture_.data(), imageSize);

  spdlog::info("Total texture size: {} bytes", imageSize);

  auto cmd = engine.getRenderer().CreateCommandBuffer();
  cmd.begin(vk::CommandBufferBeginInfo{});

  SyncBarriers barriers;
  barriers.srcStage = vk::PipelineStageFlagBits2::eTransfer;
  barriers.srcAccess = vk::AccessFlagBits2::eTransferWrite;
  barriers.dstStage = vk::PipelineStageFlagBits2::eTransfer;
  barriers.dstAccess = vk::AccessFlagBits2::eTransferRead;

  texture_image_.TransitionLayout(vk::ImageLayout::eTransferDstOptimal, cmd,
                                  barriers, levels);

  // Before the loop, let's log the total number of mip levels
  spdlog::info("Total mip levels: {}", texture_.levels());

  vk::DeviceSize offset = 0;
  for (uint32_t level = 0; level < texture_.levels(); ++level) {
    gli::extent3d const mip_extent = texture_.extent(level);
    vk::DeviceSize mipSize = texture_.size(level);

    // Log information about each mip level
    spdlog::info("Mip level {}: size = {} bytes, extent = {} x {} x {}", level,
                 mipSize, mip_extent.x, mip_extent.y, mip_extent.z);

    if (mipSize == 0) {
      spdlog::warn("Mip level {} has zero size, skipping", level);
      continue;
    }

    vk::BufferImageCopy region{};
    region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.imageSubresource.mipLevel = level;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;  // Assuming non-array texture
    region.imageOffset = vk::Offset3D{0, 0, 0};
    region.imageExtent = vk::Extent3D{static_cast<uint32_t>(mip_extent.x),
                                      static_cast<uint32_t>(mip_extent.y),
                                      static_cast<uint32_t>(mip_extent.z)};
    region.bufferOffset = offset;

    cmd.copyBufferToImage(staging_buffer.GetBuffer(), texture_image_.GetImage(),
                          vk::ImageLayout::eTransferDstOptimal, region);

    offset += mipSize;
  }

  barriers.srcStage = vk::PipelineStageFlagBits2::eTransfer;
  barriers.srcAccess = vk::AccessFlagBits2::eTransferWrite;
  barriers.dstStage = vk::PipelineStageFlagBits2::eFragmentShader;
  barriers.dstAccess = vk::AccessFlagBits2::eShaderRead;

  texture_image_.TransitionLayout(vk::ImageLayout::eShaderReadOnlyOptimal, cmd,
                                  barriers, texture_.levels());
  cmd.end();

  engine.getRenderer().SubmitAndWait(cmd);
}

auto Texture::GetExtent(gli::texture const& texture) -> vk::Extent3D {
  return {static_cast<uint32_t>(texture.extent().x),
          static_cast<uint32_t>(texture.extent().y), 1};
}

auto Texture::GetFormat(const gli::texture& texture) -> vk::Format {
  return vk::Format::eBc1RgbSrgbBlock;
}

vk::ImageCreateInfo Texture::CreateImageInfo(const gli::texture& texture) {
  vk::ImageCreateInfo imageInfo{};
  imageInfo.imageType = vk::ImageType::e2D;  // or e3D if it's a 3D texture
  imageInfo.extent = GetExtent(texture);
  imageInfo.mipLevels = texture.levels();
  imageInfo.arrayLayers = texture.layers();
  imageInfo.format = vk::Format::eBc1RgbSrgbBlock;
  imageInfo.tiling = vk::ImageTiling::eOptimal;
  imageInfo.initialLayout = vk::ImageLayout::eUndefined;
  imageInfo.usage =
      vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
  imageInfo.sharingMode = vk::SharingMode::eExclusive;
  imageInfo.samples = vk::SampleCountFlagBits::e1;
  return imageInfo;
}

VmaAllocationCreateInfo Texture::CreateAllocationInfo(
    const gli::texture& texture) {
  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  return allocInfo;
}

}  // namespace braque