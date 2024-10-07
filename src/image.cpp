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

    // 헤더 14바이트 읽기
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.fileType != 0x4D42) { // "BM" 인지 확인하기
        std::cerr << "Invalid BMP file: " << filepath << std::endl;
        return false;
    }

    //  BMP 정보 헤더 40바이트 읽기
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    if (infoHeader.bitsPerPixel != 24 && infoHeader.bitsPerPixel != 32) { // 24 bits : RGB, 32 bits : RGBA
        std::cerr << "Unsupported BMP bit depth: " << infoHeader.bitsPerPixel << std::endl;
        return false;
    }

    // BMP 파일 정보 저장
    m_width = infoHeader.width;
    m_height = abs(infoHeader.height);
    m_channelCount = infoHeader.bitsPerPixel / 8;

    // 메모리 할당 width * height * channelCount
    m_data = (uint8_t*)malloc(m_width * m_height * m_channelCount);
    memset(m_data, 0, m_width * m_height * m_channelCount);
    if (!m_data) {
        std::cerr << "Failed to allocate memory for BMP image" << std::endl;
        return false;
    }

    // 커서를 픽셀 데이터 위치로 이동
    file.seekg(fileHeader.dataOffset, file.beg);

    // BMP 파일은 행이 4의 배수로 정렬되어 있음
    int rowSize = (m_width * m_channelCount + 3) & ~3; // 4의 배수로 정렬
    std::unique_ptr<uint8_t[]> rowData(new uint8_t[rowSize]);

    for (int y = 0; y < m_height; ++y) {
        file.read(reinterpret_cast<char*>(rowData.get()), rowSize);
        memcpy(m_data + y * m_width * m_channelCount, rowData.get(), m_width * m_channelCount);
    }

    // BGR -> RGB
    if (m_channelCount >= 3) {
        for (int i = 0; i < m_width * m_height; i++) {
            uint8_t* pixel = m_data + i * m_channelCount;
            std::swap(pixel[0], pixel[2]);
        }
    }
    return true;
}
