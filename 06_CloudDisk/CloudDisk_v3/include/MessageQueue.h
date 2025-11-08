 /**
  * @file MessageQueue.h
  * @brief 消息队列客户端封装类（单例模式）
  */

#pragma once

#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include <string>
#include <mutex>

class MessageQueue
{
public:
    // 获取单例实例
    static MessageQueue* GetInstance();

    // 初始化消息队列链接
    static bool Initialize(const std::string& host = "127.0.0.1",
                           int port = 5672,
                           const std::string& username = "guest",
                           const std::string& password = "guest");

    // 清理资源
    static void Shutdown();

    // 发送消息到队列
    bool PublishMessage(const std::string& message);

    // 禁止拷贝
    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;

private:
    // 私有构造析构
    MessageQueue(): initialized_(false) {};
    ~MessageQueue();

    // 初始化连接和队列
    bool InitializeChannel(const std::string& host,
                           int port,
                           const std::string& username,
                           const std::string& password);
    
    // 静态成员变量
    static MessageQueue* instance_;
    static std::mutex mutex_;

    // 普通成员变量
    AmqpClient::Channel::ptr_t channel_;
    std::string exchange_name_;
    std::string routing_key_;
    bool initialized_;
};