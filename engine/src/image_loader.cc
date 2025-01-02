#include "image_loader.h"

#include <spdlog/spdlog.h>

namespace braque
{
ImageData ImageLoader::load(const char* filename)
{
    int width, height, channels;
    unsigned char* data = SOIL_load_image(filename, &width, &height, &channels, 0);
    if (!data)
    {
        spdlog::error("Failed to load image: {}", filename);
    }
    return {width, height, channels, data};
}
} // namespace braque