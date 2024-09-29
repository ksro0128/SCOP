#include "image.h"

ImageUPtr Image::LoadBmp(const std::string& filepath) {
    auto image = ImageUPtr(new Image());
    if (!image->LoadWithBmp(filepath))
        return nullptr;
    return std::move(image);
}

ImageUPtr Image::Create(int width, int height, int channelCount) {
    auto image = ImageUPtr(new Image());
    if (!image->Allocate(width, height, channelCount))
        return nullptr;
    return std::move(image);
}

ImageUPtr Image::CreateSingleColorImage(
    int width, int height, const glm::vec4& color) {
    glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f);
    uint8_t rgba[4] = {
        (uint8_t)clamped.r, 
        (uint8_t)clamped.g, 
        (uint8_t)clamped.b, 
        (uint8_t)clamped.a, 
    };
    auto image = Create(width, height, 4);
    for (int i = 0; i < width * height; i++) {
        memcpy(image->m_data + 4 * i, rgba, 4);
    }
    return std::move(image);
}

bool Image::Allocate(int width, int height, int channelCount) {
    m_width = width;
    m_height = height;
    m_channelCount = channelCount;
    m_data = (uint8_t*)malloc(m_width * m_height * m_channelCount);
    return m_data ? true : false;
}

Image::~Image() {
    if (m_data) {
        free(m_data);
    }
}

bool Image::LoadWithBmp(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open BMP file: " << filepath << std::endl;
        return false;
    }

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    // Read file header
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.fileType != 0x4D42) { // Check for "BM"
        std::cerr << "Invalid BMP file: " << filepath << std::endl;
        return false;
    }

    // Read info header
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    if (infoHeader.bitsPerPixel != 24 && infoHeader.bitsPerPixel != 32) {
        std::cerr << "Unsupported BMP bit depth: " << infoHeader.bitsPerPixel << std::endl;
        return false;
    }

    // Set image dimensions and channel count
    m_width = infoHeader.width;
    m_height = abs(infoHeader.height); // BMP height can be negative
    m_channelCount = infoHeader.bitsPerPixel / 8;

    // Allocate memory for image data
    m_data = (uint8_t*)malloc(m_width * m_height * m_channelCount);
    if (!m_data) {
        std::cerr << "Failed to allocate memory for BMP image" << std::endl;
        return false;
    }

    // Move file cursor to pixel data
    file.seekg(fileHeader.dataOffset, file.beg);

    // Read pixel data row by row (BMP data is bottom-up by default)
    int rowSize = (m_width * m_channelCount + 3) & ~3; // Align row size to 4-byte boundary
    std::unique_ptr<uint8_t[]> rowData(new uint8_t[rowSize]);

    for (int y = 0; y < m_height; ++y) {
        file.read(reinterpret_cast<char*>(rowData.get()), rowSize);
        
        // Copy data from rowData to the original position (no vertical flipping)
        memcpy(m_data + y * m_width * m_channelCount, rowData.get(), m_width * m_channelCount);
    }

    // Optional: Convert BGR to RGB if necessary
    if (m_channelCount >= 3) {
        for (int i = 0; i < m_width * m_height; ++i) {
            uint8_t* pixel = m_data + i * m_channelCount;
            std::swap(pixel[0], pixel[2]); // Swap R and B
        }
    }

    return true;
}


void Image::SetCheckImage(int gridX, int gridY) {
    for (int j = 0; j < m_height; j++) {
        for (int i = 0; i < m_width; i++) {
            int pos = (j * m_width + i) * m_channelCount;
            bool even = ((i / gridX) + (j / gridY)) % 2 == 0;
            uint8_t value = even ? 255 : 0;
            for (int k = 0; k < m_channelCount; k++)
                m_data[pos + k] = value;
            if (m_channelCount > 3)
                m_data[3] = 255;
        }
    }
}