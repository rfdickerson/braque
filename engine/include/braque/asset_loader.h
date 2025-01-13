#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <cstring>
#include <optional>
#include <iomanip>

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

struct AssetCatalogEntry {
    uint64_t hash;            // Hash of the asset name
    uint64_t offset;          // Offset to asset data
    uint64_t compressedSize;  // Size of compressed data
    uint64_t originalSize;    // Original uncompressed size
    uint32_t assetType;       // Type of asset (shader, texture, etc.)
    uint32_t checksum;        // CRC32 checksum
    char name[256];           // Asset name/path

    std::string getAssetName() const {
        return std::string(name);
    }
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
    std::optional<AssetCatalogEntry> getAssetInfo(const std::string& name) const;

    void SaveDirectory(const std::string& directory);

private:
    std::unique_ptr<FILE, decltype(&fclose)> file_;
    std::unordered_map<std::string, AssetCatalogEntry> catalog_;
    
    bool readCatalog();
    bool writeCatalog();
    std::vector<uint8_t> compressData(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompressData(const std::vector<uint8_t>& data, size_t originalSize, uint32_t expectedChecksum);
    uint32_t calculateCRC32(const std::vector<uint8_t>& data);

    static uint64_t fnv1aHash(const std::string& input) {
        const uint64_t fnvOffsetBasis = 14695981039346656037ULL;
        const uint64_t fnvPrime = 1099511628211ULL;
        uint64_t hash = fnvOffsetBasis;
        for (char c : input) {
            hash ^= static_cast<uint64_t>(c);
            hash *= fnvPrime;
        }
        return hash;
    }
};

} // namespace braque
