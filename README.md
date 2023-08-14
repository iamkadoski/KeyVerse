# KeyVerse

KeyVerse is a secure data storage and encryption application that allows you to store sensitive data in encrypted verse files. 
Each verse file contains encrypted data and metadata associated with the stored information.

## Features

- Generate encryption keys for data encryption and decryption.
- Encrypt data using AES-128 CBC mode.
- Decrypt data from verse files.
- Save data to encrypted verse files with metadata.
- Retrieve data from verse files.
- Configuration management through a JSON config file.
- Verse file upload to a cloud storage(AWS S3) using RESTful API

## Getting Started

### Prerequisites

- C++ compiler (supporting C++11 or later)
- OpenSSL library
- Boost C++ Libraries (asio)
- CMake (for building)

## Building and Running the Server

To build and run the KeyVerse Server, follow these steps:

1. Clone the repository to your local machine:

   ```sh
   git clone https://github.com/yourusername/KeyVerseServer.git

    ```

  ```sh
  cd KeyVerseServer

 ```
## Usage

The KeyVerse Server listens for incoming connections on a specified port (default is 4545). Clients can connect to the server to perform various actions on the key-value      store.

The server supports the following actions:

  SAVE: Save a key-value pair.
  RETRIEVE: Retrieve a value using a key.
  BACKUP: Backup the key-value store to a file.
  LIST_ALL: List all key-value pairs.

## API
  The server's API follows a straightforward text-based protocol. Clients can send commands in the format:

```sh
  <action>|<key>[|<value>]

  <action>: The action to perform (SAVE, RETRIEVE, BACKUP, LIST_ALL).
  <key>: The key associated with the data.
  <value> (optional): The value to store (used for the SAVE action).

 ```
## Configuration
  The server can be easily configured by editing the serverconfig.json file. This file allows you to specify the server's listening address and port.

  ```sh
      {
        "serverAddress": "0.0.0.0",
        "serverPort": "4545"
      }
 ```
## Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, please submit an issue or create a pull request.

## License

[MIT License](LICENSE)


