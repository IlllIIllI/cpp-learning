// Copyright 2025 Search Engine Project
// Author: Your Name

#ifndef SEARCH_ENGINE_PAGE_PROCESSOR_H_
#define SEARCH_ENGINE_PAGE_PROCESSOR_H_

#include <cppjieba/Jieba.hpp>
#include <simhash/Simhasher.hpp>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace search_engine {

// PageProcessor 负责处理网页语料库，生成网页库、偏移库和倒排索引
// 支持 XML 解析、文档去重(基于 simhash)、TF-IDF 权重计算
class PageProcessor {
 public:
  PageProcessor();
  ~PageProcessor() = default;

  // 禁止拷贝和赋值
  PageProcessor(const PageProcessor&) = delete;
  PageProcessor& operator=(const PageProcessor&) = delete;

  // 处理网页语料库，生成网页库、偏移库和倒排索引
  // 参数:
  //   dir: 网页语料库目录 (corpus/webpages)
  void Process(const std::string& dir);

 private:
  // 文档结构体
  struct Document {
    int id;
    std::string link;
    std::string title;
    std::string content;
  };

  // 从 XML 文件中提取文档
  // 参数:
  //   dir: 网页语料库目录
  void ExtractDocuments(const std::string& dir);

  // 使用 simhash 对文档去重
  void DeduplicateDocuments();

  // 生成网页库和网页偏移库
  // 参数:
  //   pages_file: 网页库文件路径
  //   offsets_file: 网页偏移库文件路径
  void BuildPagesAndOffsets(const std::string& pages_file,
                            const std::string& offsets_file);

  // 生成倒排索引库
  // 参数:
  //   index_file: 倒排索引文件路径
  void BuildInvertedIndex(const std::string& index_file);

 private:
  cppjieba::Jieba tokenizer_;
  simhash::Simhasher hasher_;
  std::set<std::string> stop_words_;
  std::vector<Document> documents_;
  std::map<std::string, std::map<int, double>> inverted_index_;
};

}  // namespace search_engine

#endif  // SEARCH_ENGINE_PAGE_PROCESSOR_H_
