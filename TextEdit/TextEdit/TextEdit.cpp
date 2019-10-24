//
// class TextEdit, implementation
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <limits.h>

#include <X11/keysym.h> /* X11 key symbols (in "/usr/include/X11/keysymdef.h") */
#include <X11/Xutil.h>

#include "TextEdit.h"

//
// Editor commands
//
const struct TextEdit::CommandDsc TextEdit::editorCommands[] = {
    // Down
    {XK_Down, 0, ControlMask, false, &TextEdit::onDown},      // down arrow
    {XK_KP_Down, 0, ControlMask, false, &TextEdit::onDown},   // down arrow on keypad

    // Up
    {XK_Up, 0, ControlMask, false, &TextEdit::onUp},          // up arrow
    {XK_KP_Up, 0, ControlMask, false, &TextEdit::onUp},       // up arrow on right keypad

    // Left
    {XK_Left, 0, ControlMask, false, &TextEdit::onLeft},      // left arrow
    {XK_KP_Left, 0, ControlMask, false, &TextEdit::onLeft},   // left arrow on keypad

    // Right
    {XK_Right, 0, ControlMask, false, &TextEdit::onRight},    // right arrow
    {XK_KP_Right, 0, ControlMask, false, &TextEdit::onRight}, // right arrow on keypad

    // To the beginning of line
    {XK_Home, 0, ControlMask, false, &TextEdit::onHome},      // Home
    {XK_KP_Home, 0, ControlMask, false, &TextEdit::onHome},   // Home on keypad
    {XK_Left, ControlMask, ControlMask, false, &TextEdit::onHome},    // Ctrl+left
    {XK_KP_Left, ControlMask, ControlMask, false, &TextEdit::onHome}, // Ctrl+left on keypad

    // To the end of line
    {XK_End, 0, ControlMask, false, &TextEdit::onEnd},      // End
    {XK_KP_End, 0, ControlMask, false, &TextEdit::onEnd},   // End on right keypad
    {XK_Right, ControlMask, ControlMask, false, &TextEdit::onEnd},    // Ctrl+right arrow
    {XK_KP_Right, ControlMask, ControlMask, false, &TextEdit::onEnd}, // Ctrl+right keypad

    // To the beginning of text
    {XK_Home, ControlMask, ControlMask, false, &TextEdit::onTextBeg},   // Ctrl+Home
    {XK_KP_Home, ControlMask, ControlMask, false, &TextEdit::onTextBeg},// Ctrl+Home keypad
    {XK_Up, ControlMask, ControlMask, false, &TextEdit::onTextBeg},     // Ctrl+Up
    {XK_KP_Up, ControlMask, ControlMask, false, &TextEdit::onTextBeg},  // Ctrl+Up

    // To the end of text
    {XK_End, ControlMask, ControlMask, false, &TextEdit::onTextEnd},    // Ctrl+End
    {XK_KP_End, ControlMask, ControlMask, false, &TextEdit::onTextEnd}, // Ctrl+End
    {XK_Down, ControlMask, ControlMask, false, &TextEdit::onTextEnd},   // Ctrl+Down
    {XK_KP_Down, ControlMask, ControlMask, false, &TextEdit::onTextEnd},// Ctrl+Down keypad

    // Tabulation to the right
    {XK_Tab, 0, ShiftMask, false, &TextEdit::onTab},                // Tab

    // Tabulation to the left
    {XK_Tab, ShiftMask, ShiftMask, false, &TextEdit::onTabLeft},    // Shift+Tab
    {XK_ISO_Left_Tab, ShiftMask, ShiftMask, false, &TextEdit::onTabLeft}, // Shift+Tab

    // Page Up, Down
    {XK_Page_Up, 0, 0, false, &TextEdit::onPageUp},             // Page Up
    {XK_Page_Down, 0, 0, false, &TextEdit::onPageDown},         // Page Down

    // Back Space (delete previous character)
    {XK_BackSpace, 0, 0, true, &TextEdit::onBackSpace},         // Back Space

    // Delete current character
    {XK_Delete, 0, ShiftMask, true, &TextEdit::onDelete},       // Delete
    {XK_KP_Delete, 0, ShiftMask, true, &TextEdit::onDelete},    // Delete on keypad

    // Delete line
    {XK_Delete, ShiftMask, ShiftMask, true, &TextEdit::onDeleteLine},  // Shift+Delete
    {XK_KP_Delete, ShiftMask, ShiftMask, true, &TextEdit::onDeleteLine}, // Shift+Delete on keypad
    {XK_k, ControlMask, ControlMask, true, &TextEdit::onDeleteLine},   // Ctrl+k
    {XK_K, ControlMask, ControlMask, true, &TextEdit::onDeleteLine},   // Ctrl+K

    // Insert space
    {XK_Insert, 0, ShiftMask, true, &TextEdit::onInsert},       // Insert
    {XK_KP_Insert, 0, ShiftMask, true, &TextEdit::onInsert},    // Insert on keypad

    // Insert line
    {XK_Insert, ShiftMask, ShiftMask, true, &TextEdit::onInsertLine}, // Shift+Insert
    {XK_KP_Insert, ShiftMask, ShiftMask, true, &TextEdit::onInsertLine}, // Shift+Insert on keypad
    {XK_l, ControlMask, ControlMask, true, &TextEdit::onInsertLine},  // Ctrl+l
    {XK_L, ControlMask, ControlMask, true, &TextEdit::onInsertLine},  // Ctrl+L

    // Return, Enter
    {XK_Return, 0, ShiftMask, true, &TextEdit::onEnter},    // Enter
    {XK_KP_Enter, 0, ShiftMask, true, &TextEdit::onEnter},  // Enter on keypad

    // Quit
    {XK_q, ControlMask, ControlMask, false, &TextEdit::onQuit}, // Ctrl+q
    {XK_Q, ControlMask, ControlMask, false, &TextEdit::onQuit}, // Ctrl+Q
    {XK_q, Mod1Mask, Mod1Mask, false, &TextEdit::onQuit},   // Alt+q
    {XK_Q, Mod1Mask, Mod1Mask, false, &TextEdit::onQuit},   // Alt+Q

    // Save text
    {XK_s, ControlMask, ControlMask, false, &TextEdit::onSave}, // Ctrl+s
    {XK_S, ControlMask, ControlMask, false, &TextEdit::onSave}, // Ctrl+S
    {XK_s, Mod1Mask, Mod1Mask, false, &TextEdit::onSave},   // Alt+s
    {XK_S, Mod1Mask, Mod1Mask, false, &TextEdit::onSave},   // Alt+S

    {XK_a, ControlMask, ControlMask, false, &TextEdit::onDeleteWord}, // Ctrl+a
    {XK_A, ControlMask, ControlMask, false, &TextEdit::onDeleteWord}, // Ctrl+A

    {0, 0, 0, false, 0}                                     // Terminator
};

