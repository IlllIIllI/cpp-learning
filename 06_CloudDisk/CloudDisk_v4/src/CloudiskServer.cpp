/**
 * @file CloudiskServer.cpp
 * @brief 云盘服务器类的实现文件
 * @details 实现了CloudiskServer类的各个功能模块，包括路由注册和请求处理逻辑
 */

#include "CloudiskServer.h"
#include "crypto_util.h"
#include "workflow/MySQLResult.h"
#include "nlohmann/json.hpp"
#include "wfrest/CodeUtil.h"
#include "wfrest/PathUtil.h"
#include "wfrest/FileUtil.h"
#include "OssClient.h"
#include "MessageQueue.h"

#include <string>
#include <map>
#include <vector>

using namespace wfrest;

/**
 * @brief 注册所有业务模块的路由
 * @details 按照功能模块依次注册各个路由处理器
 *          目前已实现的模块：
 *          - 静态资源服务模块
 *          TODO: 待实现的模块
 *          - 用户注册模块
 *          - 用户登录模块
 *          - 用户信息模块
 *          - 文件上传模块
 *          - 文件列表模块
 *          - 文件下载模块
 */
void CloudiskServer::register_modules()
{
    // 注册静态资源路由（HTML、CSS、JS、图片等）
    register_static_resources_module();

    // TODO: 后续需要实现以下模块的注册
    register_signup_module();        // 用户注册
    register_signin_module();        // 用户登录
    register_userinfo_module();      // 用户信息
    register_fileupload_module();    // 文件上传
    register_filelist_module();      // 文件列表
    register_filedownload_module();  // 文件下载
}

/**
 * @brief 注册静态资源路由模块
 * @details 配置所有静态资源的HTTP GET路由，包括：
 *          - HTML页面（注册页、登录页、首页等）
 *          - JavaScript脚本文件
 *          - 图片资源
 *          - 文件上传相关的静态资源
 *
 * 路由规则说明：
 *   - 使用 Lambda 表达式作为路由处理函数
 *   - resp->File() 方法返回指定路径的文件内容
 *   - m_server.Static() 用于映射整个目录的静态资源
 */
void CloudiskServer::register_static_resources_module()
{
    // 用户注册页面路由
    // 访问 /user/signup 时返回注册页面
    m_server.GET("/user/signup", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/signup.html");
    });

    // 用户登录页面路由
    // 访问 /static/view/signin.html 时返回登录页面
    m_server.GET("/static/view/signin.html", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/signin.html");
    });

    // 用户首页路由
    // 访问 /static/view/home.html 时返回用户主页
    m_server.GET("/static/view/home.html", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/home.html");
    });

    // 认证相关的JavaScript文件路由
    // 提供客户端认证逻辑的脚本文件
    m_server.GET("/static/js/auth.js", [](const HttpReq *, HttpResp * resp){
        resp->File("static/js/auth.js");
    });

    // 用户头像图片路由
    // 提供默认头像或用户头像资源
    m_server.GET("/static/img/avatar.jpeg", [](const HttpReq *, HttpResp * resp){
        resp->File("static/img/avatar.jpeg");
    });

    // 文件上传页面路由
    // 访问 /file/upload 时返回文件上传界面
    m_server.GET("/file/upload", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/index.html");
    });

    // 文件上传相关的静态资源目录映射
    // 将 /file/upload_files 路径映射到 static/view/upload_files 目录
    // 该目录包含文件上传组件所需的 CSS、JS 库文件（如 Bootstrap、fileinput 等）
    m_server.Static("/file/upload_files","static/view/upload_files");

    // 静态资源路由 - CSS 文件
    // 将 /static/css 路径映射到 static/css 目录
    m_server.Static("/static/css", "static/css");

    // 静态资源路由 - JS 文件
    // 将 /static/js 路径映射到 static/js 目录
    m_server.Static("/static/js", "static/js");

    // 静态资源路由 - 图片文件
    // 将 /static/img 路径映射到 static/img 目录
    m_server.Static("/static/img", "static/img");
}

/**
 * @brief 注册用户注册模块
 * @details 处理用户注册相关的HTTP请求
 */
