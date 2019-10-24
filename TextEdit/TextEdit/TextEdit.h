//
// Simple text editor for X11
// Based on XLib only.
//
// Interface of class TextEdit
//
#ifndef TEXT_EDIT_H
#define TEXT_EDIT_H

#include "GWindow/gwindow.h"    // Graphic window interface
#include <X11/keysym.h>

#include "Text.h"               // Text based on L2List

/**
 * Simple text editor.
 */
class TextEdit: public GWindow {
    Text text;          // Text storage

    int cursorX;        // Cursor position in the text
    int cursorY;

    int windowX;        // Window position in the text
    int windowY;

    int windowWidth;    // Window size in characters (dx, dy)
    int windowHeight;

    int lastChar;       // The character typed

    Font textFont;          // Font used
    XFontStruct fontStruct; // Font properties
    TextLine fontName;      // Font name (in X11 form)

    // Font metric (we use a fixed width font!)
    int dx;             // Maximal character advance (width)
    int ascent;         // Character ascent
    int descent;        // Character descent
    int leading;        // Interline skip
    int dy;             // Font height = ascent + descent + leading
    int leftMargin;     // Margins
    int topMargin;
    int statusLineMargin;
    int rightMargin;
    int bottomMargin;

    TextLine fileName;      // Path to a file
    bool fileNameSet;       // File name is assigned
    TextLine endOfText;     // "[* End of text *]" line

    bool textChanged;
    bool textSaved;
    bool inputDisabled;
    bool focusIn;

    // Colors
    unsigned long bgColor;  // Background color
    unsigned long fgColor;  // Foreground color
    unsigned long bgStatusLineColor;    // Status line colors
    unsigned long fgStatusLineColor;

public:
    TextEdit();
    void setFileName(const char* filePath);
    const char* getFileName() const;
    bool loadFile(const char* filePath);
    void createWindow();

protected:
    // X events processing
    virtual void onExpose(XEvent& event);
    virtual void onKeyPress(XEvent& event);
    virtual void onButtonPress(XEvent& event);
    virtual void onResize(XEvent& event);
    virtual void onFocusIn(XEvent& event);
    virtual void onFocusOut(XEvent& event);

    virtual bool onWindowClosing();

    // Scrolling methods
    void scrollToCursor();
    void scrollLeft(int n);
    void scrollRight(int n);
    void scrollUp(int n);
    void scrollDown(int n);

    // Cursor drawing/erasing
    void drawCursor(int cx, int cy, bool on, bool createGC = false);

    // Status line
    void drawStatusLine(bool createGC = false);
    void redrawStatusLine();

    // Actions performed before and after any command
    void preProcessCommand();
    void postProcessCommand();

    // Editor commands

    // Cursor movement
    void onDown();      // Down arrow
    void onUp();        // Up arrow
    void onLeft();      // Left arrow
    void onRight();     // Right arrow
    void onHome();      // To the beginning of line
    void onEnd();       // To the end of line
    void onTextBeg();   // To the beginning of text
    void onTextEnd();   // To the end of text
    void onTab();       // Tabulation
    void onTabLeft();   // Tabulation to the left
    void onPageUp();    // Page Up
    void onPageDown();  // Page Down

    // "Horizontal" (inline) text changing commands
    void onBackSpace(); // Delete previous character
    void onDelete();    // Delete current character
    void onInsert();    // Insert a space
    void onCharTyped(); // Insert a character typed

    // "Vertical" text changing commands
    void onDeleteLine();   // Delete a complete line
    void onInsertLine();   // Insert an empty above the current
    void onEnter();        // Divide a current line in two pieces

    // Save file
    void onSave();      // Save a text in a file
    void onSaveAs();    // ...not implemented yet...

    void onQuit();      // Ask user, if text was changed, save, and quit
    void close();       // Close editor (quit without questions)

    bool processQuit(); // Process the "Quit" request. Returns "true"
                        // if window should be closed

    // Create new text / open new file
    void onNew();       // ...not implemented yet...
    void onOpen();      // ...not implemented yet...

    void onDeleteWord();

    // Redraw a rectangle in a text
    void redrawTextRectangle(
        int x, int y, int w, int h, bool createGC = true
    );

private:
    void initialize();
    void loadTextFont();

    // Command description
    struct CommandDsc {
        KeySym keysym;          // X11 Key (see "/usr/include/X11/keysymdef.h")
        unsigned int state;     // State of Shift, Control, Alt, etc.
        unsigned int stateMask; // Which bits in "state" to consider
        bool change;            // A command changes a text
        void (TextEdit::*method)(); // Pointer to method processing the command
    };

    // Table of commands
    static const struct CommandDsc editorCommands[];

    friend class SaveDialog;
};

class SaveDialog: public GWindow {
private:
    TextEdit*   editor;

    I2Rectangle yesButtonRect;
    I2Rectangle noButtonRect;
    I2Rectangle cancelButtonRect;

    int buttonWidth;
    int buttonHeight;
    int dialogWidth;
    int dialogHeight;

    unsigned long bgColor;
    unsigned long textColor;
    unsigned long buttonColor1;
    unsigned long buttonColor2;
    unsigned long buttonColor3;

public:
    enum {
        BUTTON_YES,
        BUTTON_NO,
        BUTTON_CANCEL
    } buttonPressed;

    SaveDialog(TextEdit* e);
    void create();

private:
    // X events processing
    virtual void onExpose(XEvent& event);
    virtual void onButtonPress(XEvent& event);

    void drawButton(const I2Rectangle& buttonRect, const char* text);
};

#endif /* TEXT_EDIT_H */
