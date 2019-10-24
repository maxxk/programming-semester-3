//
// File "KeySym.cpp"
// Test of X11 keyboard
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GWindow/gwindow.h"

//--------------------------------------------------
// Definition of our main class "MyWindow"
//
class MyWindow: public GWindow {    // Our main class
    unsigned int keycode;   // Read from XKeyEvent
    unsigned int state;

    KeySym key;             // Obtained by call of XLookupString
    char keyName[256];
    int keyNameLen;
    bool wasQ;              // For quit: "q" was pressed

    Font textFont;          // Font used
    XFontStruct fontStruct;

    unsigned int bgColor;
    unsigned int fgColor;

    int dy;                     // Line skip
    int ascent;                 // Font ascent
    int margin;                 // Margin

public:
    MyWindow():                 // Constructor
        keycode(0),
        state(0),
        key(0),
        keyNameLen(0),
        wasQ(false),
        textFont(0),
        fontStruct(),
        bgColor(0),
        fgColor(0),
        dy(0),
        ascent(0),
        margin(8)
    {
        keyName[0] = 0;
    }

    void createWindow();
    void loadTextFont();

    virtual void onExpose(XEvent& event);
    virtual void onKeyPress(XEvent& event);
};

//----------------------------------------------------------
// Implementation of class "MyWindow"

void MyWindow::createWindow() {
    if (GWindow::m_Display == 0) {
        if (!GWindow::initX()) {
            perror("A connection with X server could not be established");
            exit(1);
        }
    }

    loadTextFont();

    setBgColorName("LightGray");
    setFgColorName("black");

    GWindow::createWindow(
        I2Rectangle(                    // Window frame rectangle:
            I2Point(20, 20),            //     left-top corner
            GWindow::screenMaxX()*3/4,  //     width
            2*margin + 3*dy             //     height
        ),
        "X11 Keyboard Test"             // Window title
    );

    // Save background and foreground colors
    bgColor = getBackground();
    fgColor = getForeground();

    setFont(textFont);
}

static const char* const DEFAULT_TEXT_FONT =
    "-*-courier-medium-r-normal-*-18-*-*-*-*-*-koi8-r";

void MyWindow::loadTextFont() {
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

    ascent = fontStruct.max_bounds.ascent;
    dy = ascent + fontStruct.max_bounds.descent;
    int leading = dy / 8;                       // Interline skip
    if (leading <= 2)
        leading = 2;
    dy += leading;
}

//
// Process the Expose event: draw in the window
//
void MyWindow::onExpose(XEvent& /* event */) {
    char line[512];

    // Erase a window
    setForeground(getBackground());
    fillRectangle(m_RWinRect);

    setForeground("blue");
    int x = margin; 
    int y = margin + ascent;
    drawString(x, y, "Press any key (\"q\" 2 times for quit):");
    y += dy;

    if (keycode != 0) {
        setForeground(fgColor);
        sprintf(line, "XKeyEvent: keycode=0x%x, state=0x%x",
            keycode, state);
        drawString(x, y, line);
        y += dy;

        sprintf(
            line,
            "XLookupString: keysym=0x%x, keyNameLen=%d, keyName=\"%s\"",
            (int) key, keyNameLen, keyName
        );
        drawString(x, y, line);
    }
}

//
// Process the KeyPress event: 
// if "q" is pressed, then close the window
//
void MyWindow::onKeyPress(XEvent& event) {
    keycode = event.xkey.keycode;
    state = event.xkey.state;
    keyNameLen = XLookupString(
        &(event.xkey), keyName, 255, &key, 0
    );
    // printf("KeyPress: keycode=0x%x, state=0x%x, KeySym=0x%x\n",
    //     event.xkey.keycode, event.xkey.state, (int) key);
    if (keyNameLen > 0) {
        keyName[keyNameLen] = 0;
        // printf("\"%s\" button pressed.\n", keyName);
        if (keyName[0] == 'q') { // quit 2 times => close window
            if (wasQ) {
                destroyWindow();
                return;
            }
            wasQ = true;
        } else {
            wasQ = false;
        }
    } else {
        keyName[0] = 0;
    }
    redraw();
}

//
// End of class MyWindow implementation
//----------------------------------------------------------

/////////////////////////////////////////////////////////////
// Main: initialize X, create an instance of MyWindow class,
//       and start the message loop
int main() {
    // Initialize X stuff
    if (!GWindow::initX()) {
        printf("Could not connect to X-server.\n");
        exit(1);
    }

    MyWindow w;
    w.setBgColorName("LightGray");
    w.setFgColorName("black");
    w.createWindow();

    GWindow::messageLoop();

    GWindow::closeX();
    return 0;
}
