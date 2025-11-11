// Copyright 2025 Search Engine Project
// Author: Your Name

#include "page_processor.h"

#include <utfcpp/utf8.h>
#include <tinyxml2.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <regex>

#include "directory_scanner.h"

namespace search_engine {

PageProcessor::PageProcessor() : tokenizer_(), hasher_() {
  // 加载停用词表
  std::ifstream cn_ifs{ "corpus/stopwords/cn_stopwords.txt" };
  if (cn_ifs.is_open()) {
    std::string line;
    while (std::getline(cn_ifs, line)) {
      size_t pos = line.find_last_not_of("\r\n\t ");
      if (pos != std::string::npos) {
        line.erase(pos + 1);
      }

      if (!line.empty()) {
        stop_words_.insert(line);
      }
    }
  } else {
    std::cerr << "Error: 无法打开中文停用词文件(PageProcessor函数)." << std::endl;
  }

  std::ifstream en_ifs{ "corpus/stopwords/en_stopwords.txt" };
  if (en_ifs.is_open()) {
    std::string line;
    while (std::getline(en_ifs, line)) {
      size_t pos = line.find_last_not_of("\r\n\t ");
      if (pos != std::string::npos) {
        line.erase(pos + 1);
      }

      if (!line.empty()) {
        stop_words_.insert(line);
      }
    }
  } else {
    std::cerr << "Error: 无法打开英文停用词文件(PageProcessor函数)." << std::endl;
  }
  
}

void PageProcessor::Process(const std::string& dir) {
  // 提取文档
  ExtractDocuments(dir);
  // 文档去重
  DeduplicateDocuments();
  // 生成网页库和偏移库
  BuildPagesAndOffsets("data/pages.dat", "data/offsets.dat");
  // 生成倒排索引
  BuildInvertedIndex("data/inverted_index.dat");
}

void PageProcessor::ExtractDocuments(const std::string& dir) {
  // 获取所有 XML 文件路径
  std::vector<std::string> files = DirectoryScanner::Scan(dir);
  // 解析每个 XML文件
  for (const auto& file : files) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(file.c_str()) != tinyxml2::XML_SUCCESS) {
      continue;
    }

    // 获取根元素
    tinyxml2::XMLElement* root = doc.RootElement();
    if (root == nullptr) { continue; }

    // 遍历所有 <item> 标签
    tinyxml2::XMLElement* channel = root->FirstChildElement("channel");
    if (channel == nullptr) { continue; };

    for (tinyxml2::XMLElement* item = channel->FirstChildElement("item");
         item != nullptr;
         item = item->NextSiblingElement("item")) {
      // 提取 link
      tinyxml2::XMLElement* link_elem = item->FirstChildElement("link");
      const char* link = link_elem ? link_elem->GetText() : nullptr;
      // 提取 title
      tinyxml2::XMLElement* title_elem = item->FirstChildElement("title");
      const char* title = title_elem ? title_elem->GetText() : nullptr;
      // 提取内容
      tinyxml2::XMLElement* content_elem = item->FirstChildElement("content");
      const char* content = nullptr;
      if (content_elem) {
        content = content_elem->GetText();
      } else {
        tinyxml2::XMLElement* desc_elem = item->FirstChildElement("description");
        content = desc_elem ? desc_elem->GetText() : nullptr;
      }
      // 如果没有内容则跳过该 item
      if (content == nullptr) {
        continue;
      }

      // 对内容进行正则表达式
      std::string str_content = content;
      std::regex reg("<[^>]+>");
      str_content = std::regex_replace(str_content, reg, "");

      // 存入数据
      Document new_doc;
      new_doc.id = documents_.size() + 1;
      new_doc.link = link ? link : "";
      new_doc.title = title ? title : "";
      new_doc.content = content;
      documents_.push_back(new_doc);
    }
  }
}

void PageProcessor::DeduplicateDocuments() {
  // 创建向量存储去重后的文档
  std::vector<Document> unique_docs;
  std::vector<uint64_t> hash_codes;

  // 遍历所有的文档
  for (const auto& doc : documents_) {
    // 计算 simhash 值
    std::string text = doc.title + " " + doc.content;
    uint64_t hash_code;
    size_t top_n = 5;
    hasher_.make(text, top_n, hash_code);

    // 比较文档
    bool is_duplicate = false;
    for (uint64_t existing_hash : hash_codes) {
      if (simhash::Simhasher::isEqual(hash_code, existing_hash, 3)) {
        is_duplicate = true;
        break;
      }
    }

    // 是否保留
    if (!is_duplicate) {
      unique_docs.push_back(doc);
      hash_codes.push_back(hash_code);
    }
  }

  // 更新文档ID并替换documents_
  documents_ = std::move(unique_docs);
  for (size_t i = 0; i < documents_.size(); ++i) {
    documents_[i].id = i + 1;
  }
}

