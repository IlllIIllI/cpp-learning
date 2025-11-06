#pragma once

#include "nlohmann/json.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <exception>

// OSS 配置结构体
struct OssConfig
{
    std::string endpoint_;              // oss 端点
    std::string access_key_id_;         // 阿里云 AccessKey ID
    std::string access_key_secret_;     // 阿里云 AccessKey Secret
    std::string bucket_name_;           // 存储桶名称
    std::string region_;                // 区域代码

    // 默认构造函数
    OssConfig() = default;

    // 从配置文件中加载数据
    static OssConfig LoadFromJson(const std::string json_path)
    {
        OssConfig config;

        try {
            // 打开配置文件
            std::ifstream file(json_path);
            if (!file.is_open()) {
                std::cerr << "Failed to open config file: " << json_path << std::endl;
            } // 文件打开成功

            // 解析 JSON
            nlohmann::json j;
            file >> j;

            if (j.contains("oss")) {
                auto oss = j["oss"];
                config.endpoint_ = oss.value("endpoint", "");
                config.access_key_id_ = oss.value("access_key_id", "");
                config.access_key_secret_ = oss.value("access_key_secret", "");
                config.bucket_name_ = oss.value("bucket_name", "");
                config.region_ = oss.value("region", "");
            } else {
                std::cerr << "No 'oss' section found in config file" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading config: " << e.what() << std::endl;
        }

        return config;
    }

    // 检查配置是否有效
    bool IsValid() const
    {
        return !endpoint_.empty() &&
               !access_key_id_.empty() &&
               !access_key_secret_.empty() &&
               !bucket_name_.empty() &&
               !region_.empty();
    }
};