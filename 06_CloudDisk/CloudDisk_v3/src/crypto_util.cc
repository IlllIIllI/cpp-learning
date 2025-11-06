#include "crypto_util.h"
#include "user.h"

#include "jwt.h"
#include <stdlib.h>
#include <string.h>

using namespace std;

static const char* SECRET_KEY = "$^Hk16NV"; // 内部链接：其它编译单元看不到

string CryptoUtil::generate_salt(int length)
{
    const char* alpha = "0123456789"
                  "abcdefghijklmnopqrstuvwxyz"
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    string result;
    for (int i = 0; i < length; ++i) {
        result += alpha[rand() % 62];
    }
    return result;
}

string CryptoUtil::hash_password(const string& password, const string& salt, const EVP_MD* md)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();		// 创建 EVP 上下文
    unsigned char hash[EVP_MAX_MD_SIZE];	// EVP_MAX_MD_SIZE: 最大哈希长度
    unsigned int hash_len;					// 用来接收实际的哈希长度

    EVP_DigestInit_ex(ctx, md, NULL);		// 初始化上下文，采用 sha256 哈希算法
    EVP_DigestUpdate(ctx, password.c_str(), password.size());	// 更新上下文
    EVP_DigestUpdate(ctx, salt.c_str(), salt.size());
    EVP_DigestFinal_ex(ctx, hash, &hash_len);		    // 计算哈希值

    char result[2 * EVP_MAX_MD_SIZE + 1] = { '\0' };
    for (unsigned int i = 0; i < hash_len; i++) {			    // 转换成十六进制字符
        sprintf(result + 2 * i, "%02x", hash[i]);
    }

    EVP_MD_CTX_free(ctx);							// 释放上下文
    return result;
}

string CryptoUtil::generate_token(const User& user, jwt_alg_t algorithm)
{
    jwt_t* jwt;
    jwt_new(&jwt);  // 创建 JWT

    jwt_set_alg(jwt, algorithm, (unsigned char*)SECRET_KEY, strlen(SECRET_KEY));

    // 设置载荷(Payload): 用户自定义数据(不能存放敏感数据，比如：password, salt)
    jwt_add_grant(jwt, "sub", "login");
    jwt_add_grant_int(jwt, "id", user.id_);      // 用户id
    jwt_add_grant(jwt, "username", user.username_.c_str());	// 用户名字
    jwt_add_grant(jwt, "createdAt", user.createdAt_.c_str());   // 创建时间
    jwt_add_grant_int(jwt, "expire", time(NULL) + 1800);   // 过期时间 (30min)
    
    char* token = jwt_encode_str(jwt);		// token长度是不确定的，100-300字节
    string result = token;
    // 释放资源
    jwt_free(jwt);
    free(token);

    return result;
}

bool CryptoUtil::verify_token(const std::string& token, User& user)
{
    jwt_t* jwt;
    int err = jwt_decode(&jwt, token.c_str(), (unsigned char*)SECRET_KEY, strlen(SECRET_KEY));
    if (err) {
        return false;
    }

    // 验证主题
    if (strcmp("login", jwt_get_grant(jwt, "sub")) != 0) {
        jwt_free(jwt);
        return false;
    }
    // 判断是否过期
    if (jwt_get_grant_int(jwt, "expire") < time(NULL)) {
        jwt_free(jwt);
        return false;
    }
    // 解析 token
    user.id_ = jwt_get_grant_int(jwt, "id");
    user.username_ = jwt_get_grant(jwt, "username");
    user.createdAt_ = jwt_get_grant(jwt, "createdAt");

    jwt_free(jwt);
    return true;
}

std::string CryptoUtil::hash_data(const std::string& data, const EVP_MD* md)
{
    // 判断数据是否正常
    if (data.c_str() == nullptr || data.size() == 0) {
        return "";
    }
    // 创建 EVP 上下文
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    // 创建存储哈希值的字符数组
    unsigned char hash[EVP_MAX_MD_SIZE];
    // 实际哈希值长度
    unsigned int hash_len;
    // 初始化 EVP
    EVP_DigestInit_ex(ctx, md, NULL);
    // 将数据传递给 EVP 这个函数可以重复调用追加 
    EVP_DigestUpdate(ctx, data.c_str(), data.size());
    // 完成计算输出最终哈希值
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    // 将二进制的哈希值转为16进制表示的字符串
    char result[2 * EVP_MAX_MD_SIZE + 1] = { '\0' };
    for (unsigned int i = 0; i < hash_len; ++i) {
        sprintf(result + 2 * i, "%02x", hash[i]);
    }

    // 释放 EVP
    EVP_MD_CTX_free(ctx);
    return result;
}