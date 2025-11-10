// Copyright 2025 Search Engine Project
// Test for DirectoryScanner and KeyWordProcessor

#include <iostream>
#include <fstream>
#include "directory_scanner.h"
#include "keyword_processor.h"

using namespace search_engine;

void TestDirectoryScanner() {
  std::cout << "=== 测试 DirectoryScanner ===" << std::endl;

  // 测试扫描中文语料库
  std::cout << "\n测试扫描 corpus/CN:" << std::endl;
  auto cn_files = DirectoryScanner::Scan("corpus/CN");
  std::cout << "找到 " << cn_files.size() << " 个文件" << std::endl;
  for (size_t i = 0; i < std::min(size_t(5), cn_files.size()); ++i) {
    std::cout << "  " << cn_files[i] << std::endl;
  }

  // 测试扫描英文语料库
  std::cout << "\n测试扫描 corpus/EN:" << std::endl;
  auto en_files = DirectoryScanner::Scan("corpus/EN");
  std::cout << "找到 " << en_files.size() << " 个文件" << std::endl;
  for (const auto& file : en_files) {
    std::cout << "  " << file << std::endl;
  }

  // 测试扫描网页语料库
  std::cout << "\n测试扫描 corpus/webpages:" << std::endl;
  auto xml_files = DirectoryScanner::Scan("corpus/webpages");
  std::cout << "找到 " << xml_files.size() << " 个文件" << std::endl;
  for (size_t i = 0; i < std::min(size_t(5), xml_files.size()); ++i) {
    std::cout << "  " << xml_files[i] << std::endl;
  }
}

void TestKeyWordProcessor() {
  std::cout << "\n\n=== 测试 KeyWordProcessor ===" << std::endl;

  KeyWordProcessor processor;

  std::cout << "\n开始处理中英文语料库..." << std::endl;
  processor.Process("corpus/CN", "corpus/EN");
  std::cout << "处理完成！" << std::endl;

  // 检查生成的文件
  std::cout << "\n检查生成的文件:" << std::endl;

  // 检查中文词典
  std::ifstream cn_dict("data/cn_dict.dat");
  if (cn_dict.is_open()) {
    std::cout << "✓ cn_dict.dat 已生成" << std::endl;
    std::cout << "  前 10 个词条:" << std::endl;
    std::string word;
    int freq;
    int count = 0;
    while (cn_dict >> word >> freq && count < 10) {
      std::cout << "    " << word << " " << freq << std::endl;
      count++;
    }
  } else {
    std::cout << "✗ cn_dict.dat 未生成" << std::endl;
  }

  // 检查中文索引
  std::ifstream cn_index("data/cn_index.dat");
  if (cn_index.is_open()) {
    std::cout << "\n✓ cn_index.dat 已生成" << std::endl;
    std::cout << "  前 5 个索引项:" << std::endl;
    std::string line;
    int count = 0;
    while (std::getline(cn_index, line) && count < 5) {
      std::cout << "    " << line << std::endl;
      count++;
    }
  } else {
    std::cout << "✗ cn_index.dat 未生成" << std::endl;
  }

  // 检查英文词典
  std::ifstream en_dict("data/en_dict.dat");
  if (en_dict.is_open()) {
    std::cout << "\n✓ en_dict.dat 已生成" << std::endl;
    std::cout << "  前 10 个词条:" << std::endl;
    std::string word;
    int freq;
    int count = 0;
    while (en_dict >> word >> freq && count < 10) {
      std::cout << "    " << word << " " << freq << std::endl;
      count++;
    }
  } else {
    std::cout << "✗ en_dict.dat 未生成" << std::endl;
  }

  // 检查英文索引
  std::ifstream en_index("data/en_index.dat");
  if (en_index.is_open()) {
    std::cout << "\n✓ en_index.dat 已生成" << std::endl;
    std::cout << "  前 5 个索引项:" << std::endl;
    std::string line;
    int count = 0;
    while (std::getline(en_index, line) && count < 5) {
      std::cout << "    " << line << std::endl;
      count++;
    }
  } else {
    std::cout << "✗ en_index.dat 未生成" << std::endl;
  }
}

int main() {
  TestDirectoryScanner();
  TestKeyWordProcessor();

  std::cout << "\n\n=== 测试完成 ===" << std::endl;
  return 0;
}
