cmake_minimum_required(VERSION 3.15)

project(CloudDisk VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
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

# 链接库
find_package(OpenSSL REQUIRED)

target_link_libraries(server PRIVATE
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
    crypto
    ssl
    SimpleAmqpClient
    rabbitmq
    alibabacloud-oss-cpp-sdk
    curl
    pthread
 )