static int IOErrorHandler(Display* /* display */) {
    printf("Connection to X Server broken...\n");
    return 0;
}

int main(int argc, char *argv[]) {
    GWindow::initX();
    TextEdit *editor = new TextEdit();
    if (argc > 1) {
        editor->loadFile(argv[1]);
    }
    editor->createWindow();

    XSetIOErrorHandler(&IOErrorHandler);

    GWindow::messageLoop();

    GWindow::closeX();
    delete editor;
}

TextEdit::TextEdit():   // Constructor
    GWindow(),          // Base class constructor
    text(),             // Text storage

    cursorX(0),         // Cursor position in the text
    cursorY(0),

    windowX(0),         // Window position in the text
    windowY(0),

    windowWidth(80),    // Window size in characters (dx, dy)
    windowHeight(24),

    lastChar(0),        // The character typed

    textFont(0),        // Font used
    fontStruct(),       // Font properties
    fontName(0),        // Font name (in X11 form)

    // Font metric (we use a fixed width font!)
    dx(0),              // Maximal character advance (width)
    ascent(0),          // Character ascent
    descent(0),         // Character descent
    leading(0),         // Interline skip
    dy(0),              // Font height = ascent + descent + leading
    leftMargin(4),      // Margins
    topMargin(4),
    statusLineMargin(4),
    rightMargin(4),
    bottomMargin(4),

    fileName("noname.txt"),     // Path to a file
    fileNameSet(false),         // File name is assigned
    endOfText("[* End of text *]"),

    textChanged(false),
    textSaved(false),
    inputDisabled(false),
    focusIn(true),

    bgColor(0),
    fgColor(0),
    bgStatusLineColor(0),
    fgStatusLineColor(0)
{
}

