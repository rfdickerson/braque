#include <braque/asset_loader.h>

auto main() -> int {
  braque::AssetLoader loader;
  loader.createArchive("test.gaff");
  loader.SaveDirectory("../../../../assets");

  braque::AssetLoader loader2;
  loader2.openArchive("test.gaff");

  return 0;
}