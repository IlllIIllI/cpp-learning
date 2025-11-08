/**
 * @file CloudiskServer.h
 * @brief 云盘服务器类的头文件定义
 * @details 基于 wfrest 框架实现的云盘HTTP服务器，采用装饰器模式封装底层HTTP服务器
 */

#pragma once

#include <wfrest/HttpServer.h>
#include <workflow/WFFacilities.h>

/**
 * @class CloudiskServer
 * @brief 云盘HTTP服务器类
 *
 * @details 设计思想：
 *   - 采用组合模式而非继承：有选择地复用代码而不是继承基类的所有功能
 *   - 使用装饰器模式：对 wfrest::HttpServer 进行包装，保持接口的一致性
 *   - 降低学习成本：CloudiskServer 的用法与 HttpServer 类似，用户容易上手
 *
 * 主要功能模块：
 *   - 静态资源服务（HTML、CSS、JS、图片等）
 *   - 用户注册/登录认证
 *   - 用户信息管理
 *   - 文件上传/下载
 *   - 文件列表管理
 */
class CloudiskServer
{
public:
    /**
     * @brief 默认构造函数
     */
    CloudiskServer() {}

    /**
     * @brief 注册所有业务模块的路由
     * @details 统一注册所有功能模块，包括静态资源、用户认证、文件操作等
     */
    void register_modules();

    /**
     * @brief 启动HTTP服务器
     * @param port 监听端口号
     * @return 成功返回0，失败返回非0值
     */
    int start(unsigned short port) {
        return m_server.start(port);
    }

    /**
     * @brief 停止HTTP服务器
     */
    void stop() {
        m_server.stop();
    }

    /**
     * @brief 列出所有已注册的路由信息
     * @details 用于调试和查看当前服务器配置的所有路由规则
     */
    void list_routes() {
        m_server.list_routes();
    }

    /**
     * @brief 开启路由追踪功能
     * @return 返回自身引用，支持链式调用
     * @details 启用后可以追踪每个请求的路由匹配过程，便于调试
     */
    CloudiskServer& track()
    {
        m_server.track();
        return *this;
    }

private:
    /**
     * @brief 注册静态资源路由模块
     * @details 配置HTML页面、CSS、JavaScript、图片等静态资源的访问路由
     */
    void register_static_resources_module();

    /**
     * @brief 注册用户注册模块
     * @details 处理用户注册相关的HTTP请求
     */
    void register_signup_module();

    /**
     * @brief 注册用户登录模块
     * @details 处理用户登录认证相关的HTTP请求
     */
    void register_signin_module();

    /**
     * @brief 注册用户信息管理模块
     * @details 处理用户信息查询、修改等操作
     */
    void register_userinfo_module();

    /**
     * @brief 注册文件上传模块
     * @details 处理文件上传相关的HTTP请求
     */
    void register_fileupload_module();

    /**
     * @brief 注册文件列表模块
     * @details 处理文件列表查询、搜索等操作
     */
    void register_filelist_module();

    /**
     * @brief 注册文件下载模块
     * @details 处理文件下载相关的HTTP请求
     */
    void register_filedownload_module();

private:
    /**
     * @brief 内部封装的HTTP服务器对象
     * @details 采用组合模式，通过成员变量持有 HttpServer 实例
     *          命名避免暴露实现细节（不使用 m_wfrestServer），便于后续更换底层实现
     */
    wfrest::HttpServer m_server;
};

