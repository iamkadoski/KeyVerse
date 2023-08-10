# KeyVerse Server

KeyVerse Server is a C++ application designed to provide secure storage and retrieval of key-value data. This server-side application utilizes the AES-128 CBC mode encryption for data security and offers various functionalities for managing and interacting with the stored data.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Endpoints](#endpoints)
- [Contributing](#contributing)
- [License](#license)

## Introduction

KeyVerse Server is a backend application developed in C++, aimed at securely storing and managing key-value data. The application encrypts the data using the AES-128 CBC encryption mode to ensure data security. It allows you to store, retrieve, and list key-value pairs, making it useful for various applications that require secure data storage and retrieval.

## Features

- Secure storage of key-value data using AES-128 CBC encryption.
- Support for saving and retrieving individual records.
- Ability to list all stored records.
- Integration with cloud storage for backup and restore.

## Installation

To install and run KeyVerse Server, follow these steps:

1. Clone the repository to your local machine:

   ```bash
   git clone https://github.com/your-username/KeyVerse-Server.git

2. Navigate to the project directory:
   
   ```bash
   cd KeyVerse-Server

3. Build the application using a C++ compiler:
 
   ```bash
   g++ -o keyverse_server main.cpp -lssl -lcrypto -lboost_system -lcurl

4. Run the compiled executable:
    
   ```bash
 ./KeyVerse-Server


## Usage

KeyVerse Server is designed to be a command-line application. It listens on a specified port for incoming client connections and responds to requests for storing, retrieving, and listing key-value pairs. The server can be accessed through HTTP-like requests, and responses are sent back to the client.

For example, to store a key-value pair:
   
 ```bash
SAVE|your_key|your_value

To retrieve the value for a specific key:

 ```bash
RETRIEVE|your_key

To list all stored records:

 ```bash
LIST_ALL

## Contributing
Contributions to KeyVerse Server are welcome! If you find any issues or have suggestions for improvements, feel free to submit a pull request or create an issue in the repository.

## License
KeyVerse Server is open-source software licensed under the MIT License. Feel free to use, modify, and distribute the software according to the terms of the license.


