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

  std::string GetName() const { return name_; }

  // Move constructor (already defined)
  Texture(Texture&& other) noexcept;

  // Delete copy operations
  Texture(const Texture& other) = delete;
  Texture& operator=(const Texture& other) = delete;

 private:
  std::string name_;
  TextureType texture_type_;
  std::string path_;

  int size;
  unsigned char* data;

  gli::texture texture_;

  std::unique_ptr<Image> image;
};
}  // namespace braque

#endif  // IMAGE_LOADER_H