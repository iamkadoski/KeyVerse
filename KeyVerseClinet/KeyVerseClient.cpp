#include <iostream>
#include <boost/asio.hpp>

namespace asio = boost::asio;

// Function to send a request to the server and receive a response
std::string sendRequest(const std::string& serverAddress, const std::string& serverPort, const std::string& request) {
    asio::io_service ioService;
    asio::ip::tcp::socket socket(ioService);
    asio::ip::tcp::resolver resolver(ioService);
    asio::connect(socket, resolver.resolve({ serverAddress, serverPort }));

    asio::write(socket, asio::buffer(request + "\n"));

    asio::streambuf receiveBuffer;
    asio::read_until(socket, receiveBuffer, '\n');
    std::istream receiveStream(&receiveBuffer);
    std::string receivedData;
    std::getline(receiveStream, receivedData);

    return receivedData;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <action> <key> [value]" << std::endl;
        return 1;
    }

    std::string action = argv[1];
    std::string key = argv[2];
    std::string value;

    if (action == "SAVE") {
        if (argc != 4) {
            std::cout << "Usage: " << argv[0] << " SAVE <key> <value>" << std::endl;
            return 1;
        }
        value = argv[3];
    }
    else if (action == "RETRIEVE") {
        if (argc != 3) {
            std::cout << "Usage: " << argv[0] << " RETRIEVE <key>" << std::endl;
            return 1;
        }
    }
    else if (action == "BACKUP") {
        if (argc != 3) {
            std::cout << "Usage: " << argv[0] << " BACKUP <backup_file_path>" << std::endl;
            return 1;
        }
    }
    else if (action == "LIST_ALL") {
        if (argc != 2) {
            std::cout << "Usage: " << argv[0] << " LIST_ALL" << std::endl;
            return 1;
        }
        // return 1;
    }
    else if (action == "HELP") 
    {
        std::cout << "Available actions: SAVE, RETRIEVE, BACKUP" << std::endl;
    }
    else {
        std::cout << "Invalid action: " << action << std::endl;
        std::cout << "Available actions: SAVE, RETRIEVE, BACKUP" << std::endl;
        return 1;
    }

    std::string serverAddress = "localhost";
    std::string serverPort = "4545"; // Assuming the server is running on port 4545

    // Construct the request to send to the server
    std::string request = action + "|" + key;
    if (action == "SAVE") {
        request += "|" + value;
    }

    // Send the request to the server
    std::string response = sendRequest(serverAddress, serverPort, request);

    std::cout << "Server response: " << response << std::endl;

    return 0;
}