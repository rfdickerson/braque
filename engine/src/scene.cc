//
// Created by Robert F. Dickerson on 12/27/24.
//

#include "braque/scene.h"

#include "braque/engine.h"
#include "braque/texture.h"
#include "braque/asset_loader.h"

namespace braque {
Scene::Scene(EngineContext& engine, Uniforms& uniforms, AssetLoader& assetLoader)
    : engine_(engine),
      assetLoader_(assetLoader),
      vertex_buffer_(engine, BufferType::vertex, kVertexBufferSize),
      index_buffer_(engine, BufferType::index, kVertexBufferSize),
      vertex_staging_buffer_(engine, BufferType::staging, kVertexBufferSize),
      index_staging_buffer_(engine, BufferType::staging, kVertexBufferSize) {

  // add a cube to vertex and index staging buffers
  //AddCube();
  
  AddTerrain();
  UploadSceneData();

  // Load texture data from asset loader
  auto textureData = assetLoader_.loadAsset("brick_d.dds");
  if (textureData.empty()) {
    throw std::runtime_error("Failed to load brick texture from asset archive");
  }

  texture_ = new Texture(engine, "cobblestone", TextureType::eAlbedo, textureData);
  texture_->CreateImage(engine);

  CreateTextureSampler();

  uniforms.SetTextureData(*texture_, texture_sampler_);
}

Scene::~Scene() {

  engine_.getRenderer().getDevice().destroySampler(texture_sampler_);

  delete texture_;
}

void Scene::Draw(vk::CommandBuffer buffer) {

  vertex_buffer_.Bind(buffer);
  index_buffer_.Bind(buffer);

  for (const auto& mesh : meshes_) {
    // draw the mesh
    buffer.drawIndexed(mesh.index_count, 1, mesh.index_offset,
                       mesh.vertex_offset, 0);
  }
}

void Scene::UploadSceneData() {


  const auto graphicsQueue = engine_.getRenderer().getGraphicsQueue();

  const auto command_buffer = engine_.getRenderer().CreateCommandBuffer();

  // begin single time command buffer
  vk::CommandBufferBeginInfo beginInfo;
  beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

  command_buffer.begin(beginInfo);

  vertex_staging_buffer_.CopyToBuffer(command_buffer, vertex_buffer_);
  index_staging_buffer_.CopyToBuffer(command_buffer, index_buffer_);

  // end the command buffer
  command_buffer.end();

  // submit
  vk::SubmitInfo submitInfo;
  submitInfo.setCommandBuffers(command_buffer);
  graphicsQueue.submit(submitInfo, nullptr);

  engine_.getRenderer().waitIdle();
}

void Scene::AddCube() {

  // Face 1..6 each has 6 vertices = 36 total
// Indices will then be 6 faces × 2 triangles × 3 indices = 36 indices
// Face ordering: Front (Blue), Back (Red), Left (Green), Right (Yellow), Bottom (Cyan), Top (Magenta).

std::vector<Vertex> vertices = {
    // Face 1 (Front, Blue), z = -0.5
    // bottom-left, bottom-right, top-right, top-left, repeat bottom-left, repeat top-right
    {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}, {0.0f, 0.0f}}, // [0]
    {{ 0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}, {1.0f, 0.0f}}, // [1]
    {{ 0.5f,  0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}, {1.0f, 1.0f}}, // [2]
    {{-0.5f,  0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}, {0.0f, 1.0f}}, // [3]
    {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}, {0.0f, 0.0f}}, // [4] duplicate of [0]
    {{ 0.5f,  0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}, {1.0f, 1.0f}}, // [5] duplicate of [2]

    // Face 2 (Back, Red), z = +0.5
    {{-0.5f, -0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}, {1.0f, 0.0f}}, // [6]  bottom-left
    {{ 0.5f, -0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}, {0.0f, 0.0f}}, // [7]  bottom-right
    {{ 0.5f,  0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}, {0.0f, 1.0f}}, // [8]  top-right
    {{-0.5f,  0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}, {1.0f, 1.0f}}, // [9]  top-left
    {{-0.5f, -0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}, {1.0f, 0.0f}}, // [10] duplicate of [6]
    {{ 0.5f,  0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}, {0.0f, 1.0f}}, // [11] duplicate of [8]

