#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <zlib.h>
#include <stdexcept>
#include <cstring>

// Function to read the entire content of a file into a string
std::string readCompressedFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string buffer(size, '\0');
    if (!file.read(&buffer[0], size)) {
        throw std::runtime_error("Could not completely read file: " + filename);
    }
    return buffer;
}

// Function to decompress a string back to a byte vector
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

// Main function
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <compressed_file>" << std::endl;
        return 1;
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = inputFilename.substr(0, inputFilename.find_last_of('.')) + "";

    try {
        // Read the compressed file
        std::string compressedData = readCompressedFile(inputFilename);
        std::cout << "Compressed file size: " << compressedData.size() << " bytes" << std::endl;

        // Decompress the data
        std::vector<unsigned char> decompressedData = decompressData(compressedData);
        std::cout << "Decompressed data size: " << decompressedData.size() << " bytes" << std::endl;

        // Write the decompressed data to a file
        std::ofstream outFile(outputFilename, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Could not open file for writing: " + outputFilename);
        }
        outFile.write(reinterpret_cast<const char*>(decompressedData.data()), decompressedData.size());
        outFile.close();
        std::cout << "Decompressed data written to: " << outputFilename << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}