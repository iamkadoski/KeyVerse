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
#include <stdexcept>
#include <random>
#include <curl/curl.h>
#include "log.h"
#include <boost/asio.hpp>



using json = nlohmann::json;
namespace asio = boost::asio;

std::string verseFilePath = "data.vs";
std::string dataFilePath = "data.dat";
std::string encryptionKey;



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

//declaring the config variable
std::map<std::string, std::string> config = readConfig("config.json");

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
std::string encryptData(const std::string& data, const std::string& key)
{
	try
	{
		const int blockSize = 16; // 128 bits
		unsigned char iv[blockSize];
		memset(iv, 0, blockSize);

		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		if (!ctx)
		{
			log("Failed to create EVP_CIPHER_CTX");
			throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
		}

		if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv) != 1) {
			EVP_CIPHER_CTX_free(ctx);

			log("Failed to initialize AES encryption");

			throw std::runtime_error("Failed to initialize AES encryption");
		}

		int ciphertextLen = 0;
		std::string encryptedData(data.size() + blockSize, '\0');

		if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(&encryptedData[0]), &ciphertextLen, reinterpret_cast<const unsigned char*>(data.c_str()), data.size()) != 1) {
			EVP_CIPHER_CTX_free(ctx);

			log("Failed to perform AES encryption");

			throw std::runtime_error("Failed to perform AES encryption");
		}

		int finalLen = 0;
		if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&encryptedData[ciphertextLen]), &finalLen) != 1) {
			EVP_CIPHER_CTX_free(ctx);

			log("Failed to finalize AES encryption");

			throw std::runtime_error("Failed to finalize AES encryption");
		}

		EVP_CIPHER_CTX_free(ctx);

		encryptedData.resize(ciphertextLen + finalLen);
		return encryptedData;
	}
	catch (const std::exception& ex)
	{
		log(ex.what());
	}
}

// Function to decrypt data using AES-128 CBC mode
std::string decryptData(const std::string& encryptedData, const std::string& key)
{
	try
	{
		const int blockSize = 16; // 128 bits
		unsigned char iv[blockSize];
		memset(iv, 0, blockSize);

		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		if (!ctx)
		{
			log("Failed to create EVP_CIPHER_CTX");
			throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
		}

		if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv) != 1) {
			EVP_CIPHER_CTX_free(ctx);

			log("Failed to initialize AES decryption");
			throw std::runtime_error("Failed to initialize AES decryption");
		}

		int plaintextLen = 0;
		std::string decryptedData(encryptedData.size(), '\0');

		if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(&decryptedData[0]), &plaintextLen, reinterpret_cast<const unsigned char*>(encryptedData.c_str()), encryptedData.size()) != 1) {
			EVP_CIPHER_CTX_free(ctx);
			log("Failed to perform AES decryption");
			throw std::runtime_error("Failed to perform AES decryption");
		}

		int finalLen = 0;
		if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&decryptedData[plaintextLen]), &finalLen) != 1) {
			EVP_CIPHER_CTX_free(ctx);
			log("Failed to finalize AES decryption");
			throw std::runtime_error("Failed to finalize AES decryption");

		}

		EVP_CIPHER_CTX_free(ctx);

		decryptedData.resize(plaintextLen + finalLen);

		return decryptedData;
	}
	catch (const std::exception& ex)
	{
		return "";
		log(ex.what());
	}
}

// Function to save the key-value data to an encrypted verse file and a single data file
void saveData(const std::map<std::string, std::string>& keyValues, const std::string& encryptionKey) {
	try
	{
		// Encrypt the data
		std::string jsonData = json(keyValues).dump();
		std::string encryptedData = encryptData(jsonData, encryptionKey);

		// Save the encrypted data to both the verse file and the data file
		std::ofstream verseFileOut(verseFilePath, std::ios::binary);
		if (!verseFileOut) {
			throw std::runtime_error("Failed to open verse file for writing");
		}
		verseFileOut.write(encryptedData.c_str(), encryptedData.size());
		verseFileOut.close();

		std::ofstream dataFile(dataFilePath, std::ios::binary);
		if (!dataFile) {
			throw std::runtime_error("Failed to open data file for writing");
		}
		dataFile.write(encryptedData.c_str(), encryptedData.size());
		dataFile.close();

		//logging outcome to log file
		log("Data saved to " + verseFilePath + " and " + dataFilePath);

		std::cout << "Data saved to " << verseFilePath << " and " << dataFilePath << std::endl;

	}
	catch (const std::exception& ex)
	{
		log(ex.what());
	}
}

