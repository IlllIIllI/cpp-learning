// Copyright 2025 Search Engine Project
// Author: Your Name

#include "page_processor.h"

#include <iostream>

int main() {
  std::cout << "=== 网页处理模块测试 ===" << std::endl;

  search_engine::PageProcessor processor;

  // 处理网页语料库
  std::string webpages_dir = "corpus/webpages";

  std::cout << "开始处理网页语料库..." << std::endl;
  processor.Process(webpages_dir);
  std::cout << "处理完成！" << std::endl;

  std::cout << "\n生成的文件:" << std::endl;
  std::cout << "  - pages.dat (网页库)" << std::endl;
  std::cout << "  - offsets.dat (网页偏移库)" << std::endl;
  std::cout << "  - inverted_index.dat (倒排索引库)" << std::endl;

  return 0;
}
