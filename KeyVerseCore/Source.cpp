//#include <iostream>
//#include <vector>
//#include <string>
//#include <ctime>
//#include <fstream>
//#include <sstream>
//#include <iomanip>
//#include <nlohmann/json.hpp>
//#include <openssl/md5.h>
//#include <random>
//#include <openssl/aes.h>
//#include <openssl/rand.h>
//#include <openssl/evp.h>
//#include <openssl/sha.h>
//#include <chrono>
//#include <ctime>
//#include "log.h"
//
//#ifdef _WIN32
//#include <direct.h> // Include the appropriate header for Windows
//#else
//#include <sys/stat.h> // Include the appropriate header for Unix-like systems
//#endif
//
//using json = nlohmann::json;
//
//struct Data {
//	std::string key;
//	std::string value;
//};
//
//struct KeyVerseStore {
//	std::string ID;
//	std::string timestamp;
//	std::string dataowner;
//	std::string tag;
//	Data data;
//	std::string datasource;
//	std::string dataformat;
//	std::string datasize;
//	std::int32_t ttl;
//	std::int32_t etl;
//	std::string checksum;
//};
//
//
//
//std::string generateGUID() {
//	// Create a random number generator
//	std::random_device rd;
//	std::mt19937 gen(rd());
//	std::uniform_int_distribution<> dis(0, 15);
//
//	// Generate a random hex digit
//	auto getRandomHexDigit = [&]() {
//		int randomInt = dis(gen);
//		std::stringstream ss;
//		ss << std::hex << randomInt;
//		return ss.str();
//	};
//	std::string guid;
//
//	// Generate the first segment of the GUID
//	for (int i = 0; i < 8; ++i) {
//		guid += getRandomHexDigit();
//	}
//	guid += '-';
//
//	// Generate the second segment of the GUID
//	for (int i = 0; i < 4; ++i) {
//		guid += getRandomHexDigit();
//	}
//	guid += '-';
//
//	// Generate the third segment of the GUID
//	for (int i = 0; i < 4; ++i) {
//		guid += getRandomHexDigit();
//	}
//	guid += '-';
//
//	// Generate the fourth segment of the GUID
//	for (int i = 0; i < 4; ++i) {
//		guid += getRandomHexDigit();
//	}
//	guid += '-';
//
//	// Generate the fifth segment of the GUID
//	for (int i = 0; i < 12; ++i) {
//		guid += getRandomHexDigit();
//	}
//	return guid;
//}
//
////generate encryption key
//std::string generateAESKey(const std::string& encryptionKey) {
//	std::string key;
//	key.resize(AES_BLOCK_SIZE);
//
//	// Use SHA-256 to generate a fixed-size AES key
//	SHA256(reinterpret_cast<const unsigned char*>(encryptionKey.c_str()), encryptionKey.size(), reinterpret_cast<unsigned char*>(&key[0]));
//
//	return key;
//}
//
//
//
////encrypt data in verse file
//std::string encryptData(const std::string& data, const std::string& encryptionKey) {
//	// Generate the AES key
//	std::string key = generateAESKey(encryptionKey);
//
//	// Initialize the initialization vector (IV)
//	std::vector<unsigned char> iv(AES_BLOCK_SIZE);
//	RAND_bytes(iv.data(), AES_BLOCK_SIZE);
//
//	// Create a new cipher context
//	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
//
//	// Initialize the cipher context for encryption
//	EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv.data());
//
//	// Determine the required output buffer size
//	int maxOutputSize = data.size() + AES_BLOCK_SIZE;
//	std::vector<unsigned char> encryptedData(maxOutputSize);
//
//	// Perform the encryption
//	int encryptedSize = 0;
//	EVP_EncryptUpdate(ctx, encryptedData.data(), &encryptedSize, reinterpret_cast<const unsigned char*>(data.c_str()), data.size());
//
//	// Finalize the encryption
//	int finalEncryptedSize = 0;
//	EVP_EncryptFinal_ex(ctx, encryptedData.data() + encryptedSize, &finalEncryptedSize);
//
//	// Concatenate the IV and the encrypted data
//	std::vector<unsigned char> encryptedResult(iv.begin(), iv.end());
//	encryptedResult.insert(encryptedResult.end(), encryptedData.begin(), encryptedData.begin() + encryptedSize + finalEncryptedSize);
//
//	// Clean up the cipher context
//	EVP_CIPHER_CTX_free(ctx);
//
//	// Return the encrypted data as a string
//	return std::string(reinterpret_cast<const char*>(encryptedResult.data()), encryptedResult.size());
//}
//
//
//std::string generateChecksum(const std::string& data) {
//	EVP_MD_CTX* mdContext = EVP_MD_CTX_new();
//
//	if (mdContext == nullptr) {
//		std::cerr << "Error: Failed to create MD5 context." << std::endl;
//		return "";
//	}
//
//	std::vector<unsigned char> md5Hash(EVP_MD_size(EVP_md5()));
//	unsigned int md5HashLen = 0;
//
//	if (EVP_DigestInit_ex(mdContext, EVP_md5(), nullptr) != 1) {
//		EVP_MD_CTX_free(mdContext);
//		std::cerr << "Error: Failed to initialize MD5 digest." << std::endl;
//		return "";
//	}
//
//	if (EVP_DigestUpdate(mdContext, data.c_str(), data.size()) != 1) {
//		EVP_MD_CTX_free(mdContext);
//		std::cerr << "Error: Failed to update MD5 digest." << std::endl;
//		return "";
//	}
//
//	if (EVP_DigestFinal_ex(mdContext, md5Hash.data(), &md5HashLen) != 1) {
//		EVP_MD_CTX_free(mdContext);
//		std::cerr << "Error: Failed to finalize MD5 digest." << std::endl;
//		return "";
//	}
//
//	EVP_MD_CTX_free(mdContext);
//
//	std::ostringstream oss;
//	oss << std::hex << std::setfill('0');
//	for (const auto& byte : md5Hash) {
//		oss << std::setw(2) << static_cast<unsigned>(byte);
//	}
//
//	return oss.str();
//}
//
//std::string calculateDataSize(const std::string& data) {
//	// Calculate data size in bytes
//	size_t sizeInBytes = data.size();
//
//	// Convert to a human-readable format
//	std::string units[] = { "bytes", "KB", "MB", "GB" };
//	int unitIndex = 0;
//	while (sizeInBytes > 1024 && unitIndex < 3) {
//		sizeInBytes /= 1024;
//		++unitIndex;
//	}
//
//	std::ostringstream oss;
//	oss << sizeInBytes << " " << units[unitIndex];
//
//	return oss.str();
//}
//
//bool createBooksFolder(const std::string& dbName) 
//{
//
//	try
//	{
//		int result;
//
//		std::string mainfolder = ".Data";
//		//std::string folderName = ".Data/Books";
//		std::ifstream MainfolderCheck(mainfolder);
//
//		if (MainfolderCheck) {
//			std::cerr << "Error: The Books folder already exists." << std::endl;
//			return false;
//		}
//
//
//		#ifdef _WIN32
//				 result = _mkdir(mainfolder.c_str()); // Create the Books folder on Windows
//		#else
//				 result = mkdir(mainfolder.c_str(), 0700); // Create the Books folder on Unix-like systems
//		#endif
//
//
//		std::string folderName = ".Data/Books/" + dbName + "";
//
//		// Check if the Books folder already exists
//		std::ifstream folderCheck(folderName);
//
//		if (folderCheck) {
//			std::cerr << "Error: The " + dbName + " folder already exists." << std::endl;
//			return false;
//		}
//
//			#ifdef _WIN32
//					 result = _mkdir(folderName.c_str()); // Create the Books folder on Windows
//			#else
//					 result = mkdir(folderName.c_str(), 0700); // Create the Books folder on Unix-like systems
//			#endif
//
//		if (result == 0) 
//		{
//
//			std::cout << "The Books " + dbName + " folder has been created." << std::endl;
//			return true;
//		}
//		else 
//		{
//			std::cerr << "Error: Failed to create the Books " + dbName + " folder." << std::endl;
//			return false;
//		}
//	}
//	catch (const std::exception& ex)
//	{
//		std::string errorMsg = ex.what();
//
//		// Output the error message
//		log(errorMsg);
//
//		std::cout << "Error: " << errorMsg << std::endl;
//		return false;
//	}
//}
//
////creates and check if collectionname exist
//bool createStoresFolder(const std::string& dbName, const std::string& collectionName) {
//	std::string folderName = ".Data/Books/" + dbName + "/Stores/"+ collectionName  +"";
//
//	// Check if the Books folder exists
//	std::ifstream booksFolderCheck(folderName);
//
//	if (!booksFolderCheck) {
//		std::cerr << "Error: The Books folder does not exist." << std::endl;
//		return false;
//	}
//
//	// Check if the Stores folder already exists
//	std::ifstream folderCheck(folderName);
//	if (folderCheck) {
//		std::cerr << "Error: The Stores folder already exists." << std::endl;
//		return false;
//	}
//
//			#ifdef _WIN32
//				int result = _mkdir(folderName.c_str()); // Create the Stores folder on Windows
//			#else
//				int result = mkdir(folderName.c_str(), 0700); // Create the Stores folder on Unix-like systems
//			#endif
//
//	if (result == 0) {
//		std::cout << "The Stores folder has been created in the Books folder." << std::endl;
//		return true;
//	}
//	else {
//		std::cerr << "Error: Failed to create the Stores folder." << std::endl;
//		return false;
//	}
//}
//
//bool createVerseFile(const std::string& dbName, const std::string& collectionName, const std::string& key, const std::string& value, const std::int32_t& ttl, const std::int32_t& etl)
//{
//
//	std::string folderName = ".Data/Books/" + dbName + "/Stores/" + collectionName;
//	std::string filename = folderName + "/" + key + ".verse";
//
//	// Check if the Books folder exists
//	std::ifstream booksFolderCheck(folderName);
//	if (!booksFolderCheck) {
//		std::cerr << "Error: The Books folder does not exist." << std::endl;
//		return false;
//	}
//
//	// Check if the Stores folder exists
//	std::ifstream storesFolderCheck(folderName);
//	if (!storesFolderCheck) {
//		std::cerr << "Error: The Stores folder does not exist." << std::endl;
//		return false;
//	}
//
//	// Check if the verse file already exists
//	std::ifstream fileCheck(filename);
//	if (fileCheck) {
//		std::cerr << "Error: The verse file already exists." << std::endl;
//		return false;
//	}
//
//	std::string guid = generateGUID();
//
//	//// Get the current system time
//
//	std::time_t currentTime = std::time(nullptr);
//	char timeString[26]; // Buffer to hold the time string
//
//	std::tm timeInfo;
//
//		#if defined(_WIN32)
//			localtime_s(&timeInfo, &currentTime); // Windows-specific version of localtime
//		#else
//			localtime_r(&currentTime, &timeInfo); // POSIX-compatible version of localtime
//		#endif
//
//	if (std::strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%S", &timeInfo) > 0) {
//		std::string formattedTime(timeString);
//		// Process the formattedTime as needed
//		std::cout << "Formatted Time: " << formattedTime << std::endl;
//	}
//	else {
//		std::cerr << "Error: Failed to convert time to string." << std::endl;
//		// Handle the error case
//	}
//
//	std::vector<KeyVerseStore> keyversestore;
//
//	KeyVerseStore data;
//	data.ID = guid;
//	data.timestamp = timeString;
//	data.dataowner = "";
//	data.tag = "";
//	data.data.key = key;
//	data.data.value = value;
//	data.datasource = dbName;
//	data.dataformat = "json";
//	data.datasize = calculateDataSize(value);
//	data.ttl = ttl; //"2018-12-10T13:49:51.141Z";
//	data.etl = etl; // "2018-12-10T13:49:51.141Z";
//	data.checksum = generateChecksum(value);
//
//	keyversestore.push_back(data);
//
//	// Create a JSON object
//	json jsonData;
//
//	// Convert keyversestore to JSON
//	for (const auto& item : keyversestore) {
//		json dataItem;
//		dataItem["ID"] = item.ID;
//		dataItem["timestamp"] = item.timestamp;
//		dataItem["dataowner"] = item.dataowner;
//		dataItem["tag"] = item.tag;
//		dataItem["data"]["key"] = item.data.key;
//		dataItem["data"]["value"] = item.data.value;
//		dataItem["datasource"] = item.datasource;
//		dataItem["dataformat"] = item.dataformat;
//		dataItem["datasize"] = item.datasize;
//		dataItem["ttl"] = item.ttl;
//		dataItem["etl"] = item.etl;
//		dataItem["checksum"] = item.checksum;
//
//		jsonData.push_back(dataItem);
//	}
//
//	// Generate a unique filename based on the current timestamp
//	std::time_t now = std::time(nullptr);
//	char timestamp[100];
//	std::string demacator = "_";
//	//std::strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", std::localtime(&now));
//
//	std::time_t currenttime = std::time(nullptr);
//	std::tm timeInfolocal;
//
//		#if defined(_WIN32)
//			localtime_s(&timeInfolocal, &currenttime); // Windows-specific version of localtime
//		#else
//			localtime_r(&currentTime, &timeInfo); // POSIX-compatible version of localtime
//		#endif
//
//	// char timestamp[15]; // Buffer to hold the formatted time string
//	std::strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", &timeInfolocal);
//
//	
//	std::string uniqueFilename = timestamp + demacator + key + ".verse";
//
//	// Encrypt the JSON data
//
//	std::string encryptionKey = guid + data.dataowner;
//
//	std::string encryptedData = encryptData(jsonData.dump(), encryptionKey);
//
//	// Save the encrypted data to the verse file
//	std::ofstream outputFile(filename, std::ios::binary);
//
//	if (outputFile.is_open()) 
//	{
//		outputFile << encryptedData;
//		outputFile.close();
//		std::cout << "KeyVerse data successfully saved to " << filename << std::endl;
//		return true;
//	}
//	else 
//	{
//		std::cerr << "Error: Failed to save KeyVerse data to " << filename << std::endl;
//		return false;
//	}
//}
////
////
////int main() {
////	// Encryption key for AES-256-CBC encryption
////	//std::string encryptionKey = "encryption_key"; // Replace with your desired encryption key
////
////	// Database (book) name, collection (store) name, and key-value data
////	std::string dbName = "BooksDB";
////	std::string collectionName = "Stores";
////	std::string key = "sampledata";
////	std::string value = "This is the value for the sample data.";
////	std::int32_t timetolive = 303;
////	std::int32_t expiredtime = 9000;
////
////
////	// Create the Books folder (database)
////	if (!createBooksFolder(dbName)) {
////		return 1;
////	}
////
////	// Create the Stores folder (collection)
////	if (!createStoresFolder(dbName, collectionName)) {
////		return 1;
////	}
////
////	// Create verse file (data file)
////	if (!createVerseFile(dbName, collectionName, key, value, timetolive, expiredtime)) {
////		return 1;
////	}
////
////	return 0;
////}
