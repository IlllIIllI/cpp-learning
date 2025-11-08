#pragma once  // 避免头文件重复包含

#include <string>
#include <openssl/evp.h>
#include <jwt.h>

#include "user.h"

class CryptoUtil
{
public:
    static std::string generate_salt(int length = 8);
    static std::string hash_password(const std::string& password, const std::string& salt, const EVP_MD* md = EVP_sha256());
    static std::string generate_token(const User& user, jwt_alg_t algorithm = JWT_ALG_HS256);
    static bool verify_token(const std::string& token, User& user);
    static std::string hash_data(const std::string& data, const EVP_MD* md = EVP_sha256());
private:
    CryptoUtil() = delete;
};