    // Face 3 (Left, Green), x = -0.5
    {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {0, 1, 0}, {0.0f, 0.0f}}, // [12] bottom-left
    {{-0.5f, -0.5f,  0.5f}, {-1, 0, 0}, {0, 1, 0}, {1.0f, 0.0f}}, // [13] bottom-right
    {{-0.5f,  0.5f,  0.5f}, {-1, 0, 0}, {0, 1, 0}, {1.0f, 1.0f}}, // [14] top-right
    {{-0.5f,  0.5f, -0.5f}, {-1, 0, 0}, {0, 1, 0}, {0.0f, 1.0f}}, // [15] top-left
    {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {0, 1, 0}, {0.0f, 0.0f}}, // [16] duplicate of [12]
    {{-0.5f,  0.5f,  0.5f}, {-1, 0, 0}, {0, 1, 0}, {1.0f, 1.0f}}, // [17] duplicate of [14]

    // Face 4 (Right, Yellow), x = +0.5
    {{ 0.5f, -0.5f, -0.5f}, {1, 0, 0}, {1, 1, 0}, {1.0f, 0.0f}}, // [18] bottom-left
    {{ 0.5f, -0.5f,  0.5f}, {1, 0, 0}, {1, 1, 0}, {0.0f, 0.0f}}, // [19] bottom-right
    {{ 0.5f,  0.5f,  0.5f}, {1, 0, 0}, {1, 1, 0}, {0.0f, 1.0f}}, // [20] top-right
    {{ 0.5f,  0.5f, -0.5f}, {1, 0, 0}, {1, 1, 0}, {1.0f, 1.0f}}, // [21] top-left
    {{ 0.5f, -0.5f, -0.5f}, {1, 0, 0}, {1, 1, 0}, {1.0f, 0.0f}}, // [22] duplicate of [18]
    {{ 0.5f,  0.5f,  0.5f}, {1, 0, 0}, {1, 1, 0}, {0.0f, 1.0f}}, // [23] duplicate of [20]

    // Face 5 (Bottom, Cyan), y = -0.5
    {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0, 1, 1}, {0.0f, 0.0f}}, // [24] bottom-left
    {{ 0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0, 1, 1}, {1.0f, 0.0f}}, // [25] bottom-right
    {{ 0.5f, -0.5f,  0.5f}, {0, -1, 0}, {0, 1, 1}, {1.0f, 1.0f}}, // [26] top-right
    {{-0.5f, -0.5f,  0.5f}, {0, -1, 0}, {0, 1, 1}, {0.0f, 1.0f}}, // [27] top-left
    {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0, 1, 1}, {0.0f, 0.0f}}, // [28] duplicate of [24]
    {{ 0.5f, -0.5f,  0.5f}, {0, -1, 0}, {0, 1, 1}, {1.0f, 1.0f}}, // [29] duplicate of [26]

    // Face 6 (Top, Magenta), y = +0.5
    {{-0.5f,  0.5f, -0.5f}, {0, 1, 0}, {1, 0, 1}, {0.0f, 0.0f}}, // [30] bottom-left
    {{ 0.5f,  0.5f, -0.5f}, {0, 1, 0}, {1, 0, 1}, {1.0f, 0.0f}}, // [31] bottom-right
    {{ 0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 0, 1}, {1.0f, 1.0f}}, // [32] top-right
    {{-0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 0, 1}, {0.0f, 1.0f}}, // [33] top-left
    {{-0.5f,  0.5f, -0.5f}, {0, 1, 0}, {1, 0, 1}, {0.0f, 0.0f}}, // [34] duplicate of [30]
    {{ 0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 0, 1}, {1.0f, 1.0f}}, // [35] duplicate of [32]
};


  std::vector<uint32_t> indices = {
    // Front:   uses vertices [0..5]
    0, 1, 2,   2, 3, 0,

    // Back:    uses vertices [6..11]
    6, 8, 7,   8, 6, 9,

    // Left:    uses vertices [12..17]
    12, 14, 13, 14, 12, 15,

    // Right:   uses vertices [18..23]
    18, 19, 20, 20, 21, 18,

    // Bottom:  uses vertices [24..29]
    24, 26, 25, 26, 24, 27,

    // Top:     uses vertices [30..35]
    30, 31, 32, 32, 33, 30
  };

  // move data to staging buffer
  vertex_staging_buffer_.CopyData(vertices.data(),
                                  vertices.size() * sizeof(Vertex));

  index_staging_buffer_.CopyData(indices.data(),
                                 indices.size() * sizeof(uint32_t));

  // create the cube
  Mesh cube;
  cube.name = "cube";
  cube.vertex_offset = 0;
  cube.index_offset = 0;
  cube.index_count = 36;

  meshes_.push_back(cube);
}