// Function to retrieve data from the data file and decrypt it
std::map<std::string, std::string> retrieveData(const std::string& encryptionKey, const std::string& dataFilePath) {
	std::map<std::string, std::string> keyValues;

	std::ifstream file(dataFilePath, std::ios::binary);
	if (!file) {
		throw std::runtime_error("Failed to open data file for reading");
	}

	std::string encryptedData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	std::string decryptedData = decryptData(encryptedData, encryptionKey);
	json jsonData = json::parse(decryptedData);

	for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
		keyValues[it.key()] = it.value();
	}

	return keyValues;
}

// Function to generate a GUID
std::string generateGUID()
{
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
std::map<std::string, std::string> retrieveDataFromFile(const std::string& key, const std::string& dataFilePath)
{

	std::map<std::string, std::string> data;

	try
	{

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
	catch (const std::exception& ex)
	{
		return data;
		log(ex.what());
	}
}

// Function to upload a verse file to a cloud storage using RESTful API
void uploadVerseToCloudStorage(const std::string& cloudStorageURL, const std::string& cloudStorageToken,
	const std::string& bucketName, const std::string& verseFilePath)
{
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("Failed to initialize CURL");
	}

	// Set the URL for uploading the verse file
	std::string uploadURL = cloudStorageURL + "/upload?bucket=" + bucketName;
	curl_easy_setopt(curl, CURLOPT_URL, uploadURL.c_str());

	// Set the Authorization header with the cloud storage token
	std::string authHeader = "Authorization: Bearer " + cloudStorageToken;
	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, authHeader.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	// Set the verse file as the request payload
	std::ifstream file(verseFilePath, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open verse file");
	}
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(fileSize);
	if (!file.read(buffer.data(), fileSize)) {
		file.close();
		throw std::runtime_error("Failed to read verse file");
	}
	file.close();
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.data());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, buffer.size());

	// Perform the upload request
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		curl_easy_cleanup(curl);
		throw std::runtime_error("Failed to upload verse file: " + std::string(curl_easy_strerror(res)));
	}

	// Cleanup
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}

// Function to download a verse file from cloud storage using RESTful API
void downloadVerseFromCloudStorage(const std::string& cloudStorageURL, const std::string& cloudStorageToken,
	const std::string& bucketName, const std::string& verseFilePath,
	const std::string& destinationFilePath)
{
	CURL* curl = curl_easy_init();
	if (!curl) {
		throw std::runtime_error("Failed to initialize CURL");
	}

	// Set the URL for downloading the verse file
	std::string downloadURL = cloudStorageURL + "/download?bucket=" + bucketName + "&path=" + verseFilePath;
	curl_easy_setopt(curl, CURLOPT_URL, downloadURL.c_str());

	// Set the Authorization header with the cloud storage token
	std::string authHeader = "Authorization: Bearer " + cloudStorageToken;
	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, authHeader.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	// Set the callback function to write the response to the destination file
	std::ofstream file(destinationFilePath, std::ios::binary);
	if (!file.is_open()) {
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
		throw std::runtime_error("Failed to open destination file");
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
		std::ofstream& file = *static_cast<std::ofstream*>(userdata);
	file.write(static_cast<char*>(ptr), size * nmemb);
	return size * nmemb;
		});
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

	// Perform the download request
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		file.close();
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
		throw std::runtime_error("Failed to download verse file: " + std::string(curl_easy_strerror(res)));
	}

	// Cleanup
	file.close();
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}


