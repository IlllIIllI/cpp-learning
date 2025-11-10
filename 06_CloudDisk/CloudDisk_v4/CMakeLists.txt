cmake_minimum_required(VERSION 3.15)

project(CloudDisk_v4_RPC VERSION 4.0.0 LANGUAGES CXX)

# Protobuf v3.21.12 + SRPC v0.10.3 兼容 C++11，但推荐使用 C++17
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
include_directories(${PROJECT_SOURCE_DIR}/include)

# 源文件
set(SERVER_SOURCES
    src/CloudiskServer.cpp
    src/main.cpp
    src/crypto_util.cpp
    src/OssClient.cpp
    src/MessageQueue.cpp
)

set(SERVER_HEADERS
    include/CloudiskServer.h
    include/crypto_util.h
    include/user.h
    include/OssClient.h
    include/OssConfig.h
    include/MessageQueue.h
)

# 编译
add_executable(server ${SERVER_SOURCES} ${SERVER_HEADERS})

target_compile_options(server PRIVATE -fno-rtti -g -Wall -Wextra)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(server PRIVATE -O0)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(server PRIVATE -O3)
endif()

# 查找依赖库
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)

# Protobuf 信息
message(STATUS "Protobuf version: ${Protobuf_VERSION}")
message(STATUS "Protobuf include: ${Protobuf_INCLUDE_DIRS}")
message(STATUS "Protobuf libraries: ${Protobuf_LIBRARIES}")

# 链接库
target_link_libraries(server PRIVATE
    ${Protobuf_LIBRARIES}  # Protobuf v3.21.12 (无 Abseil 依赖)
    srpc                   # SRPC v0.10.3
    workflow               # Sogou Workflow
    crypto
    ssl
    jwt
    wfrest
    alibabacloud-oss-cpp-sdk
    curl
    pthread
    SimpleAmqpClient
    rabbitmq
)

# 安装
install(TARGETS server RUNTIME DESTINATION bin)
install(DIRECTORY static/ DESTINATION share/clouddisk/static)

# 信息
message(STATUS "${PROJECT_NAME} v${PROJECT_VERSION} - Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server")

# 消费者程序
add_executable(consumer
    src/consumer.cpp
    src/OssClient.cpp
)

target_link_libraries(consumer PRIVATE
    ${Protobuf_LIBRARIES}  # Protobuf v3.21.12
    srpc                   # SRPC v0.10.3
    workflow               # Sogou Workflow
    crypto
    ssl
    SimpleAmqpClient
    rabbitmq
    alibabacloud-oss-cpp-sdk
    curl
    pthread
)

# ========== User Service RPC 服务器 ==========
set(USER_SERVER_SOURCES
    src/UserServiceImpl.cpp
    src/user_server.cpp
    src/crypto_util.cpp
    src/user_service.pb.cc
)

add_executable(user_server ${USER_SERVER_SOURCES})

target_compile_options(user_server PRIVATE -g -Wall -Wextra)

target_link_libraries(user_server PRIVATE
    ${Protobuf_LIBRARIES}
    srpc
    workflow
    crypto
    ssl
    jwt
    pthread
)

# 编译信息
message(STATUS "===== CloudDisk_v4 RPC 微服务架构 =====")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Protobuf: v3.21.12 (无 Abseil 依赖)")
message(STATUS "SRPC: v0.10.3")
message(STATUS "Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "==========================================")
