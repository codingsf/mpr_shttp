#include "base/file_path.h"
#include <gtest/gtest.h>

namespace base {

TEST(FilePath, Basic) {
  FilePath path("/a/b/c/d.epub");

  LOG(INFO) << "base name: " << path.BaseName().value();
  LOG(INFO) << "exts name: " << path.Extension();
  LOG(INFO) << "dirs name: " << path.DirName().value();

//  FilePath new_path = (std::move(path.RemoveExtension())).
  LOG(INFO) << "----" << FilePath("/a/b/e.epub").ReplaceExtension(".pdf").value();
  EXPECT_TRUE(true);
}

} 
