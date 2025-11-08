#pragma once

#include "alibabacloud/oss/OssClient.h"
#include "OssConfig.h"
#include <mutex>

// 单例模式
class OssClient
{
public:
    // 获取单例实例
    static OssClient* GetInstance();

    // 初始化单例（使用前调用）
    static bool Initialize(const OssConfig& config);

    // 清理资源（程序退出前调用）
    static void Shutdown();

    // 禁止拷贝赋值
    OssClient(const OssClient&) = delete;
    OssClient& operator=(const OssClient&) = delete;

    // 上传文件到 OSS （从内存中）
    bool UploadFile(const std::string& object_name, const std::string& file_content);

    // 检查文件是否存在于 OSS 中
    bool FileExists(const std::string& object_name);

private:
    // 私有构造
    OssClient(): oss_client_(nullptr), initialized_(false) {}
    // 私有析构
    ~OssClient();

    static OssClient* instance_;                        // 单例实例指针
    static std::mutex mutex_;                           // 线程安全的互斥锁

    AlibabaCloud::OSS::OssClient* oss_client_;          // 阿里云 OSS SDK 客户端指针
    OssConfig config_;                                  // 配置文件对象
    bool initialized_;                                  // 标志是否已经初始化
};