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
// TypingTest.  If not, see <http:// www.gnu.org/licenses/>.

#ifndef TYPING_TEST_STICKER_BUFFER_H
#define TYPING_TEST_STICKER_BUFFER_H

#include <string>
#include <utility>
#include <map>
#include <unordered_set>

#include <gtkmm.h>

#include "sticker_engine.h"

namespace typingtest {

// Hasher class to create a hash value specifically for a RefPtr to a text tag.
class TagHasher {
public:
    size_t operator()(Glib::RefPtr<Gtk::TextTag> tag) const;
};

// From the gtkmm documentation. This character is returned from the
// TextIter::get_char() function when it's on a pixbuf.
const gunichar UNKNOWN_CHAR = 0xFFFC;

// A text buffer that is meant to be a dropin replacement for the Gtkmm
// TextBuffer class. Almost none of the default behavior is overridden except
// for the fact that when text is inserted, it replaces strings of the form
// ":sticker-name:" with an appropriate sticker. It also stores tags over that
// sticker with the name of the sticker inserted. The most important thing is
// the new function, getTextWithStickers(), that acts just like get_text,
// except places where stickers have been inserted are replaced by the original
// string they could have been inserted with. The normal get_text function is
// not affected and will return the text without any inditcation that stickers
// were there.
class StickerBuffer : public Gtk::TextBuffer {
public:
    StickerBuffer();

    // Returns the text just as get_text would. However, a string of the form
    // :sticker_name:. Carelessly manipulating and reinserting this text may
    // result in unexpected results, as if there is an added colon then the
    // stickers will not display correctly. Because of the way text insertion
    // works, though, reinserting text retreived with this method will display
    // correctly because as of now there is now way create colons that would
    // mess this up. However, this only works if you insert it again with the
    // addNewlines parameter set to false.
    std::string getTextWithStickers();

    static Glib::RefPtr<StickerBuffer> create();

    // For inserting stickers from outside, not for internal use. Inserts a
    // sticker with the given name at the cursor.
    void insertSticker(const std::string &stickerName);
    // Usable for both internal and external use. Looks for all instances where
    // stickers should be inserted and performs the proper insertions.
    void substituteStickers();

    bool shouldAddNewlines() const;
    void setAddNewlines(bool addNewlines);

    using TextTagSet = std::unordered_set<Glib::RefPtr<Gtk::TextTag>,
          TagHasher>;

private:
    void onInsertText(const Gtk::TextIter &iter, const Glib::ustring &text,
        int bytes);

    TextTagSet stickerTags;

    StickerEngine engine;

    // Whether or not to insert newlines on the signal_insert() signal.
    bool addNewlines = true;
    std::vector<std::pair<int, int>> splitChars(
        const std::vector<gunichar> &elements);
    // Pass by copy because the list is adjusted as we go. Shouldn't incur that
    // much overhead.
    void replaceWords(std::vector<std::pair<int, int>> words);

    // Goes over every character, testing for if it matches a sticker, removes
    // the sticker if it doesn not occur anywhere.
    void cleanStickers();

    // Functions to shorten replaceWords().
    void eraseWord(int position, int length);
    // Inserts necessary newlines and for later inserting a pixbuf and returns
    // the amount that the buffer was shortened. This function may modify
    // stickerPosition if a newline is inserted before it.
    int insertNewlines(int &stickerPos);
    // Modifies words by shortening the values in it by amount. Only shortens
    // words from startIndex.
    void shortenWords(std::vector<std::pair<int, int>> &words, int amount,
        int startIndex);
    // If the tag represented by stickerName is already in stickerTags, then it
    // adds it to that and creates it in the buffer if it is not already in the
    // tag table.
    //
    // Returns the tag which could have been created or found.
    Glib::RefPtr<Gtk::TextTag> insertTag(const std::string &stickerName);
    // Inserts a pixbuf of a sticker with the given name if it can be found at
    // the given position.
    //
    // Returns whether or not a sticker was inserted;
    bool insertPixbuf(const std::string &stickerName, int position);
};
} // namespace typingtest

#endif // TYPING_TEST_STICKER_BUFFER_H