void TextEdit::createWindow() {
    if (GWindow::m_Display == 0) {
        if (!GWindow::initX()) {
            perror("A connection with X server could not be established");
            exit(1);
        }
    }

    // Load font, calculate window size, etc.
    initialize();

    strncpy(m_WindowTitle, getFileName(), 127);
    setBgColorName("LightGray");
    setFgColorName("black");

    GWindow::createWindow();

    // Save background and foreground colors
    bgColor = getBackground();
    fgColor = getForeground();

    // Status line colors               /usr/X11R6/lib/X11/rgb.txt
    bgStatusLineColor = allocateColor("MidnightBlue");
    fgStatusLineColor = allocateColor("white");

    setFont(textFont);

    // Hints for window manager
    XSizeHints sizeHints;
    memset(&sizeHints, 0, sizeof(sizeHints));

    // Minimal size 16 * 1
    sizeHints.min_width = 44*dx + leftMargin + rightMargin;
    sizeHints.min_height = 1*dy + topMargin + bottomMargin;

    // Base size
    sizeHints.base_width = 0*dx + leftMargin + rightMargin;
    sizeHints.base_height = 0*dy + topMargin + bottomMargin;

    // Resize increment
    sizeHints.width_inc = dx;
    sizeHints.height_inc = dy;

    sizeHints.flags = (PMinSize | PBaseSize | PResizeInc);
    XSetWMNormalHints(m_Display, m_Window, &sizeHints);
}

void TextEdit::initialize() {
    loadTextFont();             // Loads font, fills in the fontStruct

    // Save font metrics, calculate window size
    dx = fontStruct.max_bounds.width;           // Maximal character advance
    ascent = fontStruct.max_bounds.ascent;      // Character ascent
    descent = fontStruct.max_bounds.descent;    // Character descent
    int height = ascent + descent;
    leading = height / 8;                       // Interline skip
    if (leading <= 2)
        leading = 2;
    dy = ascent + descent + leading;

    topMargin += dy + statusLineMargin; // Allocate space for the status line

    // Calculate window rectangle
    m_IWinRect.setLeft(10);
    m_IWinRect.setTop(10);
    m_IWinRect.setWidth(leftMargin + windowWidth * dx + rightMargin);
    m_IWinRect.setHeight(topMargin + windowHeight * dy + bottomMargin);
}

static const char* const DEFAULT_TEXT_FONT =
    "-adobe-courier-medium-r-normal-*-18-*-*-*-*-*-koi8-r";

void TextEdit::loadTextFont() {
    bool success = false;

    // At first, we read the value of XMIMFONT variable
    const char *fontString = getenv("XMIMFONT");
    if (fontString != 0) {
        if ((textFont = loadFont(fontString)) != 0)
            success = true;
    }

    // Then, try to load the default fomt
    if (!success) {
        if ((textFont = loadFont(DEFAULT_TEXT_FONT)) != 0)
            success = true;
    }

    // At last, try to load a font with the name "fixed"
    // (the "fixed" must be set as a font alias in any X-window system)
    if (!success) {
        if ((textFont = loadFont("fixed")) != 0)
            success = true;
    }
    if (!success) {
        perror("Cannot load a font with name \"fixed\"");
        exit(1);
    }

    // Query the font metrics
    XFontStruct* fStruct = queryFont(textFont);
    if (fStruct == 0) {
        perror("Could not load a font");
        exit(1);
    }
    fontStruct = *fStruct;
}

bool TextEdit::loadFile(const char* filePath) {
    setFileName(filePath);
    fileNameSet = true;
    return text.load(filePath);
}

void TextEdit::redrawStatusLine() {
    redrawRectangle(
        I2Rectangle(0, 0, m_IWinRect.width(), dy + statusLineMargin)
    );
}

