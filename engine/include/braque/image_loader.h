#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <vulkan/vulkan.hpp>
#include <soil/SOIL.h>

namespace braque
{
    struct ImageData {
        int width;
        int height;
        int channels;
        unsigned char* data;
    };
    
    class ImageLoader
    {
    public:
        static ImageData load(const char* filename);
    };
}