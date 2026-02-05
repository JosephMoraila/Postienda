#pragma once
#include <string>
#include <vector>

namespace HASH{
    inline constexpr int SALT_SIZE = 16; /// 128 bits
    inline constexpr int HASH_SIZE = 32; /// SHA-256
    inline constexpr int ITERATIONS = 100000; 

    /**
     * Contains salt and hash to store
     */
    struct PasswordHash {
        std::vector<unsigned char> salt; ///Random data
        std::vector<unsigned char> hash; ///Hashed password
    };


    /**
     * @brief Tranforms a string into hexadecimal
     * @param data: String to convert
     * @return The string converted to hex
     */
    std::string toHex(const std::vector<unsigned char>& data);

    /**
     * @brief Hashes a string
     * @param password: String to hash
     * @return PassWordHash object with corresponding salt and hash
     */
    PasswordHash hashPassword(const std::string& password);

    /**
     * @brief Tranforms a hex into legible string
     * @param data: Hex to convert
     * @return The hex converted to string
     */
    std::vector<unsigned char> fromHex(const std::string& hex);

    /**
     * @brief verifies if password is correct
     * @param password to hash and compare
     * @param stored: Hash and salt from file to compare with password parameter
     * @return True if are equal, false otherwise
     */
    bool verifyPassword(const std::string& password, const PasswordHash& stored);
}
