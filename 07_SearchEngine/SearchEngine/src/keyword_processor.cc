// Copyright 2025 Search Engine Project
// Author: Your Name

#include "keyword_processor.h"

#include <utfcpp/utf8.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "directory_scanner.h"

namespace search_engine {

KeyWordProcessor::KeyWordProcessor() : tokenizer_() {
  // 加载中停用词
  std::ifstream cn_ifs{ "corpus/stopwords/cn_stopwords.txt" };
  if (cn_ifs.is_open()) {
    std::string line;
    while (std::getline(cn_ifs, line)) {
      // 寻找不等于空白字符的最后一个字符
      size_t pos = line.find_last_not_of("\r\n\t ");
      if (pos != std::string::npos) {
        // 删除最后一个非空白字符后所有空白字符
        line.erase(pos + 1);
      }
      if (!line.empty())  {
        cn_stop_words_.insert(line);
      }
    }
  } else {
    std::cerr << "Error: 无法打开中文停用词文件." << std::endl;
  }

  // 加载英文停用词
  std::ifstream en_ifs{ "corpus/stopwords/en_stopwords.txt" };
  if (en_ifs.is_open()) {
    std::string line;
    while (std::getline(en_ifs, line)) {
      size_t pos = line.find_last_not_of("\r\n\t ");
      if (pos != std::string::npos) {
        line.erase(pos + 1);
      }
      if (!line.empty()) {
        en_stop_words_.insert(line);
      }
    }
  } else {
    std::cerr << "Error: 无法打开英文停用词文件." << std::endl;
  }
}

void KeyWordProcessor::Process(const std::string& cn_dir,
                                const std::string& en_dir) {
  // 创建中文词典
  CreateChineseDict(cn_dir, "data/cn_dict.dat");
  // 构建中文索引
  BuildChineseIndex("data/cn_dict.dat", "data/cn_index.dat");
  // 创建英文词典
  CreateEnglishDict(en_dir, "data/en_dict.dat");
  // 构建英文索引
  BuildEnglishIndex("data/en_dict.dat", "data/en_index.dat");
}

void KeyWordProcessor::CreateChineseDict(const std::string& dir,
                                         const std::string& output_file) {
  // 获取所有文件
  std::vector<std::string> files = DirectoryScanner::Scan(dir);
  // 词频统计容器
  std::map<std::string, int> word_freq;

  // 读取每个文件内容
  for (const auto& file : files) {
    std::ifstream ifs(file);
    if (!ifs.is_open()) {
      std::cerr << "Error: 无法打开文件 " << file << std::endl;
      continue;
    }

    std::string content{ std::istreambuf_iterator<char>(ifs),
                         std::istreambuf_iterator<char>() };

    // 使用 tokenizer_.Cut() 进行分词
    std::vector<std::string> words;
    tokenizer_.Cut(content, words);
    
    for (const auto& word : words) {
      // 过滤停用词
      if (cn_stop_words_.find(word) != cn_stop_words_.end()) {
        continue;
      }
      
      // 过滤空字符串
      if (word.empty()) {
        continue;
      }

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
      word_freq[word]++;
    }
  }

  // 写入结果到 output_file 
  std::ofstream ofs{ output_file };
  if (!ofs.is_open()) {
    std::cerr << "Error: 无法创建文件 " << output_file << std::endl;
    return;
  }

  for (const auto& [word, freq] : word_freq) {
    ofs << word << " " << freq << "\n";
  }
}

void KeyWordProcessor::BuildChineseIndex(const std::string& dict_file,
                                         const std::string& index_file) {
  // 读取词典文件，为每个词语分配一个 ID
  std::map<std::string, int> word_to_id;
  std::ifstream ifs{ dict_file };
  if (ifs.is_open()) {
    std::string word;
    int freq;
    int word_id = 1;

    while (ifs >> word >> freq) {
      word_to_id[word] = word_id++;
    }
  } else {
    std::cerr << "Error: 无法打开词典文件" << std::endl;
    return;
  }

  // 使用 utfcpp 将每个词语拆分成单个汉字并建立索引库
  std::map<std::string, std::vector<int>> hanzi_to_words;
  for (const auto& [word, id] : word_to_id) {
    const char* it = word.c_str();
    const char* end = word.c_str() + word.size();
    while (it != end) {
      auto start = it;
      utf8::next(it, end);
      std::string hanzi(start, it);
      hanzi_to_words[hanzi].push_back(id);
    }
  }
  
  // 写入索引库
  std::ofstream ofs{ index_file };
  if (!ofs.is_open()) {
    std::cerr << "Error: 无法创建文件" << std::endl;
    return;
  }

  for (const auto& [hanzi, word_ids] : hanzi_to_words) {
    ofs << hanzi;
    for (int id : word_ids) {
      ofs << " " << id;
    }
    ofs << "\n";
  }
}

void KeyWordProcessor::CreateEnglishDict(const std::string& dir,
                                         const std::string& output_file) {
  // 获取所有的文件
  std::vector<std::string> files = DirectoryScanner::Scan(dir);
  // 词频统计
  std::map<std::string, int> word_freq;

  // 读取每个文件内容
  for (const auto& file : files) {
    std::ifstream ifs{ file };
    if (!ifs.is_open()) {
      std::cerr << "Error: 无法打开文件文件." << file << std::endl;
      continue;
    }

    // 得到文件内容
    std::string content{ std::istreambuf_iterator<char>(ifs),
                         std::istreambuf_iterator<char>() };
    // 预处理
    for (char& c : content) {
      if (std::isalpha(static_cast<unsigned char>(c))) {
        c = std::tolower(static_cast<unsigned char>(c));
      } else {
        c = ' ';  // 非字母
      }
    }

    // 分词
    std::istringstream iss{ content };
    std::string word;
    while (iss >> word) {
      // 过滤停用词
      if (en_stop_words_.find(word) == en_stop_words_.end()) {
        word_freq[word]++;
      }
    }
  }

  std::ofstream ofs{output_file};
  if (!ofs.is_open()) {
    std::cerr << "Error: 无法创建文件 " << output_file << std::endl;
    return;
  }

  for (const auto& [word, freq] : word_freq) {
    ofs << word << " " << freq << "\n";
  }
}

void KeyWordProcessor::BuildEnglishIndex(const std::string& dict_file,
                                         const std::string& index_file) {
  std::map<std::string, std::vector<int>> letter_to_words;
  std::ifstream ifs{ dict_file };
  if (ifs.is_open()) {
    std::string word;
    int freq;
    int word_id = 1;

    while (ifs >> word >> freq) {
      // 为当前单词的所有字母添加相同的 word_id
      for (char c : word) {
        letter_to_words[std::string(1, c)].push_back(word_id);
      }
      word_id++;
    }
  } else {
    std::cerr << "Error: 无法打开词典文件 " << dict_file << std::endl;
    return;
  }

  std::ofstream ofs{ index_file };
  if (!ofs.is_open()) {
    std::cerr << "Error: 无法创建文件 " << index_file << std::endl;
    return;
  }

  for (const auto& [letter, word_ids] : letter_to_words) {
    ofs << letter;
    for (int id : word_ids) {
      ofs << " " << id;
    }
    ofs << "\n";
  }
}

}  // namespace search_engine
