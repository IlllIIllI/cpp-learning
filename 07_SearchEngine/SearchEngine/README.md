# 搜索引擎项目 - 第一期

## 项目结构

```
SearchEngine/
├── include/               # 头文件
│   ├── directory_scanner.h
│   ├── keyword_processor.h
│   └── page_processor.h
├── src/                   # 源文件 (.cc)
│   ├── directory_scanner.cc
│   ├── keyword_processor.cc
│   └── page_processor.cc
├── test/                  # 测试程序
│   ├── test_keyword.cc
│   └── test_page.cc
└── CMakeLists.txt
```

## 代码规范

本项目严格遵循 [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)：

- **文件命名**: 小写字母 + 下划线 (snake_case)，源文件使用 `.cc` 扩展名
- **类命名**: 大驼峰 (UpperCamelCase)，如 `DirectoryScanner`
- **函数命名**: 大驼峰 (UpperCamelCase)，如 `Scan()`, `Process()`
- **变量命名**: 小写字母 + 下划线 (snake_case)，成员变量以下划线结尾，如 `tokenizer_`
- **命名空间**: 所有代码在 `search_engine` 命名空间下
- **头文件保护**: 使用 `#ifndef` 格式，如 `SEARCH_ENGINE_DIRECTORY_SCANNER_H_`
- **缩进**: 2 个空格
- **注释**: 使用 `//` 风格，函数前添加说明注释

## 构建说明

### 1. 构建项目

```bash
cd SearchEngine
mkdir build && cd build
cmake ..
make
```

### 2. 运行测试

#### 测试关键字推荐模块
```bash
./test_keyword
```

生成文件:
- `cn_dict.dat` - 中文词典
- `cn_index.dat` - 中文索引
- `en_dict.dat` - 英文词典
- `en_index.dat` - 英文索引

#### 测试网页处理模块
```bash
./test_page
```

生成文件:
- `pages.dat` - 网页库
- `offsets.dat` - 网页偏移库
- `inverted_index.dat` - 倒排索引库

## 实现要点

### DirectoryScanner
- 使用 `opendir()`, `readdir()`, `closedir()` 遍历目录
- 过滤掉 `.` 和 `..`
- 返回完整文件路径列表

### KeyWordProcessor

#### 中文处理
1. 使用 cppjieba 分词
2. 过滤停用词
3. 统计词频
4. 使用 utfcpp 将词语拆分成单个汉字构建索引

#### 英文处理
1. 去除数字和标点,只保留字母
2. 转小写
3. 按空白字符分割
4. 过滤停用词
5. 统计词频
6. 按字母构建索引

### PageProcessor

#### XML 提取
- 使用 tinyxml2 解析 XML
- 提取 `<item>` 标签中的内容
- 优先级: `<content>` > `<description>`

#### 文档去重
- 使用 simhash 计算文档哈希值
- 汉明距离 ≤ 3 视为重复

#### 倒排索引
1. 分词并过滤停用词
2. 计算 TF (词频)
3. 计算 DF (文档频率)
4. 计算 TF-IDF: weight = TF × log₂(N/DF)
5. 归一化: weight' = weight / √(sum of squares)

## 注意事项

1. **cppjieba 初始化**: 只创建一次 Jieba 对象,避免重复初始化
2. **UTF-8 处理**: 使用 utfcpp 处理中文字符
3. **权重归一化**: 倒排索引必须存储归一化后的权重
4. **停用词**: 使用 `std::set` 存储以提高查找效率
5. **命名空间**: 记得使用 `search_engine::` 命名空间

## 代码框架说明

所有源文件都包含了详细的 TODO 注释和实现提示，你可以按照提示实现各个功能模块。代码已按照 Google C++ Style Guide 组织。
