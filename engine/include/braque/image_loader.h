#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <vulkan/vulkan.hpp>
#include <SOIL2/SOIL2.h>

namespace braque
{
    class Texture {
    public:
        Texture(std::string path);

        ~Texture();

      // copy constructor
      Texture(Texture&& other) noexcept;

    private:
        int width;
        int height;
        int channels;
        unsigned char* data;
    };
}

#endif // IMAGE_LOADER_H