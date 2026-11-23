#pragma once

#include <string>

namespace inputleap {

std::string clipboardDibToBmp(const std::string& dib);
std::string clipboardDibToPng(const std::string& dib);
std::string clipboardImageToDib(const std::string& image, const char* format);

}
