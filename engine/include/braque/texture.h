#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <memory>
#include <string>

#include <braque/image.h>
#include <gli/gli.hpp>

namespace braque {
// Forward declarations
class Engine;

enum class TextureType { albedo, normal, roughness, metallic, ao };

class Texture {
 public:
  Texture(std::string name, TextureType texture_type, std::string path);

  ~Texture();

  void CreateImage(Engine& engine);

  [[nodiscard]] auto GetName() const -> std::string { return name_; }

  [[nodiscard]] auto GetImageView() const -> vk::ImageView { return texture_image_->GetImageView(); }

  // Move constructor (already defined)
  Texture(Texture&& other) noexcept;

  // Delete copy operations
  Texture(const Texture& other) = delete;
  auto operator=(const Texture& other) -> Texture& = delete;

 private:
  std::string name_;
  TextureType texture_type_;
  std::string path_;

  gli::texture texture_;

  Image* texture_image_;
};
}  // namespace braque

#endif  // IMAGE_LOADER_H