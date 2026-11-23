#include "platform/XWindowsClipboardImage.h"
#include "base/BitUtilities.h"

#include <QBuffer>
#include <QImage>

#include <cassert>
#include <iostream>

using namespace inputleap;

int main()
{
    QImage original(65, 49, QImage::Format_RGB32);
    original.fill(Qt::magenta);
    original.setPixelColor(0, 0, Qt::cyan);
    original.setPixelColor(64, 48, Qt::green);
    QByteArray png;
    QBuffer buffer(&png);
    assert(buffer.open(QIODevice::WriteOnly));
    assert(original.save(&buffer, "PNG"));
    const auto dib = clipboardImageToDib(
        std::string(png.constData(), png.size()), "PNG");
    assert(dib.size() > 40);
    const auto roundtrip = clipboardDibToPng(dib);
    const auto decoded = QImage::fromData(
        reinterpret_cast<const uchar*>(roundtrip.data()), roundtrip.size(), "PNG");
    assert(decoded.size() == original.size());
    assert(decoded.pixelColor(0, 0) == original.pixelColor(0, 0));
    assert(decoded.pixelColor(64, 48) == original.pixelColor(64, 48));
    assert(decoded.pixelColor(10, 10) == original.pixelColor(10, 10));
    assert(clipboardImageToDib("not an image", "PNG").empty());
    assert(clipboardDibToPng("").empty());
    assert(clipboardDibToBmp(std::string(40, '\0')).empty());

    // Reproduce CF_DIBV5 data: a 124-byte header with the original pixel data.
    std::string extended = dib.substr(0, 40) + std::string(84, '\0') + dib.substr(40);
    auto* dst = reinterpret_cast<std::uint8_t*>(&extended[0]);
    store_little_endian_u32(dst, 124);
    const auto bmp = clipboardDibToBmp(extended);
    assert(!bmp.empty());
    assert(load_little_endian_u32(
        reinterpret_cast<const std::uint8_t*>(bmp.data()) + 10) == 138);
    const auto normalized = clipboardImageToDib(bmp, "BMP");
    assert(!normalized.empty());
    const auto extendedPng = clipboardDibToPng(extended);
    const auto extendedDecoded = QImage::fromData(
        reinterpret_cast<const uchar*>(extendedPng.data()), extendedPng.size(), "PNG");
    assert(extendedDecoded.size() == original.size());
    assert(extendedDecoded.pixelColor(0, 0) == original.pixelColor(0, 0));
    std::cout << "PASS: PNG/DIB roundtrip, pixel orientation, extended BMP header, invalid data\n";
}
