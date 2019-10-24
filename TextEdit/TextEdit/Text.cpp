// Implementation of text
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Text.h"

static const int MIN_EXTENT = 16;
static const int MAX_EXTENT = 1024;
static const int EXT_ALIGNMENT = 16;

TextLine::TextLine():
    L2ListHeader(),
    capacity(0),
    len(0),
    str(0)
{
}

TextLine::TextLine(const TextLine& line):
    L2ListHeader(),
    capacity(line.capacity),
    len(line.len),
    str(0)
{
    if (capacity > 0) {
        str = new char[capacity];
        memmove(str, line.str, len+1);
    }
}


TextLine::TextLine(const char* line):
    L2ListHeader(),
    capacity(0),
    len(0),
    str(0)
{
    setString(line);
}

TextLine::~TextLine() {
    delete[] str;
    // printf("Destructor ~TextLine: this = %p, str=%p\n", this, str);
}

void TextLine::setString(const char* line, int l) {
    if (line == 0 || l == 0) {
        delete[] str; str = 0;
        len = 0; capacity = 0;
        return;
    }

    if (l < 0)
        l = 0;

    if (capacity <= l || str == 0) {
        // Allocate a new buffer for a string
        capacity = l+1;
        delete[] str;
        str = new char[capacity];
    }
    memmove(str, line, l);
    len = l;
    str[len] = 0;
}

void TextLine::setString(const char* line) {
    if (line == 0) {
        delete[] str; str = 0;
        len = 0; capacity = 0;
    } else {
        setString(line, strlen(line));
    }
}

static const char emptyLine[2] = "";

const char* TextLine::getString() const {
    if (str != 0)
        return str;
    else
        return emptyLine;
}

TextLine::operator const char*() const {
    return getString();
}

TextLine& TextLine::operator=(const char* line) {
    setString(line, strlen(line));
    return *this;
}

TextLine& TextLine::operator=(const TextLine& line) {
    setString(line.str, line.len);
    return *this;
}

void TextLine::append(const char* line) {
    int l;
    if (line == 0 || (l = strlen(line)) == 0)
        return;
    ensureCapacity(len + l + 1);
    strcpy(str + len, line); // Append a line
    len += l;
}

void TextLine::append(int c) {
    ensureCapacity(len + 2);
    str[len] = (char) c; str[len+1] = 0; // Append a character
    ++len;
}

void TextLine::ensureCapacity(int n) {
    if (n > capacity) {
        int optimalExtent = n / 8;
        if (optimalExtent < MIN_EXTENT) {
            optimalExtent = MIN_EXTENT;
        } else if (optimalExtent > MAX_EXTENT) {
            optimalExtent = MAX_EXTENT;
        }
        capacity += optimalExtent;
        if (capacity < n)
            capacity = n;

        // Make capacity to be a multiple of EXT_ALIGNMENT
        if ((capacity % EXT_ALIGNMENT) != 0)
            capacity += (EXT_ALIGNMENT - capacity % EXT_ALIGNMENT);

        char* tmp = new char[capacity];
        if (str != 0) {
            memmove(tmp, str, len+1);
            delete[] str;
        }
        str = tmp;
    }
}

TextLine& TextLine::operator+=(const char* line) {
    append(line);
    return *this;
}

TextLine& TextLine::operator+=(int character) {
    append(character);
    return *this;
}

TextLine TextLine::operator+(const char* line) const {
    TextLine t(*this);
    t.append(line);
    return t;
}

TextLine TextLine::operator+(int character) const {
    TextLine t(*this);
    t.append(character);
    return t;
}

void TextLine::insert(int pos, const char* line) {
    int l;
    if (line == 0 || (l = strlen(line)) == 0)
        return;
    ensureCapacity(len + l + 1);
    if (pos < 0)
        pos = 0;
    if (pos > len)
        pos = len;
    if (pos < len)
        memmove(str + pos + l, str + pos, len - pos);
    memmove(str + pos, line, l);
    len += l;
    str[len] = 0;
}