void CloudiskServer::register_signup_module()
{
    m_server.POST("/user/signup", [](const HttpReq* req, HttpResp* resp) {
        // 判断请求类型是否不为：application/x-www-form-urlencoded
        if (req->content_type() != APPLICATION_URLENCODED) {

#ifndef DEBUG
            std::cout << "请求类型不为: application/x-www-form-urlencoded" << std::endl; 
#endif
            // 返回400
            resp->set_status(HttpStatusBadRequest);
            resp->String("The request type is incorrect.");
            return;
        }

        // 获得请求体数据
        std::map<std::string, std::string> data = req->form_kv();
        std::string username = data["username"];
        std::string password = data["password"];

        // 检查是否已经编码
        if (CodeUtil::is_url_encode(username)) {
            // 解码
            username = CodeUtil::url_decode(username);
        }

#ifndef DEBUG
        std::cout << "username: " << username << ", password: " << password << std::endl;
#endif

        // 验证必填字段
        if (username.empty() || password.empty()) {
            // 返回400
            resp->set_status(HttpStatusBadRequest);
            resp->String("The user password cannot be empty.");
            return;
        }

        // 生成随机盐值与加盐后的哈希密码
        std::string salt = CryptoUtil::generate_salt();
        std::string hashed_pwd = CryptoUtil::hash_password(password, salt);

        // 拼接sql语句
        std::string sql = "INSERT INTO tbl_user (username, password, salt) VALUES ('"
                        + username + "', '" + hashed_pwd + "', '" + salt + "')";
        
        // 执行 MySQL 插入
        resp->MySQL("mysql://root:111@localhost/cloud_disk", sql,
            [resp](protocol::MySQLResultCursor* cursor) {
                int status = cursor->get_cursor_status();

                if (status == MYSQL_STATUS_OK) {
                    unsigned long long affected = cursor->get_affected_rows();
                    if (affected == 1) {
                        resp->String("SUCCESS");
                    } else {
                        resp->String("Registration failed. Please try again.");
                    }
                } else {
                    resp->set_status(HttpStatusConflict);
                    resp->String("The username already exists. Please choose another one.");
                }
            }
        );
    });
}

/**
* @brief 注册用户登录模块
* @details 处理用户登录认证相关的HTTP请求
*/
void CloudiskServer::register_signin_module()
{
    m_server.POST("/user/signin",
        [](const HttpReq* req, HttpResp* resp) {
            // 判断请求头类似是否为：application/x-www-form-urlencoded; charset=UTF-8
            if (req->content_type() != APPLICATION_URLENCODED) {

#ifndef DEBUG
                std::cout << "请求头类型不为: application/x-www-form-urlencoded; charset=UTF-8" << std::endl;
#endif

                // 返回响应
                resp->set_status(HttpStatusBadRequest);
                resp->String("The request type is incorrect.");
                return;
            }

            // 获得请求体数据
            std::map<std::string, std::string> data = req->form_kv();
            std::string username = data["username"];
            std::string password = data["password"];

            // 解码
            if (CodeUtil::is_url_encode(username)) {
                username = CodeUtil::url_decode(username);
            }

#ifndef DEBUG
            std::cout << "username: " << username << ", password: " << password << std::endl;
#endif

            // 验证必填字段是否为空
            if (username.empty() || password.empty()) {
                resp->set_status(HttpStatusBadRequest);
                resp->String("The user password cannot be empty.");
                return;
            }

            // 拼接sql语句
            std::string sql = "SELECT * FROM tbl_user WHERE username = '"
                            + username + "'";

            // 执行 MySQL 查询
            resp->MySQL("mysql://root:111@localhost/cloud_disk", sql,
                [resp, password](protocol::MySQLResultCursor* cursor) {
                    int status = cursor->get_cursor_status();

                    // 判断查询是否失败（只有 MYSQL_STATUS_ERROR 是失败状态）
                    if (status == MYSQL_STATUS_ERROR) {
                        resp->set_status(HttpStatusInternalServerError);
                        resp->String("Database query failed.");
                        return;
                    }

                    // 判断结果集是否为0
                    if (cursor->get_rows_count() == 0) {
                        resp->set_status(HttpStatusUnauthorized);
                        resp->String("Username or password is incorrect.");
                        return;
                    }

                    // 得到查询数据
                    std::vector<protocol::MySQLCell> row;
                    if (cursor->fetch_row(row)) {

                        User user;
                        user.id_ = row[0].as_int();
                        user.username_ = row[1].as_string();
                        std::string db_hashcode = row[2].as_string();
                        std::string salt = row[3].as_string();
                        user.createdAt_ = row[4].as_datetime();

                        // 验证密码
                        std::string hashed_pwd = CryptoUtil::hash_password(password, salt);
                        if (hashed_pwd != db_hashcode)
                        {
                            resp->set_status(HttpStatusUnauthorized);
                            resp->String("Username or password is incorrect.");
                            return;
                        }

                        // 验证通过，生成 token 返回 JSON 格式数据
                        std::string token = CryptoUtil::generate_token(user);
                        nlohmann::json response = {
                            {"data", {
                                {"Uid", user.id_},
                                {"Username", user.username_},
                                {"Token", token},
                                {"Location", "/static/view/home.html"}
                            }}
                        };

                        resp->set_status(HttpStatusOK);
                        resp->String(response.dump(2)); 
                    } else {
                        resp->set_status(HttpStatusInternalServerError);
                        resp->String("Failed to fetch user data from database.");
                        return;
                    }
                }
            );
        }
    );
}

