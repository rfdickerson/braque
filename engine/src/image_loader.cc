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

  spdlog::info("Loaded texture: {}", path);
  spdlog::info("Texture size: {} x {} x {}", texture_.extent().x, texture_.extent().y, texture_.extent().z);
  spdlog::info("Texture mipmaps: {}", texture_.levels());
}

Texture::Texture(Texture&& other) noexcept:
texture_(other.texture_)
{

}


Texture::~Texture() {
  delete texture_image_;

}

void Texture::CreateImage(Engine& engine) {

  auto imageSize = texture_.size();
  auto levels = texture_.levels();

  Buffer staging_buffer(engine, BufferType::staging, imageSize);

  staging_buffer.CopyData(texture_.data(), imageSize);
  // Log some information for debugging
  spdlog::info("Texture size: {} x {} x {}", texture_.extent().x, texture_.extent().y, texture_.extent().z);
  spdlog::info("Total texture size: {} bytes", imageSize);

  vk::Extent3D const extent {
    static_cast<uint32_t>(texture_.extent().x),
    static_cast<uint32_t>(texture_.extent().y),
    static_cast<uint32_t>(texture_.extent().z)
};

  vk::ImageCreateInfo imageInfo{};
  imageInfo.imageType = vk::ImageType::e2D;  // or e3D if it's a 3D texture
  imageInfo.extent = extent;
  imageInfo.mipLevels = texture_.levels();
  imageInfo.arrayLayers = texture_.layers();
  imageInfo.format = vk::Format::eBc1RgbUnormBlock;  // Make sure this matches your texture format
  imageInfo.tiling = vk::ImageTiling::eOptimal;
  imageInfo.initialLayout = vk::ImageLayout::eUndefined;
  imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
  imageInfo.sharingMode = vk::SharingMode::eExclusive;
  imageInfo.samples = vk::SampleCountFlagBits::e1;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  texture_image_ = new Image(engine, imageInfo, allocInfo);;

  auto cmd = engine.getRenderer().CreateCommandBuffer();
  cmd.begin(vk::CommandBufferBeginInfo{});

  SyncBarriers barriers;
  barriers.srcStage = vk::PipelineStageFlagBits2::eTransfer;
  barriers.srcAccess = vk::AccessFlagBits2::eTransferWrite;
  barriers.dstStage = vk::PipelineStageFlagBits2::eTransfer;
  barriers.dstAccess = vk::AccessFlagBits2::eTransferRead;

  texture_image_->TransitionLayout(vk::ImageLayout::eTransferDstOptimal, cmd, barriers, levels);

  vk::DeviceSize offset = 0;
  for (uint32_t level = 0; level < texture_.levels(); ++level)
  {
    gli::extent3d const mip_extent = texture_.extent(level);
    gli::extent3d const block_extent = gli::block_extent(texture_.format());

    vk::BufferImageCopy region{};
    region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.imageSubresource.mipLevel = level;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = texture_.layers();
    region.imageOffset = vk::Offset3D{0, 0, 0};
    region.imageExtent = vk::Extent3D{
      static_cast<unsigned int>(std::max(block_extent.x, mip_extent.x)),
      static_cast<unsigned int>(std::max(block_extent.y, mip_extent.y)),
      static_cast<unsigned int>(std::max(block_extent.z, mip_extent.z))
  };

    region.bufferOffset = offset; // Offset in the buffer for this mip level

    spdlog::info("Copying mip level {}: {} x {} x {}",
        level, region.imageExtent.width, region.imageExtent.height, region.imageExtent.depth);

    cmd.copyBufferToImage(
        staging_buffer.GetBuffer(),
        texture_image_->GetImage(),
        vk::ImageLayout::eTransferDstOptimal,
        region);

    vk::DeviceSize mipSize = texture_.size(level);
    offset += mipSize;
  }

  spdlog::info("Copying texture to image");

  barriers.srcStage = vk::PipelineStageFlagBits2::eTransfer;
  barriers.srcAccess = vk::AccessFlagBits2::eTransferWrite;
  barriers.dstStage = vk::PipelineStageFlagBits2::eFragmentShader;
  barriers.dstAccess = vk::AccessFlagBits2::eShaderRead;

  texture_image_->TransitionLayout(vk::ImageLayout::eShaderReadOnlyOptimal, cmd, barriers, texture_.levels());
  cmd.end();

  engine.getRenderer().SubmitAndWait(cmd);


}

} // namespace braque