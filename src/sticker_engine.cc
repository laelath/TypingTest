// Copyright (C) 2017 Justin Frank, Jason Waataja
//
// This file is part of TypingTest.
//
// TypingTest is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// TypingTest is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// TypingTest.  If not, see <http://www.gnu.org/licenses/>.

#include "sticker_engine.h"

namespace typingtest {

StickerEngine::StickerEngine()
{
}

StickerEngine::~StickerEngine()
{
}

std::string StickerEngine::getStickerPath(const std::string &stickerName)
{
    return STICKER_PREFIX + stickerName;
}

bool StickerEngine::hasSticker(const std::string &stickerName)
{
    std::string path{getStickerPath(stickerName)};
    return Gio::Resource::get_file_exists_global_nothrow(path);
}

Glib::RefPtr<Gdk::Pixbuf> StickerEngine::createPixbufForSticker(
    const std::string &stickerName)
{
    try {
        return Gdk::Pixbuf::create_from_resource(getStickerPath(stickerName));
    } catch (Gio::ResourceError) {
        return Glib::RefPtr<Gdk::Pixbuf>();
    } catch (Gdk::PixbufError) {
        return Glib::RefPtr<Gdk::Pixbuf>();;
    }
    return Glib::RefPtr<Gdk::Pixbuf>();
}

Glib::RefPtr<Gdk::Pixbuf> StickerEngine::createPixbufForSticker(
    const std::string &stickerName, int width, int height)
{
    Glib::RefPtr<Gdk::Pixbuf> pixbuf{createPixbufForSticker(stickerName)};
    return (pixbuf) ? pixbuf->scale_simple(width, height, Gdk::INTERP_HYPER)
        : pixbuf;
}

Glib::RefPtr<Gdk::Pixbuf> StickerEngine::createPixbufDefaultSize(
    const std::string &stickerName)
{
    return createPixbufWithWidth(stickerName, DEFAULT_STICKER_WIDTH);
}

Glib::RefPtr<Gdk::Pixbuf> StickerEngine::createPixbufWithWidth(
    const std::string &stickerName, int width)
{
    Glib::RefPtr<Gdk::Pixbuf> pixbuf{createPixbufForSticker(stickerName)};
    if (!pixbuf)
        return pixbuf;

    int srcWidth{pixbuf->get_width()};
    int srcHeight{pixbuf->get_height()};
    return pixbuf->scale_simple(width, srcHeight * width / srcWidth,
        Gdk::INTERP_HYPER);
}

Glib::RefPtr<Gdk::Pixbuf> StickerEngine::createPixbufWithHeight(
    const std::string &stickerName, int height)
{
    Glib::RefPtr<Gdk::Pixbuf> pixbuf{createPixbufForSticker(stickerName)};
    if (!pixbuf)
        return pixbuf;

    int srcWidth{pixbuf->get_width()};
    int srcHeight{pixbuf->get_height()};
    return pixbuf->scale_simple(srcWidth * height / srcHeight, height,
        Gdk::INTERP_HYPER);
}
} // namespace typingtest
