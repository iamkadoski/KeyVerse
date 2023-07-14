# KeyVerse

KeyVerse is a secure data storage and encryption application that allows you to store sensitive data in encrypted verse files. Each verse file contains encrypted data and metadata associated with the stored information.

## Features

- Generate encryption keys for data encryption and decryption.
- Encrypt data using AES-128 CBC mode.
- Decrypt data from verse files.
- Save data to encrypted verse files with metadata.
- Retrieve data from verse files.
- Configuration management through a JSON config file.

## Getting Started

### Prerequisites

- C++ compiler (supporting C++11 or later)
- OpenSSL library

### Building the Application

1. Clone the KeyVerse repository: git clone https://github.com/your-username/KeyVerse.git 

2. Build the application using your preferred build system or IDE. `./KeyVerse`


### Usage

1. Configure the application settings by editing the `config.json` file. Set the verse folder path and encryption key.

2. Run the KeyVerse application:

3. Follow the prompts to enter the key ID and value for your data.

4. The data will be saved to a verse file in the configured verse folder.

5. To retrieve data, specify the key ID and the corresponding verse file will be decrypted.

## Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, please submit an issue or create a pull request.

## License

[MIT License](LICENSE)


