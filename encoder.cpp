#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <zlib.h>
#include <stdexcept>
#include <cstring>

// Function to read the entire content of a file into a vector of bytes
std::vector<unsigned char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Could not completely read file " + filename);
    }
    return buffer;
}

// Function to compress a byte vector using zlib
std::string compressData(const std::vector<unsigned char>& data) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, Z_DEFAULT_COMPRESSION) != Z_OK) {
        throw std::runtime_error("deflateInit failed");
    }

    zs.next_in = const_cast<Bytef*>(data.data());
    zs.avail_in = data.size();

    int chunk = 16384; // Process in chunks
    std::vector<unsigned char> outBuffer(chunk);
    std::string compressedString;

    int deflate_result;
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outBuffer.data());
        zs.avail_out = chunk;
        deflate_result = deflate(&zs, Z_FINISH); // Z_FINISH signals the end of the input data

        if (deflate_result == Z_STREAM_ERROR) {
            deflateEnd(&zs);
            throw std::runtime_error("zlib stream error during compression");
        }

        size_t compressedChunkSize = chunk - zs.avail_out;
        compressedString.append(reinterpret_cast<char*>(outBuffer.data()), compressedChunkSize);

    } while (zs.avail_in > 0 || deflate_result != Z_STREAM_END);

    deflateEnd(&zs);
    return compressedString;
}

// Function to decompress a string back to a byte vector (for verification)
std::vector<unsigned char> decompressData(const std::string& compressedString) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK) {
        throw std::runtime_error("inflateInit failed");
    }

    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedString.data()));
    zs.avail_in = compressedString.size();

    int chunk = 16384;
    std::vector<unsigned char> outBuffer(chunk);
    std::vector<unsigned char> decompressedData;

    int inflate_result;
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outBuffer.data());
        zs.avail_out = chunk;
        inflate_result = inflate(&zs, Z_NO_FLUSH);

        if (inflate_result == Z_STREAM_ERROR) {
            inflateEnd(&zs);
            throw std::runtime_error("zlib stream error during decompression");
        }

        size_t decompressedChunkSize = chunk - zs.avail_out;
        decompressedData.insert(decompressedData.end(), outBuffer.begin(), outBuffer.begin() + decompressedChunkSize);

    } while (inflate_result != Z_STREAM_END);

    inflateEnd(&zs);
    return decompressedData;
}

/*
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string inputFilename = argv[1];

    try {
        std::vector<unsigned char> fileData = readFile(inputFilename);
        std::cout << "Original file size: " << fileData.size() << " bytes" << std::endl;

        std::string compressedString = compressData(fileData);
        std::cout << "Compressed string size: " << compressedString.size() << " bytes" << std::endl;
        std::cout << "Compression ratio: " << static_cast<double>(compressedString.size()) / fileData.size() << std::endl;

        // Optional: Decompress and verify
        std::vector<unsigned char> decompressedData = decompressData(compressedString);
        std::cout << "Decompressed data size: " << decompressedData.size() << " bytes" << std::endl;

        if (fileData == decompressedData) {
            std::cout << "Decompression successful! Original and decompressed data match." << std::endl;
        } else {
            std::cerr << "Error: Original and decompressed data do NOT match!" << std::endl;
        }

        // You can now use the 'compressedString' variable
        // (e.g., store it in a database, transmit it over a network, etc.)

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 
*/

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = inputFilename + ".cmpfile";

    try {
        std::vector<unsigned char> fileData = readFile(inputFilename);
        std::cout << "Original file size: " << fileData.size() << " bytes" << std::endl;

        std::string compressedString = compressData(fileData);
        std::cout << "Compressed string size: " << compressedString.size() << " bytes" << std::endl;
        std::cout << "Compression ratio: " << static_cast<double>(compressedString.size()) / fileData.size() << std::endl;

        // Write compressed string to a file
        std::ofstream outFile(outputFilename, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Could not open file for writing: " + outputFilename);
        }
        outFile.write(compressedString.data(), compressedString.size());
        outFile.close();
        std::cout << "Compressed data written to: " << outputFilename << std::endl;

        // Optional: Decompress and verify
        std::vector<unsigned char> decompressedData = decompressData(compressedString);
        std::cout << "Decompressed data size: " << decompressedData.size() << " bytes" << std::endl;

        if (fileData == decompressedData) {
            std::cout << "Decompression successful! Original and decompressed data match." << std::endl;
        } else {
            std::cerr << "Error: Original and decompressed data do NOT match!" << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}