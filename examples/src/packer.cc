#include <braque/asset_loader.h>
#include <iostream>
#include <filesystem>
#include <iomanip>

auto main() -> int {
  namespace fs = std::filesystem;
  
  // Create and populate archive
  {
    braque::AssetLoader loader;
    std::cout << "Creating archive...\n";
    if (!loader.createArchive("../../../../test.gaff")) {
      std::cerr << "Failed to create archive\n";
      return 1;
    }

    std::cout << "Saving directory contents...\n";
    loader.SaveDirectory("../../../../assets");
    
    // Print statistics about the archive
    auto assets = loader.listAssets();
    std::cout << "\nArchive Statistics:\n";
    std::cout << "Total assets: " << assets.size() << "\n\n";
  }

  // Verify archive contents
  {
    braque::AssetLoader loader;
    std::cout << "Opening archive for verification...\n";
    if (!loader.openArchive("../../../../test.gaff")) {
      std::cerr << "Failed to open archive for verification\n";
      return 1;
    }

    // List and verify each asset
    auto assets = loader.listAssets();
    for (const auto& asset : assets) {
      std::cout << "\nVerifying asset: " << asset << "\n";
      
      // Load the asset data
      auto data = loader.loadAsset(asset);
      if (data.empty()) {
        std::cerr << "Failed to load asset: " << asset << "\n";
        continue;
      }

      // Print asset information
      std::cout << "  Size: " << data.size() << " bytes\n";
      
      // Calculate and print a simple checksum
      uint32_t checksum = 0;
      for (uint8_t byte : data) {
        checksum = (checksum << 1) ^ byte;
      }
      std::cout << "  Checksum: 0x" << std::hex << std::setw(8) 
                << std::setfill('0') << checksum << std::dec << "\n";
    }
  }

  std::cout << "\nVerification complete!\n";
  return 0;
}