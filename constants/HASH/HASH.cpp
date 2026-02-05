#include "constants/HASH/HASH.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

#include <cstring>
#include <stdexcept>

namespace HASH {

    std::string toHex(const std::vector<unsigned char>& data) {
        static const char* digits = "0123456789abcdef";
        std::string hex;
        hex.reserve(data.size() * 2);

        for (unsigned char b : data) {
            hex.push_back(digits[b >> 4]);
            hex.push_back(digits[b & 0x0F]);
        }
        return hex;
    }

    PasswordHash hashPassword(const std::string& password) {
        PasswordHash result;
        result.salt.resize(SALT_SIZE);
        result.hash.resize(HASH_SIZE);

        // Salt aleatorio
        if (RAND_bytes(result.salt.data(), SALT_SIZE) != 1) {
            throw std::runtime_error("Failed to generate salt");
        }

        // PBKDF2-HMAC-SHA256
        if (PKCS5_PBKDF2_HMAC(
            password.c_str(),
            static_cast<int>(password.size()),
            result.salt.data(),
            SALT_SIZE,
            ITERATIONS,
            EVP_sha256(),
            HASH_SIZE,
            result.hash.data()) != 1) {
            throw std::runtime_error("Failed to hash password");
        }

        return result;
    }


    std::vector<unsigned char> fromHex(const std::string& hex) {
        if (hex.size() % 2 != 0) {
            throw std::runtime_error("Invalid hex string");
        }

        std::vector<unsigned char> data;
        data.reserve(hex.size() / 2);

        auto hexValue = [](char c) -> unsigned char {
            if ('0' <= c && c <= '9') return c - '0';
            if ('a' <= c && c <= 'f') return c - 'a' + 10;
            if ('A' <= c && c <= 'F') return c - 'A' + 10;
            throw std::runtime_error("Invalid hex character");
            };

        for (size_t i = 0; i < hex.size(); i += 2) {
            unsigned char high = hexValue(hex[i]);
            unsigned char low = hexValue(hex[i + 1]);
            data.push_back((high << 4) | low);
        }

        return data;
    }


    bool verifyPassword(const std::string& password, const PasswordHash& stored) {

    std::vector<unsigned char> computedHash(stored.hash.size());

        PKCS5_PBKDF2_HMAC(
            password.c_str(),
            static_cast<int>(password.size()),
            stored.salt.data(),
            static_cast<int>(stored.salt.size()),
            ITERATIONS,
            EVP_sha256(),
            static_cast<int>(computedHash.size()),
            computedHash.data()
        );

        bool ok = CRYPTO_memcmp(
            stored.hash.data(),
            computedHash.data(),
            stored.hash.size()
        ) == 0;

        OPENSSL_cleanse(computedHash.data(), computedHash.size());
        return ok;
    }



}
