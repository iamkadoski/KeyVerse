#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <memory>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <random>

using json = nlohmann::json;



// Function to read the configuration from the config file
std::map<std::string, std::string> readConfig(const std::string& configFilePath) {
    std::ifstream file(configFilePath);
    if (!file) {
        throw std::runtime_error("Failed to open config file for reading");
    }

    json configJson;
    file >> configJson;
    file.close();

    std::map<std::string, std::string> config;
    for (auto it = configJson.begin(); it != configJson.end(); ++it) {
        config[it.key()] = it.value();
    }

    return config;
}



// Function to generate a random encryption key
std::string generateKey() {
    const int keyLength = 16; // 128 bits
    unsigned char key[keyLength];

    if (RAND_bytes(key, keyLength) != 1) {
        throw std::runtime_error("Failed to generate encryption key");
    }

    return std::string(reinterpret_cast<char*>(key), keyLength);
}

// Function to encrypt data using AES-128 CBC mode
std::string encryptData(const std::string& data, const std::string& key) {
    const int blockSize = 16; // 128 bits
    unsigned char iv[blockSize];
    memset(iv, 0, blockSize);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize AES encryption");
    }

    int ciphertextLen = 0;
    std::string encryptedData(data.size() + blockSize, '\0');

    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(&encryptedData[0]), &ciphertextLen, reinterpret_cast<const unsigned char*>(data.c_str()), data.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to perform AES encryption");
    }

    int finalLen = 0;
    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&encryptedData[ciphertextLen]), &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize AES encryption");
    }

    EVP_CIPHER_CTX_free(ctx);

    encryptedData.resize(ciphertextLen + finalLen);
    return encryptedData;
}

// Function to decrypt data using AES-128 CBC mode
std::string decryptData(const std::string& encryptedData, const std::string& key) {
    const int blockSize = 16; // 128 bits
    unsigned char iv[blockSize];
    memset(iv, 0, blockSize);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize AES decryption");
    }

    int plaintextLen = 0;
    std::string decryptedData(encryptedData.size(), '\0');

    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(&decryptedData[0]), &plaintextLen, reinterpret_cast<const unsigned char*>(encryptedData.c_str()), encryptedData.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to perform AES decryption");
    }

    int finalLen = 0;
    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&decryptedData[plaintextLen]), &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize AES decryption");
    }

    EVP_CIPHER_CTX_free(ctx);

    decryptedData.resize(plaintextLen + finalLen);
    return decryptedData;
}

// Function to save the key-value data to an encrypted verse file
void saveData(const std::string& key, const std::string& verseFilePath, const std::string& dataFilePath, const std::string& keyId, const std::string& value) {
    std::map<std::string, std::string> data;

    // Check if the verse file exists
    std::ifstream verseFile(verseFilePath);
    if (verseFile) {
        std::string encryptedData((std::istreambuf_iterator<char>(verseFile)), std::istreambuf_iterator<char>());
        verseFile.close();

        std::string decryptedData = decryptData(encryptedData, key);
        data = json::parse(decryptedData);
    }

    // Add the new key-value pair
    data[keyId] = value;

    // Encrypt the updated data
    std::string jsonData = json(data).dump();
    std::string encryptedData = encryptData(jsonData, key);

    // Save the encrypted data to the verse file
    std::ofstream file(dataFilePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing");
    }

    file << encryptedData;
    file.close();

    std::cout << "Data saved to " << dataFilePath << std::endl;
}


// Function to generate a GUID
std::string generateGUID() {
    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    // Generate a random hex digit
    auto getRandomHexDigit = [&]() {
        int randomInt = dis(gen);
        std::stringstream ss;
        ss << std::hex << randomInt;
        return ss.str();
    };
    std::string guid;

    // Generate the first segment of the GUID
    for (int i = 0; i < 8; ++i) {
        guid += getRandomHexDigit();
    }
    guid += '-';

    // Generate the second segment of the GUID
    for (int i = 0; i < 4; ++i) {
        guid += getRandomHexDigit();
    }
    guid += '-';

    // Generate the third segment of the GUID
    for (int i = 0; i < 4; ++i) {
        guid += getRandomHexDigit();
    }
    guid += '-';

    // Generate the fourth segment of the GUID
    for (int i = 0; i < 4; ++i) {
        guid += getRandomHexDigit();
    }
    guid += '-';

    // Generate the fifth segment of the GUID
    for (int i = 0; i < 12; ++i) {
        guid += getRandomHexDigit();
    }
    return guid;
}



// Function to retrieve data from a data file
std::map<std::string, std::string> retrieveData(const std::string& key, const std::string& dataFilePath) {
    std::map<std::string, std::string> data;

    std::ifstream file(dataFilePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open data file for reading");
    }

    std::string encryptedData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::string decryptedData = decryptData(encryptedData, key);
    json jsonData = json::parse(decryptedData);

    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        data[it.key()] = it.value();
    }

    return data;
}

// Function to retrieve data from a data file
std::map<std::string, std::string> retrieveDataFromFile(const std::string& key, const std::string& dataFilePath) {
    std::map<std::string, std::string> data;

    std::ifstream file(dataFilePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open data file for reading");
    }

    std::string encryptedData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::string decryptedData = decryptData(encryptedData, key);
    json jsonData = json::parse(decryptedData);

    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        data[it.key()] = it.value();
    }

    return data;
}


int main() {
    try {
        std::map<std::string, std::string> config = readConfig("config.json");

        std::string verseFolderPath = config["verseFolderPath"];
        std::string encryptionKey = config["encryptionKey"];

        // Prompt user to enter the key ID and value
        std::cout << "Enter key ID: ";
        std::string keyId;
        std::cin >> keyId;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the newline character

        std::cout << "Enter value: ";
        std::string value;
        std::getline(std::cin, value);

        // Create the verse folder if it doesn't exist
        std::filesystem::create_directory(verseFolderPath);

        // Generate a unique filename for the verse and data files
        std::string guid = generateGUID();
        std::string verseFilePath = verseFolderPath + "/" + keyId + "_" + guid + ".vs";
        std::string dataFilePath = verseFolderPath + "/" + keyId + "_" + guid + ".dat";

        // Save the data to the verse file
        saveData(encryptionKey, verseFilePath, dataFilePath, keyId, value);

        // Retrieve the data from the data file
        std::map<std::string, std::string> retrievedData = retrieveData(encryptionKey, dataFilePath);

        // Print the retrieved data
        std::cout << "Retrieved data from " << dataFilePath << ":" << std::endl;
        for (const auto& pair : retrievedData) {
            std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Exception occurred: " << ex.what() << std::endl;
        // Handle the exception gracefully
    }

    return 0;
}