void TextEdit::drawStatusLine(bool createGC /* = false */) {
    GC savedGC;
    if (createGC) {
        // Save the previous graphic contex, create a temporary GC
        savedGC = m_GC;
        m_GC = XCreateGC(m_Display, m_Window, 0, 0);
        setFont(textFont);
    }

    setForeground(bgStatusLineColor);
    fillRectangle(
        I2Rectangle(0, 0, m_IWinRect.width(), dy + statusLineMargin)
    );

    setForeground(fgStatusLineColor);

    char statusLine[256];
    int x = leftMargin;
    int y = statusLineMargin + ascent;
    sprintf(statusLine, "col=%d", cursorX+1);
    drawString(x, y, statusLine);

    sprintf(statusLine, "row=%d", cursorY+1);
    drawString(x + 8*dx, y, statusLine);

    if (textChanged)
        drawString(x + 19*dx, y, "Modified");
    else if (textSaved)
        drawString(x + 19*dx, y, "Saved");

    drawString(m_IWinRect.width() - 15*dx, y, "Ctrl+Q to quit");

    if (createGC) {
        // Release the temporary graphic contex, restore the previous GC
        XFreeGC(m_Display, m_GC);
        m_GC = savedGC;
    }
}

void TextEdit::onExpose(XEvent& /* event */) {
    // Draw a status line
    drawStatusLine();

    // Erase a window
    setForeground(bgColor);
    fillRectangle(
        I2Rectangle(
            0, topMargin - statusLineMargin, 
            m_IWinRect.width(), m_IWinRect.height()
        )
    );

    // Draw a text in a window
    setForeground(fgColor);
    int x = leftMargin;
    int y = topMargin + ascent;
    for (int screenY = 0; screenY < windowHeight; screenY++, y += dy) {
        int textY = windowY + screenY;
        const TextLine* currentLine;
        if (textY > text.size()) {
            break;
        } else if (textY == text.size()) {
            currentLine = &(endOfText);
        } else {
            currentLine = &(text.getLine(textY));
        }
        int len = currentLine->length();
        if (len > windowX) {
            int restrictedLen = len - windowX;
            if (restrictedLen > windowWidth)
                restrictedLen = windowWidth;
            drawString(
                x, y,
                currentLine->getString() + windowX,
                restrictedLen
            );
        }
    }

    // Draw cursor
    if (!inputDisabled) {
        drawCursor(cursorX, cursorY, true);
    }
}

void TextEdit::drawCursor(
    int cx, int cy, bool on, bool createGC /* = false */
) {
    bool cursorOn = on;
    if (!focusIn || inputDisabled)
        cursorOn = false;
    if (
        cx < windowX || cy < windowY ||
        cx >= windowX + windowWidth || 
        cy >= windowY + windowHeight
    )
        return;         // Cursor outside of window

    GC savedGC;
    if (createGC) {
        // Save the previous graphic contex, create a temporary GC
        savedGC = m_GC;
        m_GC = XCreateGC(m_Display, m_Window, 0, 0);
        setFont(textFont);
    }

    if (cursorOn) setForeground(fgColor);
    else setForeground(bgColor);

    int x = leftMargin + (cx - windowX) * dx;
    int y = topMargin + (cy - windowY) * dy;

    fillRectangle(
        I2Rectangle(x, y, dx, ascent + descent)
    );

    if (cursorOn) setForeground(bgColor);
    else setForeground(fgColor);

    if (cy <= text.size()) {
        TextLine* line;
        if (cy == text.size()) {
            line = &(endOfText);
        } else {
            line = &(text.getLine(cy));
        }

        if (cx < line->length()) {
            drawString(
                x, y + ascent,
                line->getString() + cx, 1
            );
        }
    }

    if (createGC) {
        // Release the temporary graphic contex, restore the previous GC
        XFreeGC(m_Display, m_GC);
        m_GC = savedGC;
    }
}

void TextEdit::setFileName(const char* filePath) {
    fileName = filePath;
    fileNameSet = true;
}

const char* TextEdit::getFileName() const { return fileName; }

void TextEdit::onKeyPress(XEvent& event) {
    if (inputDisabled) 
        return;

    preProcessCommand();

    // State of modifiers keys (Shift, Controld, Alt, etc.)
    unsigned int state = event.xkey.state;

    KeySym keySymbol; // X11 keyboard symbol (see "/usr/include/X11/keysymdef.h")
    char keyName[256];
    int keyNameLen;

    keyNameLen = XLookupString( // define keyboard symbol
        &(event.xkey), keyName, 255, &keySymbol, 0
    );

    // Look up the command in the table
    const struct CommandDsc* command = editorCommands;
    bool commandFound = false;
    while (!commandFound && command->method != 0) {
        if (
            keySymbol == command->keysym &&
            (state & command->stateMask) == command->state
        ) {
            commandFound = true; break;
        }
        ++command;
    }

    if (commandFound) {
        (this->*(command->method))();   // Perform the command
        if (command->change) {
            textChanged = true;         // Command changes the text
        }
    } else if ((state & ControlMask) == 0) {
        // This is not a Control character
        if (keyNameLen > 0) {
            // Normal character (Latin letter, etc.)
            lastChar = keyName[0];
            onCharTyped();
            textChanged = true;
        } else if ((keySymbol & 0x8000) == 0) {
            // This is not a special character. Probably, it is a Russian letter
            lastChar = (keySymbol & 0xff);
            onCharTyped();
            textChanged = true;
        }
    }

    postProcessCommand();
}

