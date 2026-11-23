#include "platform/XWindowsClipboardImage.h"

#include "base/BitUtilities.h"

#include <QBuffer>
#include <QImage>

#include <limits>

namespace inputleap {

std::string clipboardDibToBmp(const std::string& dib)
{
    if (dib.size() < 40 ||
        dib.size() > std::numeric_limits<std::uint32_t>::max() - 14) {
        return {};
    }
    const auto* raw = reinterpret_cast<const std::uint8_t*>(dib.data());
    const auto headerSize = load_little_endian_u32(raw);
    const auto depth = load_little_endian_u16(raw + 14);
    const auto compression = load_little_endian_u32(raw + 16);
    const auto colorsUsed = load_little_endian_u32(raw + 32);
    if (headerSize < 40 || headerSize > dib.size() ||
        load_little_endian_u16(raw + 12) != 1) {
        return {};
    }

    // Extended DIB headers include their masks; BITMAPINFOHEADER does not.
    std::uint64_t offset = headerSize;
    if (headerSize == 40 && (compression == 3 || compression == 6)) {
        offset += compression == 6 ? 16 : 12;
    }
    if (depth <= 8 && colorsUsed == 0) {
        offset += (std::uint64_t{1} << depth) * 4;
    }
    else {
        offset += std::uint64_t{colorsUsed} * 4;
    }
    if (offset >= dib.size()) {
        return {};
    }
    std::uint8_t header[14];
    auto* dst = header;
    store_little_endian_u8(dst, 'B');
    store_little_endian_u8(dst, 'M');
    store_little_endian_u32(dst, 14 + dib.size());
    store_little_endian_u16(dst, 0);
    store_little_endian_u16(dst, 0);
    store_little_endian_u32(dst, 14 + offset);
    return std::string(reinterpret_cast<const char*>(header), 14) + dib;
}

std::string clipboardDibToPng(const std::string& dib)
{
    const auto bmp = clipboardDibToBmp(dib);
    if (bmp.empty()) {
        return {};
    }
    const auto image = QImage::fromData(
        reinterpret_cast<const uchar*>(bmp.data()), bmp.size(), "BMP");
    if (image.isNull()) {
        return {};
    }
    QByteArray encoded;
    QBuffer buffer(&encoded);
    if (!buffer.open(QIODevice::WriteOnly) || !image.save(&buffer, "PNG")) {
        return {};
    }
    return std::string(encoded.constData(), encoded.size());
}

std::string clipboardImageToDib(const std::string& data, const char* format)
{
    auto image = QImage::fromData(
        reinterpret_cast<const uchar*>(data.data()), data.size(), format);
    if (image.isNull()) {
        return {};
    }
    // Qt's BMP encoder normalizes compressed/extended headers to a Windows DIB.
    image = image.convertToFormat(QImage::Format_RGB32);
    QByteArray encoded;
    QBuffer buffer(&encoded);
    if (!buffer.open(QIODevice::WriteOnly) || !image.save(&buffer, "BMP") ||
        encoded.size() <= 54) {
        return {};
    }
    return std::string(encoded.constData() + 14, encoded.size() - 14);
}

}