/**
 * @brief 注册用户信息管理模块
 * @details 处理用户信息查询、修改等操作
 */
void CloudiskServer::register_userinfo_module()
{
    m_server.GET("/user/info", [](const HttpReq* req, HttpResp* resp) {
        // 得到查询参数
        std::string username = req->query("username");
        std::string token = req->query("token");

#ifndef DEBUG
        std::cout << "username: " << username << std::endl;
        std::cout << "token: " << token << std::endl;
#endif

        // 校验参数是否存在
        if (username.empty() || token.empty()) {
            resp->set_status(HttpStatusBadRequest);
            resp->String("Token and username are required.");
            return;
        }

        // 校验token是否正确
        User user;
        if (!CryptoUtil::verify_token(token, user)) {
            resp->set_status(HttpStatusUnauthorized);
            resp->String("Unauthorized");
            return;
        }

        // token验证通过，返回json数据
        nlohmann::json response = {
            { "data", {
                { "Username", user.username_ },
                { "SignupAt", user.createdAt_ }
            } }
        };

        resp->set_status(HttpStatusOK);
        resp->String(response.dump(2));
    });
}

/**
 * @brief 注册文件列表查询模块
 * @details 处理文件列表查询操作
 */
void CloudiskServer::register_filelist_module()
{
    m_server.POST("/file/query",
        [](const HttpReq* req, HttpResp* resp) {
            // 判断请求头类型： x-www-form-urlencoded
            if (req->content_type() != APPLICATION_URLENCODED) {

#ifndef DEBUG
                std::cout << "请求头类型不为: x-www-form-urlencoded" << std::endl;
#endif

                resp->set_status(HttpStatusBadRequest);
                resp->String("The request type is incorrect.");
                return;
            }

            // 解析url参数
            std::string username = req->query("username");
            std::string token = req->query("token");

#ifndef DEBUG
            std::cout << "username: " << username << std::endl;
            std::cout << "token: " << token << std::endl;
#endif

            if (username.empty() || token.empty()) {
                resp->set_status(HttpStatusBadRequest);
                resp->String("Token and username are required.");
                return;
            }

            // 参数存在，校验并解析token
            User user;
            if (!CryptoUtil::verify_token(token, user)) {
                resp->set_status(HttpStatusUnauthorized);
                resp->String("Token error or expired.");
                return;
            }

            // 解析请求体数据
            std::map<std::string, std::string> form_data = req->form_kv();
            int limit = 15; // 默认显示15行
            if (!form_data.empty()) {
                limit = std::stoi(form_data["limit"]);
            }

            // 查询 MySQL 文件表
            std::string sql =
                "select hashcode, filename, size, created_at, last_update from tbl_file where uid = "
                + std::to_string(user.id_) + " order by created_at desc limit "
                + std::to_string(limit);

#ifndef DEBUG
            std::cout << "sql: " << sql << std::endl;
#endif

            resp->MySQL("mysql://root:111@localhost/cloud_disk", sql,
                [resp](protocol::MySQLResultCursor* cursor) {
                    // 得到状态
                    int status = cursor->get_cursor_status();

                    // 判断查询是否失败
                    if (status == MYSQL_STATUS_ERROR) {
                        resp->set_status(HttpStatusInternalServerError);
                        resp->String("Database query failed.");
                        return;
                    }

                    int rows_count = cursor->get_rows_count();
                    // 判断结果集是否为0
                    if (rows_count == 0) {
                        nlohmann::json empty_array = nlohmann::json::array();
                        resp->set_status(HttpStatusOK);
                        resp->String(empty_array.dump());
                        return;
                    }

                    // 遍历结果集
                    std::vector<protocol::MySQLCell> row;
                    nlohmann::json file_list = nlohmann::json::array();

                    while (cursor->fetch_row(row)) {
                        file_list.push_back({
                            {"FileHash", row[0].as_string()},           // 文件的哈希值
                            {"FileName", row[1].as_string()},           // 文件的名字
                            {"FileSize", row[2].as_ulonglong()},        // 文件的大小
                            {"UploadAt", row[3].as_datetime()},         // 上传时间
                            {"LastUpdated", row[4].as_datetime()}       // 最近修改的时间
                        });
                    }

                    resp->set_status(HttpStatusOK);
                    resp->String(file_list.dump(2));
                }
            );
        }
    );
}

/**
 * @brief 注册文件上传模块
 * @details 处理文件上传相关的HTTP请求
 */