// Actions to be performed before any command
void TextEdit::preProcessCommand() {
    inputDisabled = true; // Disable any input while command is not completed
    drawCursor(cursorX, cursorY, false, true);  // Remove cursor
}

// Actions to be performed after any command
void TextEdit::postProcessCommand() {
    if (m_Window == 0)  // Window is destroyed => return
        return;

    if (textChanged)
        textSaved = false;

    // Scroll the window to the cursor, if necessary
    if (
        cursorX < windowX || cursorX > windowX + windowWidth - 1 ||
        cursorY < windowY || cursorY > windowY + windowHeight - 1
    ) {
        // Cursor is removed at the moment!
        scrollToCursor();
    }

    // Set pointer in the text
    text.setPointer(cursorY);

    inputDisabled = false;

    drawCursor(cursorX, cursorY, true, true);
    drawStatusLine(true);
}

void TextEdit::onFocusIn(XEvent& /* event */) {
    focusIn = true;
    if (!inputDisabled)
        drawCursor(cursorX, cursorY, true, true);
}

void TextEdit::onFocusOut(XEvent& /* event */) {
    focusIn = false;
    drawCursor(cursorX, cursorY, false, true);
}

void TextEdit::onButtonPress(XEvent& event) {
    if (inputDisabled)
        return;

    // Calculate the new position of cursor
    int x = event.xbutton.x;
    int y = event.xbutton.y;
    int cx = (x - leftMargin) / dx;
    if (cx >= windowWidth)
        cx = windowWidth - 1;
    cx += windowX;

    int cy = (y - topMargin) / dy;
    if (cy >= windowHeight)
        cy = windowHeight - 1;
    cy += windowY;
    if (cy > text.size())
        cy = text.size();

    // Erase the old curson and draw the new one
    drawCursor(cursorX, cursorY, false, true);
    cursorX = cx; cursorY = cy;
    drawCursor(cursorX, cursorY, true, true);
}

void TextEdit::onResize(XEvent& /* event */) {
    int w = m_IWinRect.width();
    windowWidth = (w - leftMargin) / dx;
    rightMargin = w - leftMargin - windowWidth * dx;
    if (windowWidth <= 0) {
        windowWidth = 1; rightMargin = 0;
    }

    int h = m_IWinRect.height();
    windowHeight = (h - topMargin) / dy;
    bottomMargin = h - topMargin - windowHeight * dy;
    if (windowHeight <= 0) {
        windowHeight = 1; bottomMargin = 0;
    }

    //... redraw();
}

void TextEdit::onDown() {
    if (cursorY < text.size())
        ++cursorY;
}

void TextEdit::onUp() {
    if (cursorY > 0)
        --cursorY;
}

void TextEdit::onLeft() {
    if (cursorX > 0)
        --cursorX;
}

void TextEdit::onRight() {
    ++cursorX;
}

void TextEdit::onPageUp() {
    cursorY -= windowHeight;
    if (cursorY < 0)
        cursorY = 0;
}

void TextEdit::onPageDown() {
    cursorY += windowHeight;
    if (cursorY > text.size())
        cursorY = text.size();
}

void TextEdit::scrollToCursor() {
    if (cursorX < windowX) {
        scrollLeft(windowX - cursorX);
    } else if (cursorX > windowX + windowWidth - 1) {
        scrollRight(cursorX - (windowX + windowWidth - 1));
    }

    if (cursorY < windowY) {
        scrollUp(windowY - cursorY);
    } else if (cursorY > windowY + windowHeight - 1) {
        scrollDown(cursorY - (windowY + windowHeight - 1));
    }
}

