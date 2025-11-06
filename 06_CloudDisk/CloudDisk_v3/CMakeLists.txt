###############################################################################
# CMake 配置文件 - 云盘服务器项目
# 项目名称: CloudDisk
# 描述: 基于 wfrest 框架的云盘HTTP服务器
###############################################################################

# 设置CMake的最小版本
cmake_minimum_required(VERSION 3.15)

# 项目信息
# 项目可以包含多个目标(target): 可执行程序、静态库、动态库
project(CloudDisk
    VERSION 1.0.0
    DESCRIPTION "Cloud Disk Server based on wfrest"
    LANGUAGES CXX
)

# 指定C++标准版本
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用编译器特定扩展，确保代码可移植性

# 输出编译信息
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

###############################################################################
# 目录配置
###############################################################################

# 添加头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)  # 可执行文件
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)  # 静态库
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)  # 动态库

###############################################################################
# 源文件配置
###############################################################################

# 定义源文件列表
set(SERVER_SOURCES
    src/CloudiskServer.cpp
    src/main.cpp
    src/crypto_util.cc
    src/OssClient.cpp
)

# 定义头文件列表（可选，便于IDE识别）
set(SERVER_HEADERS
    include/CloudiskServer.h
    include/crypto_util.h
    include/user.h
    include/OssClient.h
    include/OssConfig.h
)

###############################################################################
# 目标配置
###############################################################################

# 生成可执行程序
add_executable(server ${SERVER_SOURCES} ${SERVER_HEADERS})

# 设置编译选项
target_compile_options(server PRIVATE
    -fno-rtti           # 禁用运行时类型识别（减小二进制体积）
    -g                  # 生成调试信息
    -Wall               # 启用所有警告
    -Wextra             # 启用额外警告
)

# 根据编译类型设置不同的编译选项
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Debug mode: Adding debug flags")
    target_compile_options(server PRIVATE -O0)  # 不优化，便于调试
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Release mode: Adding optimization flags")
    target_compile_options(server PRIVATE -O3)  # 最高级别优化
endif()

###############################################################################
# 链接库配置
###############################################################################

# 查找依赖库
find_package(OpenSSL REQUIRED)

# 链接第三方库
# crypto, ssl: OpenSSL库，提供加密和SSL/TLS支持
# jwt: JSON Web Token库，用于用户认证
# wfrest: workflow REST框架，HTTP服务器核心
# alibabacloud-oss-cpp-sdk: 阿里云OSS C++ SDK，用于对象存储
# curl: libcurl库，HTTP/HTTPS网络通信（OSS SDK依赖）
# pthread: POSIX线程库，用于多线程支持
target_link_libraries(server PRIVATE
    crypto
    ssl
    jwt
    wfrest
    alibabacloud-oss-cpp-sdk
    curl
    pthread
)

###############################################################################
# 安装配置（可选）
###############################################################################

# 安装可执行文件
install(TARGETS server
    RUNTIME DESTINATION bin
)

# 安装静态资源
install(DIRECTORY static/
    DESTINATION share/clouddisk/static
)

###############################################################################
# 打印配置信息
###############################################################################

message(STATUS "--------------------------------------")
message(STATUS "Server executable will be created at: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server")
message(STATUS "Include directories: ${PROJECT_SOURCE_DIR}/include")
message(STATUS "--------------------------------------")
