#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <cstring>

namespace braque {

enum class CompressionType : uint16_t {
    NONE = 0,
    SNAPPY = 1
};

// Header structure (fixed size: 40 bytes)
struct GAAFHeader {
  char magic[4];            // "GAAF"
  uint16_t version;         // Format version
  uint16_t compression;     // Compression type
  uint64_t catalogOffset;   // Offset to the catalog
  uint64_t catalogSize;     // Size of the catalog
  char reserved[16];        // Reserved for future use
};

struct GAAFCatalogEntry {
  uint64_t hash;            // Hash of the asset name
  char assetName[256];      // Asset name (for reference/debugging)
  uint64_t offset;          // Offset to asset data
  uint64_t compressedSize;  // Compressed size
  uint64_t uncompressedSize;// Uncompressed size
  uint16_t compression;     // Compression type
  uint32_t checksum;        // CRC32 checksum

  std::string getAssetName() const {
    return std::string(assetName);
  }

  void print() const {
    std::cout << "Asset Name: " << getAssetName() << "\n"
              << "  Offset: " << offset << "\n"
              << "  Compressed Size: " << compressedSize << "\n"
              << "  Uncompressed Size: " << uncompressedSize << "\n"
              << "  Compression: " << compression << "\n"
              << "  Checksum: " << checksum << "\n";
  }
};

struct AssetCatalogEntry {
    uint64_t offset;          // Offset to asset data
    uint64_t compressedSize;  // Size of compressed data
    uint64_t originalSize;    // Original uncompressed size
    uint32_t assetType;       // Type of asset (shader, texture, etc.)
    char name[256];           // Asset name/path
};

class AssetLoader {
public:
    enum class AssetType : uint32_t {
        SHADER = 1,
        TEXTURE = 2,
        MODEL = 3,
        AUDIO = 4
    };

    AssetLoader();
    ~AssetLoader();

    // Open and create archive
    bool openArchive(const std::string& path);
    bool createArchive(const std::string& path);

    // Asset management
    bool addAsset(const std::string& name, AssetType type, const std::vector<uint8_t>& data);
    std::vector<uint8_t> loadAsset(const std::string& name);
    
    // Utility functions
    bool exists(const std::string& name) const;
    std::vector<std::string> listAssets() const;

    void SaveDirectory(const std::string& directory);

private:
    std::unique_ptr<FILE, decltype(&fclose)> file_;
    std::unordered_map<std::string, AssetCatalogEntry> catalog_;
    
    bool readCatalog();
    bool writeCatalog();
    std::vector<uint8_t> compressData(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressData(const std::vector<uint8_t>& data, size_t originalSize);
};

} // namespace braque