void CloudiskServer::register_fileupload_module()
{
    m_server.POST("/file/upload", [](const HttpReq* req, HttpResp* resp) {
        // 判断请求头类型
        if (req->content_type() != MULTIPART_FORM_DATA) {
            resp->set_status(HttpStatusBadRequest);
            resp->String("Request body error");
            return;
        }

        // 解析url查询参数
        std::string username = req->query("username");
        std::string token = req->query("token");

        if (username.empty() || token.empty()) {
            resp->set_status(HttpStatusBadRequest);
            resp->String("Token and username are required.");
            return;
        }

        // 校验并解析token
        User user;
        if (!CryptoUtil::verify_token(token, user)) {
            resp->set_status(HttpStatusUnauthorized);
            resp->String("Token error or expired.");
            return;
        }

        // 校验成功
        const Form& form = req->form();    // Form: std::map<std::string, std::pair<std::string, std::string>>;
        // 获得文件信息（文件名 、文件内容）
        for (const auto& [_, file_info]: form) {
            // 分割文件名与文件内容
            const auto& [filename, content] = file_info;
            // 处理文件名
            if (filename.find('/') != std::string::npos) {
                resp->set_status(HttpStatusBadRequest);
                resp->String("The file name is not compliant.");
                return;
            }   // 文件名正常

            // 通过文件内容生成哈希值
            std::string file_hash = CryptoUtil::hash_data(content);
            if (file_hash.empty()) {
                resp->set_status(HttpStatusBadRequest);
                resp->String("There is an error in the document.");
                return;
            }
            

            // 写入数据库
            std::string sql = "INSERT INTO tbl_file (uid, hashcode, filename, size) "
                              "VALUES (" + std::to_string(user.id_) + ", '"
                              + file_hash + "', '"
                              + filename + "', "
                              + std::to_string(content.size()) + ")";

            resp->MySQL("mysql://root:111@localhost/cloud_disk", sql,
                [resp](protocol::MySQLResultCursor* cursor) {
                    // 得到状态
                    int status = cursor->get_cursor_status();
                    // 判断 MySQL 插入是否失败
                    if (status == MYSQL_STATUS_ERROR || cursor->get_affected_rows() == 0) {
                        resp->set_status(HttpStatusInternalServerError);
                        resp->String("Data insertion failed, There may be duplicate files.");
                        return;
                    }

                    // 跳转到用户首页
                    resp->set_status(HttpStatusSeeOther);
                    resp->headers["Location"] = "/static/view/home.html";
                }
            );

            // 确保上传目录存在
            std::string upload_dir = "../user_uploads";
            FileUtil::create_directories(upload_dir);

            // 判断文件是否已经存在
            std::string file_path = upload_dir + "/" + file_hash;
            if (FileUtil::file_exists(file_path)) {
                return;
            }

            // 写入本地磁盘
            std::string oss_content = content;
            resp->Save(file_path, std::move(content));


            // 上传 OSS 
            auto oss_client = OssClient::GetInstance();
            if (oss_client) {
                if (!oss_client->FileExists(file_hash)) {
                    auto mq = MessageQueue::GetInstance();

                    if (mq) {
                        nlohmann::json task = {
                            { "file_hash", file_hash },
                            { "file_content", oss_content }
                        };

                        // 发送消息
                        if (mq->PublishMessage(task.dump())) {
                            std::cout << "The backup task has been sent." << std::endl;
                        } else {
                            std::cerr << "Fail to send" << std::endl;
                        }
                    }

                } else {
                    std::cerr << "File already exists in OSS, skip upload: " << file_hash << std::endl;
                }
            }
        }
    });
}

/**
 * @brief 注册文件下载模块
 * @details 处理文件下载相关的HTTP请求
 */
void CloudiskServer::register_filedownload_module()
{
    m_server.GET("/file/download", [](const HttpReq* req, HttpResp* resp) {
        // 解析 url 参数
        std::string filename = req->query("filename");
        std::string filehash = req->query("filehash");
        std::string username = req->query("username");
        std::string token = req->query("token");

        if (
            filename.empty() ||
            filehash.empty() ||
            username.empty() ||
            token.empty()
            ) 
        {
            resp->set_status(HttpStatusBadRequest);
            resp->String("The request parameters are incorrect.");
            return;
        }

        // 解码username
        if (CodeUtil::is_url_encode(username)) {
            username = CodeUtil::url_decode(username);
        }

        // 校验并解析token
        User user;
        if (!CryptoUtil::verify_token(token, user)) {
            resp->set_status(HttpStatusUnauthorized);
            resp->String("Token error or expired.");
            return;
        } // token 验证通过

        // 获得文件真实路径并判断是否存在该文件
        std::string file_path = "../user_uploads/" + filehash;
        if (!FileUtil::file_exists(file_path)) {
            resp->set_status(HttpStatusNotFound);
            resp->String("File not found.");
            return;
        } // 找到文件
        
        // 设置下载文件名
        resp->add_header("Content-Type", "application/octet-stream");
        resp->add_header("Content-Disposition", "attachment; filename=\"" + filename + "\"");
        // 发送文件
        resp->File(file_path);
    });
}