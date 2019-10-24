#ifndef L2LIST_TEXT_H
#define L2LIST_TEXT_H

#include "L2List.h"

class OutOfRangeException {
public:
    const char* reason;
    OutOfRangeException(): reason("") {}
    OutOfRangeException(const char* cause): reason(cause) {}
};

//
// TextLine is the dynamic array of characters
//
class TextLine: public L2ListHeader {
    int     capacity;
    int     len;        // Not including the terminating zero character
    char*   str;
public:
    TextLine();
    TextLine(const TextLine& line);     // Copy constructor
    TextLine(const char* line);
    virtual ~TextLine();

    // Assignment
    TextLine& operator=(const TextLine& line);
    TextLine& operator=(const char* line);
    const char* getString() const;
    void setString(const char* line);
    void setString(const char* line, int length);

    // Character access
    char operator[](int i) const { return str[i]; }
    char& operator[](int i) { return str[i]; }
    char at(int i) const {
        if (i < 0 || i >= len)
            throw OutOfRangeException("Index out of range");
        return str[i];
    }
    char& at(int i) {
        if (i < 0 || i >= len)
            throw OutOfRangeException("Index out of range");
        return str[i];
    }

    // Covertion to C-style string
    operator const char*() const;

    int size() const { return len; }
    int length() const { return size(); }
    void setSize(int s);

    void truncate(int pos);
    void trim();    // Remove white space at the end of line

    void ensureCapacity(int n);

    void append(const char* line);
    void append(int character);
    TextLine& operator+=(const char* line);
    TextLine& operator+=(int character);
    TextLine operator+(const char* line) const;
    TextLine operator+(int character) const;
    void insert(int position, int character);
    void insert(int position, const char* line);
    void removeAt(int position);
};

class Text: public L2List {
public:
    int tabWidth;       // Size of tabulation

    Text():
        L2List(),
        tabWidth(8)
    {
    }

    // Load/save text in a file
    bool load(const char *filePath);
    bool save(const char *filePath) const;

    // Get a pointer to i-th line, i = 0..size-1
    TextLine& getLine(int i);
    const char* getString(int i) const;

    class iterator: public L2List::iterator {
    public:
        iterator():
            L2List::iterator()
        {
        }

        iterator(const L2List::iterator& i):
            L2List::iterator(i)
        {
        }

        TextLine& operator*() const {
            return (TextLine&) L2List::iterator::operator*();
        }

        TextLine* operator->() const {
            return &(operator*());
        }
    };

    class const_iterator: public iterator {
    public:
        const_iterator():
            iterator()
        {
        }

        const_iterator(const iterator& i):
            iterator(i)
        {
        }

        const TextLine& operator*() const {
            return ((const iterator*) this)->operator*();
        }

        const TextLine* operator->() const {
            return &(operator*());
        }
    };

    iterator begin() {
        return (iterator) L2List::begin();
    }
    iterator end() {
        return (iterator) L2List::end();
    }
    iterator endBefore() {
        return (iterator) L2List::endBefore();;
    }
    iterator beginAfter() {
        return (iterator) L2List::beginAfter();
    }

    const_iterator begin() const {
        return (const_iterator) L2List::begin();
    }
    const_iterator end() const {
        return (const_iterator) L2List::end();
    }
    const_iterator endBefore() const {
        return (const_iterator) L2List::endBefore();
    }
    const_iterator beginAfter() const {
        return (const_iterator) L2List::beginAfter();
    }
};

#endif /* L2LIST_TEXT_H */
