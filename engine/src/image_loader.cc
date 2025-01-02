#include <braque/image_loader.h>

#include <spdlog/spdlog.h>

namespace braque
{

Texture::Texture(std::string filename)
{
    int width=0, height=0, channels=0;
    unsigned char* data = SOIL_load_image(filename.c_str(), &width, &height, &channels, 0);
    if (data == nullptr)
    {
        spdlog::error("Failed to load image: {}", filename);
    }

    this->width = width;
    this->height = height;
    this->channels = channels;
    this->data = data;
}

Texture::Texture(Texture&& other) noexcept:
  width(other.width), height(other.height), channels(other.channels), data(other.data)
{
  other.data = nullptr;
}

Texture::~Texture() {
  if (data == nullptr) {
    return;
  }
  SOIL_free_image_data(this->data);
  data = nullptr;
}

} // namespace braque