void PageProcessor::BuildPagesAndOffsets(const std::string& pages_file,
                                         const std::string& offsets_file) {
  // 生成网页库与网页偏移库
  std::ofstream pages_ofs(pages_file);
  if (!pages_ofs.is_open()) {
    std::cerr << "Error: 无法打开文件 " << pages_file << std::endl;
    return;
  }
  std::ofstream offsets_ofs(offsets_file);
  if (!offsets_ofs.is_open()) {
    std::cerr << "Error: 无法打开文件 " << offsets_file << std::endl;
    return;
  }

  // 遍历 documents_
  for (const auto& doc : documents_) {
    // 记录当前文件偏移量
    std::streampos start_pos = pages_ofs.tellp();

    // 写入网页库
    pages_ofs << "<doc>\n";
    pages_ofs << "  <id>" << doc.id << "</id>\n";
    pages_ofs << "  <link>" << doc.link << "</link>\n";
    pages_ofs << "  <title>" << doc.title << "</title>\n";
    pages_ofs << " <content>" << doc.content << "</content>\n";
    pages_ofs << "</doc>\n";

    // 记录文档结束偏移量
    std::streampos end_pos = pages_ofs.tellp();

    // 写入网页偏移库
    offsets_ofs << doc.id << " "
                << static_cast<long long>(start_pos) << " "
                << static_cast<long long>(end_pos - start_pos) << "\n";
  }
}

void PageProcessor::BuildInvertedIndex(const std::string& index_file) {
  // 1. 统计频率
  std::map<int, std::map<std::string, int>> doc_word_freq;  // 统计单个文档词频 
  std::map<std::string, int> word_doc_count;                // 统计所有文档词频

  for (const auto& doc : documents_) {
    // 分词
    std::string text = doc.title + " " + doc.content;
    std::vector<std::string> words;
    tokenizer_.Cut(text, words);

    // 统计单个文档词频
    std::set<std::string> unique_words; 
    for (const auto& word : words) {
      // 过滤停用词
      if (stop_words_.find(word) != stop_words_.end()) {
        continue;
      }

      // 过滤空字符串
      if (word.empty()) { continue; };

      // 只保留包含汉字的词 
      bool contains_chinese = false;
      const char* it = word.c_str();
      const char* end = word.c_str() + word.size();

      while (it != end) {
        char32_t codepoint = utf8::next(it, end);
        if (codepoint >= 0x4E00 && codepoint <= 0x9FFF) {
          contains_chinese = true;
          break;
        }
      }

      if (!contains_chinese) {
        continue;
      }

      // 词频统计
      doc_word_freq[doc.id][word]++;
      unique_words.insert(word);
    }

    // 统计所有文档词频
    for (const auto& word : unique_words) {
      word_doc_count[word]++;
    }
  }

  // 2. 计算权重
  int total_docs = documents_.size();
  std::map<int, std::map<std::string, double>> doc_weights; 

  for (const auto& [doc_id, word_freq] : doc_word_freq) {
    for (const auto& [word, tf] : word_freq) {                          // TF (Term Frequency): 词语在文档中出现的频率。
      int df = word_doc_count[word];                                    // DF (Document Frequency): 包含词语的文档个数。
      double idf = std::log2(static_cast<double>(total_docs) / df);     // 逆文档频率
      double weight = tf * idf;                                         // 单个关键字 TF-IDF 权重
      doc_weights[doc_id][word] = weight;                               // 每个关键字 TF-IDF 权重 
    }
  }

  // 3. 权重归一化
  for (auto& [doc_id, weights] : doc_weights) {
    // 计算权重平方和
    double sum_squares = 0.0;
    for (const auto& [word, weight] : weights) {
      sum_squares += weight * weight;
    }
    
    // 归一化
    double norm = std::sqrt(sum_squares);
    if (norm > 0) {
      for (auto& [word, weight] : weights) {
        weight /= norm;
      }
    }
  }

  // 4. 构建倒排索引
  for (const auto& [doc_id, weights] : doc_weights) {
    for (const auto& [word, weight] : weights) {
      inverted_index_[word][doc_id] = weight;
    }
  }

  // 5. 写入文件
  std::ofstream ofs(index_file);
  for (const auto& [word, doc_weights] : inverted_index_) {
    ofs << word;
    for (const auto& [doc_id, weight] : doc_weights) {
      ofs << " " << doc_id << " " << weight;
    }
    ofs << "\n";
  }
}

}  // namespace search_engine
