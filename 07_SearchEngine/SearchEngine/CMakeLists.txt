cmake_minimum_required(VERSION 3.10)
project(SearchEngine CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

# 包含头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    /usr/local/include
)

# 查找 tinyxml2 库
find_library(TINYXML2_LIBRARY tinyxml2 PATHS /usr/local/lib64 /usr/local/lib)
if(NOT TINYXML2_LIBRARY)
    message(FATAL_ERROR "tinyxml2 library not found")
endif()

# 创建关键字推荐测试程序
add_executable(test_keyword
    test/test_keyword.cc
    src/directory_scanner.cc
    src/keyword_processor.cc
)
target_link_libraries(test_keyword ${TINYXML2_LIBRARY} pthread)

# 创建网页处理测试程序
add_executable(test_page
    test/test_page.cc
    src/directory_scanner.cc
    src/page_processor.cc
)
target_link_libraries(test_page ${TINYXML2_LIBRARY} pthread)