void TextLine::insert(int pos, int c) {
    ensureCapacity(len + 2);
    if (pos < 0)
        pos = 0;
    if (pos > len)
        pos = len;
    if (pos < len)
        memmove(str + pos + 1, str + pos, len - pos);
    str[pos] = (char) c;
    ++len;
    str[len] = 0;
}

void TextLine::removeAt(int pos) {
    if (pos >= len || pos < 0)
        return;
    if (pos < len-1)
        memmove(str + pos, str + pos + 1, (len-1) - pos);
    --len;
    str[len] = 0;
    truncate(len);
}

void TextLine::setSize(int n) {
    if (n < 0)
        n = 0;
    ensureCapacity(n+1);
    len = n;
    str[len] = 0;
}

void TextLine::truncate(int pos) {
    if (pos < len) {
        str[pos] = 0;
        len = pos;
    }
    if (capacity > len + 1 + EXT_ALIGNMENT) {
        // Release unused space
        int newCapacity = len + 1;
        if ((newCapacity % EXT_ALIGNMENT) != 0)
            newCapacity += (EXT_ALIGNMENT - newCapacity % EXT_ALIGNMENT);
        char* tmp = new char[newCapacity];
        memmove(tmp, str, len + 1);
        capacity = newCapacity;
        delete[] str;
        str = tmp;
    }
}

void TextLine::trim() {    // Remove white space at the end of line
    if (len == 0)
        return;
    int pos = len;
    while (pos > 0 && isspace(str[pos-1]))
        --pos;
    if (pos < len)
        truncate(pos);
}

bool Text::load(const char *filePath) {
    removeAll();
    FILE* f = fopen(filePath, "r");
    if (f == 0)
        return false;

    char buffer[512];
    int buffLen = 0;
    TextLine line;
    line.ensureCapacity(1024);
    line.truncate(0);
    int pos = 0;
    int prevChar = 0;

    buffLen = fread(buffer, 1, 512, f);
    while (buffLen > 0) {
        for (int i = 0; i < buffLen; i++) {
            int c = buffer[i];
            if (c == '\n') {
                line.trim();
                addBefore(new TextLine(line.getString()));
                line.setSize(0);
                pos = 0;
                prevChar = 0;
            } else {
                if (prevChar == '\r') {
                    line.append(prevChar);
                    pos++;
                }
                if (c == '\t') {
                    // Convert a tabulation into spaces
                    int spacesToAdd = tabWidth - (pos % tabWidth);
                    while (spacesToAdd > 0) {
                        line.append(' ');
                        pos++;
                        spacesToAdd--;
                    }
                } else if (c != '\r') {
                    line.append(c);
                    pos++;
                }
            }
            prevChar = c;
        }
        buffLen = fread(buffer, 1, 512, f);
    }

    if (line.size() > 0) {
        line.trim();
        addBefore(new TextLine(line.getString()));
    }

    fclose(f);
    return true;
}

bool Text::save(const char *filePath) const {
    int ret = true;
    FILE* f = fopen(filePath, "w");
    if (f == 0)
        return false;
    const_iterator i = begin();
    const_iterator e = end();
    for (int k = 0; k < size() && i != e; ++k, ++i) {
        const TextLine& line = (const TextLine&) *i;
        int l = line.size();
        if (l > 0) {
            // Write a line
            if (fwrite(line.getString(), 1, l, f) <= 0) {
                ret = false;    // Write error
                break;
            }
        }
        // Write the "end of line" character
        if (fputc('\n', f) < 0) {
            ret = false;    // Write error
            break;
        }
    }
    fclose(f);
    return ret;
}

TextLine& Text::getLine(int n) {
    setPointer(n);
    return (TextLine&) elementAfter();
}

const char* Text::getString(int n) const {
    if (n < 0 || n >= size())
        return 0;
    int pos = getPointerPosition();
    const_iterator i;
    if (n < pos) {
        i = endBefore();
        while (pos != n) {
             --i; --pos;
        }
    } else {
        i = beginAfter();
        while (pos != n) {
             ++i; ++pos;
        }
    }
    return ((const TextLine&) *i).getString();
}