void TextEdit::scrollLeft(int n) {
    if (n == 0)
        return;
    if (n > windowX) 
        n = windowX;
    windowX -= n;
    if (n > windowWidth / 2) {
        redraw();
    } else {
        int shift = dx * n;
        XCopyArea(
            m_Display, m_Window, m_Window, m_GC,
            leftMargin, topMargin,
            windowWidth * dx - shift, windowHeight * dy,
            leftMargin + shift, topMargin
        );
        redrawRectangle(
            I2Rectangle(
                leftMargin, topMargin,
                shift, windowHeight * dy
            )
        );
    }
}

void TextEdit::scrollRight(int n) {
    if (n == 0)
        return;
    windowX += n;
    if (n > windowWidth / 2) {
        redraw();
    } else {
        int shift = dx * n;
        XCopyArea(
            m_Display, m_Window, m_Window, m_GC,
            leftMargin + shift, topMargin,
            windowWidth * dx - shift, windowHeight * dy,
            leftMargin, topMargin
        );
        redrawRectangle(
            I2Rectangle(
                leftMargin + windowWidth*dx - shift, topMargin,
                shift, windowHeight * dy
            )
        );
    }
}

void TextEdit::scrollUp(int n) {
    if (n == 0)
        return;
    if (n > windowY)
        n = windowY;
    windowY -= n;
    if (n > windowHeight / 2) {
        redraw();
    } else {
        int shift = dy * n;
        XCopyArea(
            m_Display, m_Window, m_Window, m_GC,
            leftMargin, topMargin,
            windowWidth * dx, windowHeight * dy - shift,
            leftMargin, topMargin + shift
        );
        redrawRectangle(
            I2Rectangle(
                leftMargin, topMargin,
                windowWidth * dx, shift
            )
        );
    }
}

void TextEdit::scrollDown(int n) {
    if (n == 0)
        return;
    windowY += n;
    if (n > windowHeight / 2) {
        redraw();
    } else {
        int shift = dy * n;
        XCopyArea(
            m_Display, m_Window, m_Window, m_GC,
            leftMargin, topMargin + shift,
            windowWidth * dx, windowHeight * dy - shift,
            leftMargin, topMargin
        );
        redrawRectangle(
            I2Rectangle(
                leftMargin, topMargin + windowHeight * dy - shift,
                windowWidth * dx, shift
            )
        );
    }
}

void TextEdit::onHome() {
    cursorX = 0;
}

void TextEdit::onEnd() {
    if (cursorY >= text.size()) {
        cursorX = 0;
    } else {
        cursorX = text.getLine(cursorY).length();
    }
}

void TextEdit::onTextBeg() {
    cursorX = 0;
    cursorY = 0;
}

void TextEdit::onTextEnd() {
    cursorX = 0;
    cursorY = text.size();
}

static const int TAB_DX = 4;

void TextEdit::onTab() {
    ++cursorX;
    if ((cursorX % TAB_DX) != 0)
        cursorX += TAB_DX - (cursorX % TAB_DX);
}

void TextEdit::onTabLeft() {
    if (cursorX > 0) {
        --cursorX;
        if ((cursorX % TAB_DX) != 0)
            cursorX -= (cursorX % TAB_DX);
    }
}

void TextEdit::onBackSpace() { // Delete previous character
    if (cursorX <= 0 || cursorY >= text.size())
        return;
    --cursorX;
    onDelete();
}

void TextEdit::onDelete() { // Delete current character
    if (cursorY >= text.size())
        return;
    TextLine& line = text.getLine(cursorY);
    if (cursorX < line.length()) {
        line.removeAt(cursorX);
    }
    line.trim();
    redrawTextRectangle(cursorX, cursorY, INT_MAX, 1);
}

void TextEdit::onInsert() { // Insert a space
    lastChar = ' ';
    onCharTyped();
    --cursorX;
}

void TextEdit::onCharTyped() {
    if (lastChar == 0) 
        return;

    if (cursorY == text.size()) 
        onInsertLine();
    text.setPointer(cursorY);
    TextLine& line = text.getLine(cursorY);

    if (cursorX > line.length()) {
        int extraSpaces = cursorX - line.length();
        line.ensureCapacity(line.length() + extraSpaces + 2);
        while(--extraSpaces >= 0)
            line.append(' ');   // Add space to the end of line
    }
    line.insert(cursorX, lastChar);
    line.trim();
    cursorX++;
    redrawTextRectangle(cursorX - 1, cursorY, INT_MAX, 1, true);
}

