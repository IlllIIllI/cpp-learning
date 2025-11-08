#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include "OssClient.h"
#include "OssConfig.h"
#include "nlohmann/json.hpp"
#include <signal.h>
#include <unistd.h> 

#include <exception>
#include <iostream>
#include <atomic>
#include <string>
#include <fstream>

using namespace AmqpClient;

static std::atomic<bool> g_running{ true };

void sig_handler(int)
{
    g_running = false;
    std::cout << "\nShutting down..." << std::endl;
}

int main()
{
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    try {
        // 初始化 OSS
        OssConfig oss_config = OssConfig::LoadFromJson("./config/config.json");
        if (oss_config.IsValid()) {
            if (OssClient::Initialize(oss_config)) {
                std::cout << "OSS backup enabled" << std::endl;
            } else {
                std::cerr << "Warning: OSS initialization failed, backup disabled" << std::endl;
            }
        } else {
            std::cerr << "Warning: OSS config not set, backup disabled" << std::endl;
        }

        // 连接 RabbitMQ
        AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create(
            "127.0.0.1",
            5672,
            "guest",
            "guest",
            "/"
        );
        std::cout << "Connected to RabbitMQ" << std::endl;

        // 声明队列
        std::string queue_name = "oss.queue";
        channel->DeclareQueue(queue_name, false, true, false, false);

        // 订阅队列, 手动确认
        std::string consumer_tag = channel->BasicConsume(queue_name, "", false, false, false);
        std::cout << "Waiting for messages..." << std::endl;

        // 循环接收消息
        while (g_running) {
            Envelope::ptr_t envelope;
            // 接收消息
            if (channel->BasicConsumeMessage(consumer_tag, envelope, 1000)) {
                // 判断消息是否有效
                if (!envelope || !envelope->Message()) {
                    channel->BasicReject(envelope, false);
                    continue;
                }

                // 获取消息内容
                std::string message_body = envelope->Message()->Body();
                try {
                    // 解析 JSON 消息
                    auto task = nlohmann::json::parse(message_body);
                    // 文件路径
                    std::string file_path = task["file_path"];
                    std::string file_hash = task["file_hash"];

                    // 读取文件内容
                    std::ifstream file(file_path, std::ios::binary);
                    if (!file.is_open()) {
                        std::cerr << "File not found: " << file_path << std::endl;
                        channel->BasicReject(envelope, false);
                        continue;
                    }
                    std::string file_content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
                    // 关闭文件流
                    file.close();

                    std::cout << "Processing: " << file_hash 
                               << " (" << file_content.size() << " bytes)" << std::endl;

                    // 获取 OSS 实例
                    OssClient* oss_client = OssClient::GetInstance();
                    // 判断文件是否存在
                    if (!oss_client->FileExists(file_hash)) {
                        // 上传文件
                        if (oss_client->UploadFile(file_hash, file_content)) {
                            std::cout << "✓ Successfully backed up: " << file_hash << std::endl;
                            channel->BasicAck(envelope);
                        } else {
                            std::cerr << "Failed to backup: " << file_hash << std::endl;
                            channel->BasicReject(envelope, true);
                        }

                    } else {
                        std::cerr << "File already exists in OSS: " << file_hash << std::endl;
                        channel->BasicAck(envelope);
                    }

                } catch (std::exception& e) {
                    std::cerr << "Error processing message:" << e.what() << std::endl;
                    channel->BasicReject(envelope, false);
                }
            }
        }
        // 清理资源
        std::cout << "Shutting down consumer..." << std::endl;
        // 取消订阅
        channel->BasicCancel(consumer_tag);
        // 清理 OSS 资源
        OssClient::Shutdown();
    } catch (std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Consumer stopped" << std::endl;
    return 0;
}