void Scene::AddTerrain() {
    // Create a simple terrain grid (10x10)
    const int grid_size = 10;
    const float cell_size = 1.0f;
    const float height_scale = 0.5f;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Generate vertices
    for (int z = 0; z < grid_size; z++) {
        for (int x = 0; x < grid_size; x++) {
            float xPos = (x - grid_size/2.0f) * cell_size;
            float zPos = (z - grid_size/2.0f) * cell_size;
            // Simple height function (can be replaced with more complex terrain generation)
            float height = height_scale * sinf(xPos) * cosf(zPos);

            Vertex v;
            v.position = {xPos, height, zPos};
            // Calculate normal (simplified - just pointing up for now)
            v.normal = {0.0f, 1.0f, 0.0f};
            // UV coordinates for texturing
            v.uv = {static_cast<float>(x) / (grid_size - 1),
                   static_cast<float>(z) / (grid_size - 1)};
            // Terrain color (can be modified based on height)
            v.color = {0.5f, 0.5f, 0.3f};

            vertices.push_back(v);
        }
    }

    // Generate indices for triangle strips
    for (int z = 0; z < grid_size - 1; z++) {
        for (int x = 0; x < grid_size - 1; x++) {
            uint32_t topLeft = z * grid_size + x;
            uint32_t topRight = topLeft + 1;
            uint32_t bottomLeft = (z + 1) * grid_size + x;
            uint32_t bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    // Copy vertex data to staging buffer
    size_t vertex_offset = vertex_staging_buffer_.GetSize();
    vertex_staging_buffer_.CopyData(vertices.data(),
                                  vertices.size() * sizeof(Vertex));

    // Copy index data to staging buffer
    size_t index_offset = index_staging_buffer_.GetSize();
    index_staging_buffer_.CopyData(indices.data(),
                                 indices.size() * sizeof(uint32_t));

    // Add mesh to the scene
    Mesh terrain_mesh;
    terrain_mesh.name = "terrain";
    terrain_mesh.vertex_offset = 0;
    terrain_mesh.index_offset = 0;
    terrain_mesh.index_count = indices.size();
    meshes_.push_back(terrain_mesh);
}

void Scene::CreateTextureSampler() {
  vk::SamplerCreateInfo samplerInfo{};

  samplerInfo.setMagFilter(vk::Filter::eLinear);
  samplerInfo.setMinFilter(vk::Filter::eLinear);
  samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
  samplerInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
  samplerInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
  samplerInfo.setAddressModeW(vk::SamplerAddressMode::eRepeat);
  samplerInfo.setMipLodBias(0.0f);
  samplerInfo.setAnisotropyEnable(true);
  samplerInfo.setMaxAnisotropy(16.0f);
  samplerInfo.setCompareOp(vk::CompareOp::eAlways);
  samplerInfo.setMinLod(0.0f);
  samplerInfo.setMaxLod(8.0f);

  texture_sampler_ = engine_.getRenderer().getDevice().createSampler(samplerInfo);
}


}  // namespace braque