#ifndef L2LIST_H
#define L2LIST_H

class L2ListException {
public:
    const char *reason;
    L2ListException():
        reason("")
    {}

    L2ListException(const char *cause):
        reason(cause)
    {}
};

// An object of class ListHeader contains only the pointers
// to the next and previous ListHeaders. This class is used
// to link elements in the list. Also an objects of this class
// is used to store the header of a list.
//
//     ListHeader:
//          +------+
//          | next |
//          | prev |
//          +------+
//
class L2ListHeader {
public:
    L2ListHeader* next;
    L2ListHeader* prev;

    L2ListHeader():
        next(0),
        prev(0)
    {
    }

    L2ListHeader(L2ListHeader* n, L2ListHeader* p):
        next(n),
        prev(p)
    {
    }

    void link(L2ListHeader* h) {
        next = h;
        h->prev = this;
    }

    // Destructor of list element must be virtual!
    virtual ~L2ListHeader() {}
};

// The idea of implementation of L2List:
// All elements of the list are linked into ring.
// The ring begins with headList element ("header"). If list is
// empty, then the header is linked with itself.
//
//     headList       First elem.     Second elem ... Last element
//     +------+        +------+        +------+        +------+
// +-->|      | ------>|      | ------>|      |     -->|      |---->+
// |   |      |        |      |        |      | . . .  |      |     |
// |+<-|      | < - - -|      | < - - -|      |   < - -|      |<- -+|
// ||  +------+        |      |        |      |        |      |    ||
// ||                  +------+        +------+        +------+    ||
// |+- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --+|
// +----------------------------------------------------------------+
//
// The next element for the headList is the first element of list,
// the previous -- the last element of list.
//
// The elements of the list should be instances of some
// class derived from the class L2ListHeader

class L2List {
private:
    L2ListHeader    headList; // Head of the ring of list elements
    L2ListHeader*   pointer;  // Pointer to element AFTER pointer (element
                              //     BEFORE can be obtained via m_Prev)
    int numElements;          // Number of elements in the list
    int pointerPos;           // Number of elements before a pointer

public:
    L2List():
        headList(&headList, &headList), // Link the head of list with itself
        pointer(&headList),
        numElements(0),
        pointerPos(0)
    {
    }

    void removeAll() {
        moveToBeg();
        while (!inEnd()) {
            removeAfter();
        }
    }

    ~L2List() {
        removeAll();
    }

    void moveToBeg() {
        pointer = headList.next;
        pointerPos = 0;
    }

    void moveToEnd() {
        pointer = &headList;
        pointerPos = size();
    }

    bool inBeg() {
        return (pointer->prev == &headList);
    }

    bool inEnd() {
        return (pointer == &headList);
    }

    void moveForward() {
        if (inEnd()) {
            throw L2ListException("moveForward: End of list");
        } else {
            pointer = pointer->next;
            ++pointerPos;
        }
    }

    void moveBack() {
        if (inBeg()) {
            throw L2ListException("moveBack: Beginning of list");
        } else {
            pointer = pointer->prev;
            --pointerPos;
        }
    }

    L2ListHeader& elementAfter() {
        return *pointer;
    }

    L2ListHeader& elementBefore() {
        return *(pointer->prev);
    }

    /**
     * Add element before a pointer.
     * Includes the block in the list.
     * The block must be allocated in dynamic memory (using "new")
     */
    void addBefore(L2ListHeader* block) {
        pointer->prev->link(block);
        block->link(pointer);
        numElements++;
        pointerPos++;
    }

    /**
     * Remove element before a pointer.
     */
    void removeBefore() {
        if (inBeg()) {
            throw L2ListException("removeBefore: Beginning of list");
        } else {
            L2ListHeader* block = pointer->prev;
            block->prev->link(pointer);
            delete block;
            numElements--;
            pointerPos--;
        }
    }

    /**
     * Add element after a pointer.
     */
    void addAfter(L2ListHeader* block) {
        pointer->prev->link(block);
        block->link(pointer);
        pointer = block;
        numElements++;
    }

    /**
     * Remove element after a pointer.
     */
    void removeAfter() {
        if (inEnd()) {
            throw  L2ListException("removeAfter: End of list");
        } else {
            L2ListHeader* block = pointer;
            pointer = block->next;
            block->prev->link(pointer);
            delete block;
            numElements--;
        }
    }

    int size() const { return numElements; }

    /**
     * Set the pointer after first n elements, if possible.
     * Returns the actual number of elements before pointer
     * on operation completion.
     */
    int setPointer(int n) {
        int s = n - pointerPos;
        if (s < 0) {
            while (!inBeg() && s != 0) {
                moveBack(); ++s;
            }
        } else if (s > 0) {
            while (!inEnd() && s != 0) {
                moveForward(); --s;
            }
        }
        return pointerPos;
    }

    int getPointerPosition() const { return pointerPos; }

    class iterator {
        L2ListHeader* current;
    public:
        iterator():
            current(0)
        {
        }

        iterator(L2ListHeader* h):
            current(h)
        {
        }

        iterator& operator++() {
            current = current->next;
            return *this;
        }

        iterator operator++(int) { // Postfix increment operator
            iterator tmp = *this;
            operator++(); // Apply the prefix increment operator
            return tmp;
        }

        iterator& operator--() {
            current = current->prev;
            return *this;
        }

        iterator operator--(int) { // Postfix decrement operator
            iterator tmp = *this;
            operator--(); // Apply the prefix decrement operator
            return tmp;
        }

        L2ListHeader& operator*() const {
            return *current;
        }

        L2ListHeader* operator->() const {
            return current;
        }

        bool operator==(const iterator& i) const {
            return (current == i.current);
        }

        bool operator!=(const iterator& i) const {
            return !operator==(i);
        }
    };

    class const_iterator: public iterator {
    public:
        const_iterator():
            iterator()
        {
        }

        const_iterator(const L2ListHeader* h):
            iterator(const_cast<L2ListHeader*>(h))
        {
        }

        const_iterator(const iterator& i):
            iterator(i)
        {
        }

        const L2ListHeader& operator*() const {
            return ((const iterator*) this)->operator*();
        }

        const L2ListHeader* operator->() const {
            return &(
                ((const iterator*) this)->operator*()
            );
        }
    };

    iterator begin() { return iterator(headList.next); }
    iterator end() { return iterator(&headList); }

    // end of elements before pointer
    iterator endBefore() { return iterator(pointer); }

    // beginning of elements after pointer
    iterator beginAfter() { return iterator(pointer); }

    const_iterator begin() const { return const_iterator(headList.next); }
    const_iterator end() const { return const_iterator(&headList); }
    const_iterator endBefore() const { return const_iterator(pointer); }
    const_iterator beginAfter() const { return const_iterator(pointer); }
};

#endif /* L2LIST_H */
