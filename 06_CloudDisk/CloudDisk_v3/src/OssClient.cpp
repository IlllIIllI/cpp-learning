#include "OssClient.h"
#include "OssConfig.h"
#include "alibabacloud/oss/OssClient.h"
#include <iostream>
#include <sstream>
#include <mutex>

// 定义并初始化静态成员
OssClient* OssClient::instance_ = nullptr;
std::mutex OssClient::mutex_;

// 析构
OssClient::~OssClient()
{
    if (oss_client_) {
        delete oss_client_;
        oss_client_ = nullptr;
    }
}

// 初始化单例
bool OssClient::Initialize(const OssConfig& config)
{
    // 检查配置有效性
    if (!config.IsValid()) {
        std::cerr << "OssClient::Initialize failed: Invalid config" << std::endl;
        return false;
    }

    if (!instance_) {
        // 加锁
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
            // 初始化 OSS SDK
            AlibabaCloud::OSS::InitializeSdk();

            // 创建单例实例
            instance_ = new OssClient();
            instance_->config_ = config;

            // 创建 OSS 客户端
            AlibabaCloud::OSS::ClientConfiguration conf;
            instance_->oss_client_ = new AlibabaCloud::OSS::OssClient(
                config.endpoint_,
                config.access_key_id_,
                config.access_key_secret_,
                conf
            );

            // 设置区域
            instance_->oss_client_->SetRegion(config.region_);
            instance_->initialized_ = true;

            std::cout << "OssClient initialized successfully" << std::endl;
        }
    }
    return true;
}

// 获得单例实例
OssClient* OssClient::GetInstance()
{
    return instance_;
}

// 清理资源
void OssClient::Shutdown()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (instance_) {
        delete instance_;
        instance_ = nullptr;

        // 清理 OSS SDK
        AlibabaCloud::OSS::ShutdownSdk();

        std::cout << "OssClient shutdown successfully" << std::endl;
    }
}

// 上传文件到 OSS（从内存中）
bool OssClient::UploadFile(const std::string& object_name, const std::string& file_content)
{
    if (!initialized_ || !oss_client_) {
        std::cerr << "OssClient not initialized" << std::endl;
        return false;
    }

    // 创建内存流
    auto stream = std::make_shared<std::stringstream>(file_content);
    // 构造上传请求
    AlibabaCloud::OSS::PutObjectRequest request(config_.bucket_name_, object_name, stream);
    // 执行上传
    auto outcome = oss_client_->PutObject(request);
    if (!outcome.isSuccess()) {
        std::cerr << "PutObject failed"
                  << ", code: " << outcome.error().Code()
                  << ", message: " << outcome.error().Message()
                  << ", requestId: " << outcome.error().RequestId()
                  << std::endl;

        return false;
    }
    return true;
}

// 检查文件是否存在于 OSS 中
bool OssClient::FileExists(const std::string& object_name)
{
    if (!initialized_ || !oss_client_) {
        std::cerr << "OssClient not initialized" << std::endl;
        return false;
    }

    return oss_client_->DoesObjectExist(config_.bucket_name_, object_name);
}