// Function to retrieve and return all records from the shared data store
std::string listRecords(const std::map<std::string, std::string>& keyValues)
{
	std::string result;

	for (const auto& kvp : keyValues) {
		result += kvp.first + ": " + kvp.second + "\n";
	}

	if (result.empty()) {
		result = "No records found.";
	}

	return result;
}



std::string listAllRecords()
{
	try {
		std::string encryptedData;
		std::ifstream file(dataFilePath, std::ios::binary);
		if (!file) {
			throw std::runtime_error("Failed to open data file for reading");
		}

		encryptedData.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();

		std::string decryptedData = decryptData(encryptedData, encryptionKey);
		json jsonData = json::parse(decryptedData);

		std::string result = "Listing all records:\n";
		for (const auto& kv : jsonData.items()) {
			result += kv.key() + " => " + kv.value().get<std::string>() + "\n";
		}
		return result;
	}
	catch (const std::exception& ex) {
		log(ex.what());
		return "An error occurred while listing records.";
	}
}


std::string handleRequest(const std::string& request, std::map<std::string, std::string>& keyValues)
{
	std::string action;

	// Find the first space character in the request

	size_t firstDelimPos = request.find('|');

	if (firstDelimPos != std::string::npos)
	{
		action = request.substr(0, firstDelimPos);

		size_t secondDelimPos = request.find('|', firstDelimPos + 1);


		std::string  key = request.substr(firstDelimPos + 1, secondDelimPos - firstDelimPos - 1);


		std::string  value = request.substr(secondDelimPos + 1);

		if (action == "SAVE")
		{

			keyValues[key] = value;
			saveData(keyValues, encryptionKey);
			std::cout << "Action: " << action << "  Data saved successfully." << std::endl;
			//std::cout << "Action: " << action << std::endl;
			return "Data saved successfully.";
		}
		else if (action == "RETRIEVE")
		{
			std::map<std::string, std::string>::iterator it = keyValues.find(key);

			if (it != keyValues.end()) 
			{
				return it->second;
			}
			else 
			{
				return "Key not found.";
			}
		}
		else if (action == "LIST_ALL") 
		{
			return listAllRecords();
		}
		else if (action == "BACKUP") 
		{
			// Encrypt the data
			std::string jsonData = json(keyValues).dump();
			std::string encryptedData = encryptData(jsonData, encryptionKey);
			return encryptedData;

		}


	}
	return "Invalid request.";
}



int main() {
	try {
		// std::map<std::string, std::string> config = readConfig("config.json");

		std::string verseFolderPath = config["verseFolderPath"];
		encryptionKey = config["encryptionKey"];

		std::string portnum = config["appport"];

		int port = stoi(portnum);

		verseFilePath = verseFolderPath + "/data.vs";
		dataFilePath = verseFolderPath + "/data.dat";

		std::map<std::string, std::string> keyValues; // Define keyValues here

		// Load existing data from files, if any
		try
		{
			keyValues = retrieveData(encryptionKey, dataFilePath);
		}
		catch (const std::exception& ex) {
			// Ignore if data files don't exist or are empty
		}

		// Server setup and accepting client connections
		asio::io_service ioService;
		//asio::ip::tcp::acceptor acceptor(ioService, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 4545));
		asio::ip::tcp::acceptor acceptor(ioService, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));

		std::cout << "KeyVerse Server is running. Listening on port " << port << " ..." << std::endl;

		while (true) {
			asio::ip::tcp::socket socket(ioService);
			acceptor.accept(socket);

			asio::streambuf receiveBuffer;
			asio::read_until(socket, receiveBuffer, '\n');

			std::istream receiveStream(&receiveBuffer);
			std::string request;
			std::getline(receiveStream, request);

			std::string response = handleRequest(request, keyValues); // Pass keyValues to handleRequest

			asio::write(socket, asio::buffer(response + "\n"));
		}

	}
	catch (const std::exception& ex) {
		// std::cout << "Exception occurred: " << ex.what() << std::endl;
		log(ex.what());
		// Handle the exception gracefully
	}

	return 0;
}