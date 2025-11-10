// Copyright 2025 Search Engine Project
// Author: Your Name

#include "directory_scanner.h"

#include <dirent.h>
#include <sys/types.h>

#include <cstring>

namespace search_engine {

std::vector<std::string> DirectoryScanner::Scan(const std::string& dir) {
  std::vector<std::string> files;

  // 打开目录流
  DIR* dp = opendir(dir.c_str());
  if (dp == nullptr) {
    perror("Error opening directory");
    return files;
  }

  // 读取目录项
  struct dirent* entry;
  while ((entry = readdir(dp)) != nullptr) {
    // 过滤 . 与 ..
    if (strcmp(entry->d_name, ".") == 0 ||
        strcmp(entry->d_name, "..") == 0) {
      continue;       
    }

    // 拼接完整路径
    std::string full_path = dir + "/" + entry->d_name;
    files.push_back(full_path);
  }

  closedir(dp);
  return files;
}

}  // namespace search_engine
