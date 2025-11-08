/**
 * @file main.cpp
 * @brief 云盘服务器程序的主入口
 * @details 负责初始化服务器、注册信号处理器、启动服务并优雅退出
 */

#include "CloudiskServer.h"
#include "OssClient.h"
#include "OssConfig.h"
#include "MessageQueue.h"

#include <iostream>
#include <signal.h>

/**
 * @brief 全局等待组对象
 * @details 用于实现服务器的优雅关闭
 *          初始计数为1，当接收到退出信号时调用 done() 减少计数
 *          主线程通过 wait() 阻塞等待，直到计数变为0
 */
WFFacilities::WaitGroup g_waitGroup { 1 };

/**
 * @brief 信号处理函数
 * @param signum 接收到的信号编号（此处未使用）
 * @details 当接收到 SIGINT 信号（Ctrl+C）时被调用
 *          通过调用 g_waitGroup.done() 通知主线程可以退出
 */
void sig_handler(int)
{
    g_waitGroup.done();
}

int main()
{
    // 注册信号处理器：捕获 Ctrl+C (SIGINT) 信号
    signal(SIGINT, sig_handler);

    // 初始化 OSS 客户端
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

    // 初始化 MessageQueue 
    if (MessageQueue::Initialize()) {
        std::cout << "MessageQueue enabled" << std::endl;
    } else {
        std::cerr << "Warning: MessageQueue initialization failed" << std::endl;
    }

    // 创建云盘服务器对象
    CloudiskServer svr;

    // 注册所有业务模块的路由（静态资源、用户认证、文件操作等）
    svr.register_modules();

    // 启动服务器
    if (svr.track().start(8888) == 0) {
        // 打印所有已注册的路由信息到控制台
        svr.list_routes();

        // 主线程在此阻塞等待，直到接收到退出信号
        g_waitGroup.wait();

        // 收到退出信号后，优雅关闭服务器
        svr.stop();

        // 清理 OSS 资源
        OssClient::Shutdown();

        // 清理 MessageQueue 资源
        MessageQueue::Shutdown();

    } else {
        // 服务器启动失败，输出错误信息
        std::cerr << "Error: server start failed!\n";
    }
    return 0;
}