void TextEdit::onDeleteLine() {
    if (cursorY >= text.size())
        return;
    text.setPointer(cursorY);
    text.removeAfter();
    redrawTextRectangle(0, cursorY, INT_MAX, INT_MAX);
}

void TextEdit::onInsertLine() {
    text.setPointer(cursorY);
    text.addAfter(new TextLine());
    redrawTextRectangle(0, cursorY, INT_MAX, INT_MAX);
}

void TextEdit::onEnter() {
    text.setPointer(cursorY);
    if (cursorY < text.size()) {
        TextLine& line = text.getLine(cursorY);
        text.moveForward();
        int l = line.length();
        if (cursorX >= l) {
            text.addBefore(new TextLine());
        } else {
            text.addBefore(
                new TextLine(line.getString() + cursorX)
            );
            line.truncate(cursorX);
            line.trim();
        }
    } else {
        text.addBefore(new TextLine());
    }
    cursorX = 0;
    ++cursorY;
    redrawTextRectangle(0, cursorY - 1, INT_MAX, INT_MAX);
}

void TextEdit::onSave() {
    if (textChanged) {
        if (text.save(fileName)) {
            textChanged = false;
            textSaved = true;
        }
    }
}

void TextEdit::onQuit() {
    if (processQuit())
        close();
}

bool TextEdit::processQuit() {
    bool quit = true;
    if (textChanged) {
        SaveDialog saveDialog(this);
        saveDialog.create();
        saveDialog.doModal();
        if (saveDialog.buttonPressed == SaveDialog::BUTTON_YES) {
            onSave();
        } else if (saveDialog.buttonPressed == SaveDialog::BUTTON_NO) {
            // Nothing to do.
        } else if (saveDialog.buttonPressed == SaveDialog::BUTTON_CANCEL) {
            quit = false;
        }
    }
    return quit;
}

// Close text editor window, quit
void TextEdit::close() {
    destroyWindow();
}

// This virtual method is called when user presses the window close box
bool TextEdit::onWindowClosing() {
    return processQuit();
}

void TextEdit::redrawTextRectangle(
    int x, int y, int w, int h, bool createGC /* = false */
) {
    int x1 = x + w;
    if (w == INT_MAX) 
        x1 = INT_MAX;
    int y1 = y + h;
    if (h == INT_MAX) 
        y1 = INT_MAX;

    int x0 = x;
    if (x0 < windowX) 
        x0 = windowX;
    int y0 = y;
    if (y0 < windowY)  
        y0 = windowY;

    if (x1 > windowX + windowWidth) 
        x1 = windowX + windowWidth;
    if (y1 > windowY + windowHeight) 
        y1 = windowY + windowHeight;
    if (x1 <= x || y1 <= y) 
        return;

    GC savedGC;
    if (createGC) {
        // Save the previous graphic contex, create a temporary GC
        savedGC = m_GC;
        m_GC = XCreateGC(m_Display, m_Window, 0, 0);
        setFont(textFont);
    }

    int left = leftMargin + (x0 - windowX) * dx;
    int top = topMargin + (y0 - windowY) * dy;
    int width = (x1 - x0) * dx;
    int height = (y1 - y0) * dy;

    if (left + width > m_IWinRect.width())
        width -= (left + width - m_IWinRect.width());
    if (top + height > m_IWinRect.height())
        height -= (top + height - m_IWinRect.height());

    if (!createGC) {
        redrawRectangle(
            I2Rectangle(left, top, width, height)
        );
    } else {
        // Erase a window
        setForeground(bgColor);
        fillRectangle(
            I2Rectangle(left, top, width, height)
        );

        // Draw a text in a window
        setForeground(fgColor);
        int iy = top + ascent;
        for (int yy = y0; yy < y1; yy++, iy += dy) {
            const TextLine* currentLine;
            if (yy > text.size()) {
                break;
            } else if (yy == text.size()) {
                currentLine = &(endOfText);
            } else {
                currentLine = &(text.getLine(yy));
            }
            int len = currentLine->length();
            if (len > x0) {
                len -= x0;
                if (len > x1 - x0)
                    len = x1 - x0;
                drawString(
                    left, iy,
                    currentLine->getString() + x0,
                    len
                );
            }
        }
    }

    if (createGC) {
        // Release the temporary graphic contex, restore the previous GC
        XFreeGC(m_Display, m_GC);
        m_GC = savedGC;
    }
}

