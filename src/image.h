#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "common.h"

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t fileType;     // 파일 타입 ("BM")
    uint32_t fileSize;     // 전체 파일 크기
    uint16_t reserved1;    // 예약된 공간
    uint16_t reserved2;    // 예약된 공간
    uint32_t dataOffset;   // 픽셀 데이터 오프셋
}; // 14바이트

struct BMPInfoHeader {
    uint32_t headerSize;        // 헤더 크기
    int32_t width;              // 이미지 너비
    int32_t height;             // 이미지 높이
    uint16_t planes;            // 컬러 플레인 수
    uint16_t bitsPerPixel;      // 비트 수 (24비트 또는 32비트)
    uint32_t compression;       // 압축 타입 (0은 비압축)
    uint32_t imageSize;         // 이미지 크기
    int32_t xPixelsPerMeter;    // 수평 해상도
    int32_t yPixelsPerMeter;    // 수직 해상도
    uint32_t colorsUsed;        // 색상 수
    uint32_t colorsImportant;   // 중요한 색상 수
}; // 40바이트
#pragma pack(pop)


CLASS_PTR(Image)
class Image {
public:
    static ImageUPtr LoadBmp(const std::string& filepath);
    static ImageUPtr Create(int width, int height, int channelCount = 4);
    ~Image();

    const uint8_t* GetData() const { return m_data; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannelCount() const { return m_channelCount; }
    void SetCheckImage(int gridX, int gridY);

private:
    Image() {};
    bool LoadWithBmp(const std::string& filepath);
    bool Allocate(int width, int height, int channelCount);
    int m_width { 0 };
    int m_height { 0 };
    int m_channelCount { 0 };
    uint8_t* m_data { nullptr };
};

#endif // __IMAGE_H__