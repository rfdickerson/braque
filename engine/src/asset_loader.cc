//
// Created by Robert F. Dickerson on 1/10/25.
//

#include "braque/asset_loader.h"
#include <snappy.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <unordered_map>

namespace braque {

AssetLoader::AssetLoader() : file_(nullptr, fclose) {}

AssetLoader::~AssetLoader() = default;

bool AssetLoader::openArchive(const std::string& path) {
  FILE* f = fopen(path.c_str(), "rb");
  if (!f)
    return false;

  file_.reset(f);
  if (!readCatalog())
    return false;

  // Print catalog information
  std::cout << "Archive opened successfully: " << path << std::endl;
  std::cout << "Catalog contents:" << std::endl;
  std::cout << "Total assets: " << catalog_.size() << std::endl;

  for (const auto& [name, entry] : catalog_) {
    std::cout << "Asset: " << name << std::endl;
    std::cout << "  Type: " << static_cast<int>(entry.assetType) << std::endl;
    std::cout << "  Compressed Size: " << entry.compressedSize << " bytes"
              << std::endl;
    std::cout << "  Original Size: " << entry.originalSize << " bytes"
              << std::endl;
    std::cout << std::endl;
  }

  return true;
}

bool AssetLoader::createArchive(const std::string& path) {
  FILE* f = fopen(path.c_str(), "wb+");
  if (!f)
    return false;

  file_.reset(f);

  // Write initial header
  GAAFHeader header;
  std::memcpy(header.magic, "GAAF", 4);
  header.version = 1;
  header.compression = static_cast<uint16_t>(CompressionType::SNAPPY);
  header.catalogOffset = 0;
  header.catalogSize = 0;
  std::memset(header.reserved, 0, sizeof(header.reserved));

  fwrite(&header, sizeof(header), 1, file_.get());
  return true;
}

bool AssetLoader::addAsset(const std::string& name, AssetType type,
                           const std::vector<uint8_t>& data) {
  if (exists(name))
    return false;

  AssetCatalogEntry entry;
  entry.hash = fnv1aHash(name);
  strncpy(entry.name, name.c_str(), sizeof(entry.name) - 1);
  entry.name[sizeof(entry.name) - 1] = '\0';
  entry.assetType = static_cast<uint32_t>(type);
  entry.originalSize = data.size();

  // Compress the data
  auto compressed = compressData(data);
  entry.compressedSize = compressed.size();

  long currentPosition = ftell(file_.get());

  // Seek to end for new data
  fseek(file_.get(), 0, SEEK_END);
  entry.offset = ftell(file_.get());

  // Write the compressed data
  if (fwrite(compressed.data(), 1, compressed.size(), file_.get()) != compressed.size())
    return false;

  // Add to catalog
  catalog_[name] = entry;

  // Update the catalog on disk
  return writeCatalog();
}

std::vector<uint8_t> AssetLoader::loadAsset(const std::string& name) {
  auto it = catalog_.find(name);
  if (it == catalog_.end())
    return {};

  const auto& entry = it->second;
  if (entry.hash != fnv1aHash(name)) {
    // Hash collision detected
    return {};
  }

  // Read compressed data
  std::vector<uint8_t> compressed(entry.compressedSize);
  fseek(file_.get(), entry.offset, SEEK_SET);
  fread(compressed.data(), 1, entry.compressedSize, file_.get());

  // Decompress
  return decompressData(compressed, entry.originalSize);
}

bool AssetLoader::exists(const std::string& name) const {
  return catalog_.find(name) != catalog_.end();
}

std::vector<std::string> AssetLoader::listAssets() const {
  std::vector<std::string> assets;
  assets.reserve(catalog_.size());
  for (const auto& [name, _] : catalog_) {
    assets.push_back(name);
  }
  return assets;
}

bool AssetLoader::readCatalog() {
  GAAFHeader header;
  fseek(file_.get(), 0, SEEK_SET);
  if (fread(&header, sizeof(header), 1, file_.get()) != 1)
    return false;

  // Verify magic
  if (std::memcmp(header.magic, "GAAF", 4) != 0)
    return false;

  // Read catalog
  fseek(file_.get(), header.catalogOffset, SEEK_SET);
  size_t entryCount = header.catalogSize / sizeof(AssetCatalogEntry);

  catalog_.clear();
  for (size_t i = 0; i < entryCount; ++i) {
    AssetCatalogEntry entry;
    if (fread(&entry, sizeof(entry), 1, file_.get()) != 1)
      return false;
    catalog_[entry.name] = entry;
  }

  return true;
}

bool AssetLoader::writeCatalog() {
  // Get current position
  long currentPos = ftell(file_.get());

  // Write catalog
  for (const auto& [_, entry] : catalog_) {
    fwrite(&entry, sizeof(entry), 1, file_.get());
  }

  // Update header
  GAAFHeader header;
  fseek(file_.get(), 0, SEEK_SET);
  fread(&header, sizeof(header), 1, file_.get());

  header.catalogOffset = currentPos;
  header.catalogSize = catalog_.size() * sizeof(AssetCatalogEntry);

  fseek(file_.get(), 0, SEEK_SET);
  fwrite(&header, sizeof(header), 1, file_.get());

  return true;
}

std::vector<uint8_t> AssetLoader::compressData(
    const std::vector<uint8_t>& data) {
  std::string compressed;
  snappy::Compress(reinterpret_cast<const char*>(data.data()), data.size(),
                   &compressed);
  return std::vector<uint8_t>(compressed.begin(), compressed.end());
}

std::vector<uint8_t> AssetLoader::decompressData(
    const std::vector<uint8_t>& data, size_t originalSize) {
  std::string decompressed;
  snappy::Uncompress(reinterpret_cast<const char*>(data.data()), data.size(),
                     &decompressed);
  return std::vector<uint8_t>(decompressed.begin(), decompressed.end());
}

void AssetLoader::SaveDirectory(const std::string& directory) {
  namespace fs = std::filesystem;
  std::vector<std::tuple<std::string, AssetType, fs::path>> assetRecords;
  std::unordered_map<std::string, AssetCatalogEntry> tempCatalog;

  // First pass: Build the catalog
  try {
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
      if (fs::is_regular_file(entry)) {
        std::string filename = entry.path().filename().string();
        std::string extension = entry.path().extension().string();

        // Convert extension to lowercase for consistency
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        // Determine AssetType based on extension
        AssetType assetType;
        if (extension == ".dds") {
          assetType = AssetType::TEXTURE;
        } else if (extension == ".obj" || extension == ".fbx") {
          assetType = AssetType::MODEL;
        } else if (extension == ".wav" || extension == ".mp3") {
          assetType = AssetType::AUDIO;
        } else if (extension == ".spv") {
          assetType = AssetType::SHADER;
        } else {
          continue;
        }

        // Store the record for second pass
        assetRecords.emplace_back(filename, assetType, entry.path());
        
        // Create catalog entry (without offset/size info yet)
        AssetCatalogEntry entry;
        entry.hash = fnv1aHash(filename);
        strncpy(entry.name, filename.c_str(), sizeof(entry.name) - 1);
        entry.name[sizeof(entry.name) - 1] = '\0';
        entry.assetType = static_cast<uint32_t>(assetType);
        
        tempCatalog[filename] = entry;
      }
    }

    // Write initial header (will update later)
    GAAFHeader header;
    std::memcpy(header.magic, "GAAF", 4);
    header.version = 1;
    header.compression = static_cast<uint16_t>(CompressionType::SNAPPY);
    header.catalogOffset = sizeof(GAAFHeader);  // Initial catalog position
    header.catalogSize = 0;  // Will update later
    std::memset(header.reserved, 0, sizeof(header.reserved));

    fseek(file_.get(), 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, file_.get());

    // Second pass: Process files and write data
    uint64_t currentOffset = sizeof(GAAFHeader) + (tempCatalog.size() * sizeof(AssetCatalogEntry));
    
    for (const auto& [filename, assetType, filepath] : assetRecords) {
      // Read file content
      std::ifstream file(filepath, std::ios::binary | std::ios::ate);
      if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        continue;
      }

      std::streamsize size = file.tellg();
      file.seekg(0, std::ios::beg);

      std::vector<uint8_t> data(size);
      if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
        std::cerr << "Failed to read file: " << filename << std::endl;
        continue;
      }

      // Compress the data
      auto compressed = compressData(data);
      
      // Update catalog entry with size and offset information
      auto& catalogEntry = tempCatalog[filename];
      catalogEntry.offset = currentOffset;
      catalogEntry.compressedSize = compressed.size();
      catalogEntry.originalSize = data.size();

      // Write data at the current offset
      fseek(file_.get(), currentOffset, SEEK_SET);
      fwrite(compressed.data(), 1, compressed.size(), file_.get());
      
      currentOffset += compressed.size();
    }

    // Write catalog after header
    fseek(file_.get(), sizeof(GAAFHeader), SEEK_SET);
    for (const auto& [name, entry] : tempCatalog) {
      fwrite(&entry, sizeof(AssetCatalogEntry), 1, file_.get());
      catalog_[name] = entry;  // Update in-memory catalog
    }

    // Update header with final catalog size
    header.catalogSize = tempCatalog.size() * sizeof(AssetCatalogEntry);
    fseek(file_.get(), 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, file_.get());

  } catch (const fs::filesystem_error& e) {
    std::cerr << "Filesystem error: " << e.what() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

}  // namespace braque