///////////////////////////////////////
// class SaveDialog, Implementation

SaveDialog::SaveDialog(TextEdit* e):
    GWindow(),
    editor(e),
    yesButtonRect(),
    noButtonRect(),
    cancelButtonRect(),
    buttonWidth(0),
    buttonHeight(0),
    dialogWidth(0),
    dialogHeight(0),
    buttonColor1(0),
    buttonColor2(0),
    buttonPressed(SaveDialog::BUTTON_CANCEL)
{
    buttonWidth = 8 * editor->dx;
    buttonHeight = editor->dy + 2 * editor->statusLineMargin;

    dialogWidth = 4 * editor->leftMargin + 3 * buttonWidth;
    int w = 33 * editor->dx + 2 * editor->leftMargin; // Text width
    if (w > dialogWidth)
        dialogWidth = w;

    int skip = (dialogWidth - 2 * editor->leftMargin -
        3 * buttonWidth) / 2;

    dialogHeight = 3 * editor->statusLineMargin + editor->dy + buttonHeight;

    int x = editor->leftMargin;
    int y = 2 * editor->bottomMargin + editor->dy;

    yesButtonRect = I2Rectangle(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + skip;

    noButtonRect = I2Rectangle(x, y, buttonWidth, buttonHeight);
    x += buttonWidth + skip;

    cancelButtonRect = I2Rectangle(x, y, buttonWidth, buttonHeight);
}

void SaveDialog::create() {
    setWindowTitle("Quit Dialog");
    setBgColorName("LightGray");
    setFgColorName("black");
    m_IWinRect = I2Rectangle(
        editor->m_IWinRect.left() + 30,
        editor->m_IWinRect.top() + 30,
        dialogWidth,
        dialogHeight
    );

    setBgColorName("SlateGray2");       // "/usr/X11R6/lib/X11/rgb.txt"
    GWindow::createWindow(editor);

    setFont(editor->textFont);

    textColor = allocateColor("black");
    buttonColor1 = allocateColor("white");
    buttonColor2 = textColor;
    buttonColor3 = allocateColor("SlateGray3");
}

void SaveDialog::onExpose(XEvent& /* event */) {
    setForeground(getBackground());
    fillRectangle(m_IWinRect);

    setForeground(textColor);
    drawString(
        editor->leftMargin,
        editor->statusLineMargin + editor->ascent,
        "A text has been changed, save it?"
    );

    drawButton(yesButtonRect, "Yes");
    drawButton(noButtonRect, "No");
    drawButton(cancelButtonRect, "Cancel");
}

void SaveDialog::drawButton(
    const I2Rectangle& rect, const char* text
) {
    setForeground(buttonColor3);
    fillRectangle(rect);

    setForeground(buttonColor1);
    moveTo(rect.left(), rect.bottom());
    drawLineTo(rect.left(), rect.top());
    drawLineTo(rect.right(), rect.top());

    setForeground(buttonColor2);
    drawLineTo(rect.right(), rect.bottom());
    drawLineTo(rect.left(), rect.bottom());

    setForeground(textColor);
    int len = strlen(text);
    int skip = (rect.width() - editor->dx * len) / 2;
    if (skip < 2)
        skip = 2;

    drawString(
        rect.left() + skip,
        rect.top() + editor->statusLineMargin + editor->ascent,
        text
    );
}

void SaveDialog::onButtonPress(XEvent& event) {
    I2Point p(event.xbutton.x, event.xbutton.y);
    bool closeDialog = false;

    if (yesButtonRect.contains(p)) {
        buttonPressed = BUTTON_YES;
        closeDialog = true;
    } else if (noButtonRect.contains(p)) {
        buttonPressed = BUTTON_NO;
        closeDialog = true;
    } else if (cancelButtonRect.contains(p)) {
        buttonPressed = BUTTON_CANCEL;
        closeDialog = true;
    }

    if (closeDialog) {
        destroyWindow();
    }
}

void TextEdit::onDeleteWord() {
    TextLine& line = text.getLine(cursorY);
    char atCursor = line.at(cursorX);
    printf("Delete word. Current cursor: %d %d, char: %c\n", cursorX, cursorY, atCursor);
}
