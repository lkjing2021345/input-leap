/*
 * InputLeap -- mouse and keyboard sharing utility
 * Copyright (C) 2012-2016 Symless Ltd.
 * Copyright (C) 2004 Chris Schoeneman
 *
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "base/BitUtilities.h"
#include "platform/XWindowsClipboardBMPConverter.h"
#include "platform/XWindowsClipboardImage.h"

namespace inputleap {

// BMP file header structure
struct CBMPHeader {
public:
    std::uint16_t type;
    std::uint32_t size;
    std::uint16_t reserved1;
    std::uint16_t reserved2;
    std::uint32_t offset;
};

XWindowsClipboardBMPConverter::XWindowsClipboardBMPConverter(
                Display* display) :
    m_atom(XInternAtom(display, "image/bmp", False))
{
    // do nothing
}

XWindowsClipboardBMPConverter::~XWindowsClipboardBMPConverter()
{
    // do nothing
}

IClipboard::EFormat
XWindowsClipboardBMPConverter::getFormat() const
{
    return IClipboard::kBitmap;
}

Atom
XWindowsClipboardBMPConverter::getAtom() const
{
    return m_atom;
}

int
XWindowsClipboardBMPConverter::getDataSize() const
{
    return 8;
}

std::string XWindowsClipboardBMPConverter::fromIClipboard(const std::string& bmp) const
{
    return clipboardDibToBmp(bmp);
}

std::string XWindowsClipboardBMPConverter::toIClipboard(const std::string& bmp) const
{
    return clipboardImageToDib(bmp, "BMP");
}

} // namespace inputleap
