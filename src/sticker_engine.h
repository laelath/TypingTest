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

#ifndef TYPING_TEST_STICKER_ENGINE_H
#define TYPING_TEST_STICKER_ENGINE_H

#include <string>

#include <gtkmm.h>

namespace typingtest {

// The default path to find stickers with in the resources.
const char STICKER_PREFIX[] = "/us/laelath/typingtest/stickers/";
// The default width for stickers to be used in text.
const int DEFAULT_STICKER_WIDTH = 128;

// An object that can create stickers. If you're wondering why this is a class
// and not a set of functions, you would be thinking correctly but it should be
// that way. However, I thought that this might involve some RAII so I made it
// into a class and was too far in to change it back. Should be pretty easy if
// someone else wants to do it, though.
class StickerEngine {
public:
    StickerEngine();
    ~StickerEngine();

    std::string getStickerPath(const std::string &stickerName);

    // Returns true if there is a sticker with the given name available.
    bool hasSticker(const std::string &stickerName);

    // Creates a pixbuf for the given sticker name.
    //
    // Returns a smart pointer to the pixbuf if it could be created, a null
    // smart pointer otherwise.
    Glib::RefPtr<Gdk::Pixbuf> createPixbufForSticker(
        const std::string &stickerName);
    Glib::RefPtr<Gdk::Pixbuf> createPixbufForSticker(
        const std::string &stickerName, int width, int height);
    // Like createPixbufForSticker except the sticker has the default
    // dimensions.
    Glib::RefPtr<Gdk::Pixbuf> createPixbufDefaultSize(
        const std::string &stickerName);
    // Like createPixbufForSticker except the sticker is scaled to the given
    // width.
    Glib::RefPtr<Gdk::Pixbuf> createPixbufWithWidth(
        const std::string &stickerName, int width);
    // Like createPixbufForSticker except the sticker is scaled to the given
    // height.
    Glib::RefPtr<Gdk::Pixbuf> createPixbufWithHeight(
        const std::string &stickerName, int height);
};
} // namespace typingtest

#endif // TYPING_TEST_STICKER_ENGINE_H
