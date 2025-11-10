// Copyright 2025 Search Engine Project
// Author: Your Name

#include "page_processor.h"

#include <tinyxml2.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#include "directory_scanner.h"

namespace search_engine {

PageProcessor::PageProcessor() : tokenizer_(), hasher_() {
  // TODO: 加载停用词表
  // 从 corpus/stopwords/cn_stopwords.txt 和 en_stopwords.txt
  // 加载停用词到 stop_words_
  //
  // 提示:
  // std::ifstream ifs("corpus/stopwords/cn_stopwords.txt");
  // std::string line;
  // while (std::getline(ifs, line)) {
  //   if (!line.empty()) {
  //     stop_words_.insert(line);
  //   }
  // }
  //
  // 对英文停用词做同样的处理
}

void PageProcessor::Process(const std::string& dir) {
  // TODO: 实现处理流程
  // 1. 提取文档: ExtractDocuments(dir)
  // 2. 文档去重: DeduplicateDocuments()
  // 3. 生成网页库和偏移库: BuildPagesAndOffsets("pages.dat", "offsets.dat")
  // 4. 生成倒排索引: BuildInvertedIndex("inverted_index.dat")
}

void PageProcessor::ExtractDocuments(const std::string& dir) {
  // TODO: 从 XML 文件中提取文档
  // 步骤:
  // 1. 使用 DirectoryScanner::Scan() 获取所有 XML 文件
  //    std::vector<std::string> files = DirectoryScanner::Scan(dir);
  //
  // 2. 使用 tinyxml2 解析每个 XML 文件
  //    for (const auto& file : files) {
  //      tinyxml2::XMLDocument doc;
  //      if (doc.LoadFile(file.c_str()) != tinyxml2::XML_SUCCESS) {
  //        continue;  // 跳过无法解析的文件
  //      }
  //
  // 3. 提取规则:
  //    - 获取根元素
  //      tinyxml2::XMLElement* root = doc.RootElement();
  //      if (root == nullptr) continue;
  //
  //    - 遍历所有 <item> 标签
  //      tinyxml2::XMLElement* channel = root->FirstChildElement("channel");
  //      if (channel == nullptr) continue;
  //      tinyxml2::XMLElement* item = channel->FirstChildElement("item");
  //
  //      while (item != nullptr) {
  //        // 处理每个 item
  //
  //    - 提取 <link> 作为 link
  //        tinyxml2::XMLElement* link_elem = item->FirstChildElement("link");
  //        const char* link = link_elem ? link_elem->GetText() : nullptr;
  //
  //    - 提取 <title> 作为 title
  //        tinyxml2::XMLElement* title_elem = item->FirstChildElement("title");
  //        const char* title = title_elem ? title_elem->GetText() : nullptr;
  //
  //    - 优先使用 <content> 的内容作为 content
  //        tinyxml2::XMLElement* content_elem =
  //            item->FirstChildElement("content");
  //        const char* content = nullptr;
  //        if (content_elem) {
  //          content = content_elem->GetText();
  //        } else {
  //          // 如果没有 <content>，使用 <description>
  //          tinyxml2::XMLElement* desc_elem =
  //              item->FirstChildElement("description");
  //          content = desc_elem ? desc_elem->GetText() : nullptr;
  //        }
  //
  //    - 如果两者都没有，跳过该 <item>
  //        if (content == nullptr) {
  //          item = item->NextSiblingElement("item");
  //          continue;
  //        }
  //
  // 4. 将提取的文档存入 documents_
  //        Document new_doc;
  //        new_doc.id = documents_.size() + 1;
  //        new_doc.link = link ? link : "";
  //        new_doc.title = title ? title : "";
  //        new_doc.content = content;
  //        documents_.push_back(new_doc);
  //
  //        item = item->NextSiblingElement("item");
  //      }
  //    }
}

void PageProcessor::DeduplicateDocuments() {
  // TODO: 使用 simhash 对文档去重
  // 步骤:
  // 1. 创建临时向量存储去重后的文档
  //    std::vector<Document> unique_docs;
  //    std::vector<uint64_t> hash_codes;
  //
  // 2. 遍历所有文档
  //    for (const auto& doc : documents_) {
  //
  // 3. 对每篇文档的 title + content 计算 simhash 值
  //      std::string text = doc.title + " " + doc.content;
  //      uint64_t hash_code;
  //      size_t top_n = 5;
  //      hasher_.make(text, top_n, hash_code);
  //
  // 4. 比较新文档与已保存文档的汉明距离
  //      bool is_duplicate = false;
  //      for (uint64_t existing_hash : hash_codes) {
  //        // 计算汉明距离
  //        int hamming_dist = __builtin_popcountll(hash_code ^ existing_hash);
  //
  // 5. 如果汉明距离 <= 3，认为是重复文档，丢弃
  //        if (hamming_dist <= 3) {
  //          is_duplicate = true;
  //          break;
  //        }
  //      }
  //
  // 6. 否则保留该文档
  //      if (!is_duplicate) {
  //        unique_docs.push_back(doc);
  //        hash_codes.push_back(hash_code);
  //      }
  //    }
  //
  // 7. 更新文档ID并替换 documents_
  //    documents_ = std::move(unique_docs);
  //    for (size_t i = 0; i < documents_.size(); ++i) {
  //      documents_[i].id = i + 1;
  //    }
}

void PageProcessor::BuildPagesAndOffsets(const std::string& pages_file,
                                         const std::string& offsets_file) {
  // TODO: 生成网页库和网页偏移库
  // 步骤:
  // 1. 打开 pages_file 和 offsets_file
  //    std::ofstream pages_ofs(pages_file);
  //    std::ofstream offsets_ofs(offsets_file);
  //
  // 2. 遍历 documents_
  //    for (const auto& doc : documents_) {
  //
  // 3. 对每篇文档:
  //    - 记录当前文件偏移量
  //      std::streampos start_pos = pages_ofs.tellp();
  //
  //    - 写入网页库，格式:
  //      pages_ofs << "<doc>\n";
  //      pages_ofs << "    <id>" << doc.id << "</id>\n";
  //      pages_ofs << "    <link>" << doc.link << "</link>\n";
  //      pages_ofs << "    <title>" << doc.title << "</title>\n";
  //      pages_ofs << "    <content>" << doc.content << "</content>\n";
  //      pages_ofs << "</doc>\n";
  //
  //    - 记录文档结束位置，计算文档大小
  //      std::streampos end_pos = pages_ofs.tellp();
  //      int doc_size = end_pos - start_pos;
  //
  //    - 写入偏移库，格式: <文档ID> <偏移量> <文档大小>
  //      offsets_ofs << doc.id << " " << start_pos << " " << doc_size << "\n";
  //    }
}

void PageProcessor::BuildInvertedIndex(const std::string& index_file) {
  // TODO: 生成倒排索引库
  // 步骤:
  // 1. 统计每个词在每篇文档中的词频(TF)
  //    // 文档ID -> {词 -> 词频}
  //    std::map<int, std::map<std::string, int>> doc_word_freq;
  //    // 词 -> 包含该词的文档数(DF)
  //    std::map<std::string, int> word_doc_count;
  //
  //    for (const auto& doc : documents_) {
  //      // 分词
  //      std::string text = doc.title + " " + doc.content;
  //      std::vector<std::string> words;
  //      tokenizer_.Cut(text, words);
  //
  //      // 统计词频
  //      std::set<std::string> unique_words;
  //      for (const auto& word : words) {
  //        // 过滤停用词
  //        if (stop_words_.find(word) != stop_words_.end()) {
  //          continue;
  //        }
  //        doc_word_freq[doc.id][word]++;
  //        unique_words.insert(word);
  //      }
  //
  //      // 统计文档频率
  //      for (const auto& word : unique_words) {
  //        word_doc_count[word]++;
  //      }
  //    }
  //
  // 2. 计算 TF-IDF 权重
  //    int total_docs = documents_.size();
  //    // 文档ID -> {词 -> TF-IDF权重}
  //    std::map<int, std::map<std::string, double>> doc_weights;
  //
  //    for (const auto& [doc_id, word_freq] : doc_word_freq) {
  //      for (const auto& [word, tf] : word_freq) {
  //        int df = word_doc_count[word];
  //        double idf = std::log2(static_cast<double>(total_docs) / df);
  //        double weight = tf * idf;
  //        doc_weights[doc_id][word] = weight;
  //      }
  //    }
  //
  // 3. 权重归一化
  //    for (auto& [doc_id, weights] : doc_weights) {
  //      // 计算权重平方和
  //      double sum_squares = 0.0;
  //      for (const auto& [word, weight] : weights) {
  //        sum_squares += weight * weight;
  //      }
  //
  //      // 归一化
  //      double norm = std::sqrt(sum_squares);
  //      if (norm > 0) {
  //        for (auto& [word, weight] : weights) {
  //          weight /= norm;
  //        }
  //      }
  //    }
  //
  // 4. 构建倒排索引: inverted_index_[词][文档ID] = 归一化权重
  //    for (const auto& [doc_id, weights] : doc_weights) {
  //      for (const auto& [word, weight] : weights) {
  //        inverted_index_[word][doc_id] = weight;
  //      }
  //    }
  //
  // 5. 写入文件，格式: <关键字> <文档ID> <权重> [<文档ID> <权重>]...
  //    std::ofstream ofs(index_file);
  //    for (const auto& [word, doc_weights] : inverted_index_) {
  //      ofs << word;
  //      for (const auto& [doc_id, weight] : doc_weights) {
  //        ofs << " " << doc_id << " " << weight;
  //      }
  //      ofs << "\n";
  //    }
}

}  // namespace search_engine
