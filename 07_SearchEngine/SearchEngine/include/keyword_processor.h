// Copyright 2025 Search Engine Project
// Author: Your Name

#ifndef SEARCH_ENGINE_KEYWORD_PROCESSOR_H_
#define SEARCH_ENGINE_KEYWORD_PROCESSOR_H_

#include <cppjieba/Jieba.hpp>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace search_engine {

// KeyWordProcessor 负责处理中英文语料库，生成词典和索引
// 支持中文分词(基于 cppjieba)和英文分词，并构建索引用于关键字推荐
class KeyWordProcessor {
 public:
  KeyWordProcessor();
  ~KeyWordProcessor() = default;

  // 禁止拷贝和赋值
  KeyWordProcessor(const KeyWordProcessor&) = delete;
  KeyWordProcessor& operator=(const KeyWordProcessor&) = delete;

  // 处理中英文语料库，生成词典和索引
  // 参数:
  //   cn_dir: 中文语料库目录
  //   en_dir: 英文语料库目录
  void Process(const std::string& cn_dir, const std::string& en_dir);

 private:
  // 创建中文词典
  // 参数:
  //   dir: 中文语料库目录
  //   output_file: 输出的词典文件路径
  void CreateChineseDict(const std::string& dir,
                         const std::string& output_file);

  // 构建中文索引
  // 参数:
  //   dict_file: 词典文件路径
  //   index_file: 索引文件路径
  void BuildChineseIndex(const std::string& dict_file,
                         const std::string& index_file);

  // 创建英文词典
  // 参数:
  //   dir: 英文语料库目录
  //   output_file: 输出的词典文件路径
  void CreateEnglishDict(const std::string& dir,
                         const std::string& output_file);

  // 构建英文索引
  // 参数:
  //   dict_file: 词典文件路径
  //   index_file: 索引文件路径
  void BuildEnglishIndex(const std::string& dict_file,
                         const std::string& index_file);

 private:
  cppjieba::Jieba tokenizer_;
  std::set<std::string> en_stop_words_;
  std::set<std::string> cn_stop_words_;
};

}  // namespace search_engine

#endif  // SEARCH_ENGINE_KEYWORD_PROCESSOR_H_
