#include "MessageQueue.h"
#include <iostream>

// 定义并初始化静态成员
MessageQueue* MessageQueue::instance_ = nullptr;
std::mutex MessageQueue::mutex_;

// 获取单例实例
MessageQueue* MessageQueue::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
            instance_ = new MessageQueue();
        }
    }

    return instance_;
}

// 初始化消息队列链接
bool MessageQueue::Initialize(const std::string& host,
                              int port,
                              const std::string& username,
                              const std::string& password)
{
    return GetInstance()->InitializeChannel(host, port, username, password);
}

// 初始化连接与队列
bool MessageQueue::InitializeChannel(const std::string& host,
                                     int port,
                                     const std::string& username,
                                     const std::string& password)
{
    if (initialized_) {
        return true;
    }

    try {
        // 创建Channel
        channel_ = AmqpClient::Channel::Create(host, port, username, password, "/");

        // 设置配置
        exchange_name_ = "oss.direct";
        routing_key_ = "oss";

        // 声明交换机
        channel_->DeclareExchange(exchange_name_,
                                  AmqpClient::Channel::EXCHANGE_TYPE_DIRECT,
                                  false, true, false);
        // 声明队列
        std::string queue_name = "oss.queue";
        channel_->DeclareQueue(queue_name, false, true, false, false);
        
        // 绑定队列到交换机
        channel_->BindQueue(queue_name, exchange_name_, routing_key_);

        initialized_ = true;
        std::cout << "MessageQueue initialized successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize: " << e.what() << std::endl;
        initialized_ = false;
        return false;
    }
}

// 清理资源
void MessageQueue::Shutdown()
{
    std::lock_guard<std::mutex> lock{ mutex_ };
    if (instance_) {
        delete instance_;
        instance_  = nullptr;
         std::cout << "MessageQueue shutdown successfully" << std::endl;
    }
}

// 发布消息
bool MessageQueue::PublishMessage(const std::string& message)
{
    if (!initialized_) {
        std::cerr << "MessageQueue not initialized" << std::endl;
        return false;
    }

    try {
        // 创建消息对象
        AmqpClient::BasicMessage::ptr_t message_ptr = AmqpClient::BasicMessage::Create(message);
        // 消息持久化
        message_ptr->DeliveryMode(AmqpClient::BasicMessage::dm_persistent);
        // 发布消息
        channel_->BasicPublish(exchange_name_, routing_key_, message_ptr);

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Failed to publish message: " << e.what() << std::endl;
        return false;
    }
}

// 析构
MessageQueue::~MessageQueue()
{
    initialized_ = false;
    if (channel_) {
        channel_.reset();   // 释放智能指针
    }
}