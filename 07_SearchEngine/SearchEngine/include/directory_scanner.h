// Copyright 2025 Search Engine Project
// Author: Your Name

#ifndef SEARCH_ENGINE_DIRECTORY_SCANNER_H_
#define SEARCH_ENGINE_DIRECTORY_SCANNER_H_

#include <string>
#include <vector>

namespace search_engine {

// DirectoryScanner 提供目录扫描功能，用于遍历指定目录下的所有文件
// 该类不可实例化，所有方法均为静态方法
class DirectoryScanner {
 public:
  // 遍历目录，获取目录里面的所有文件名
  // 参数:
  //   dir: 要扫描的目录路径
  // 返回:
  //   目录下所有文件的完整路径列表
  static std::vector<std::string> Scan(const std::string& dir);

 private:
  DirectoryScanner() = delete;
  ~DirectoryScanner() = delete;
  DirectoryScanner(const DirectoryScanner&) = delete;
  DirectoryScanner& operator=(const DirectoryScanner&) = delete;
};

}  // namespace search_engine

#endif  // SEARCH_ENGINE_DIRECTORY_SCANNER_H_
