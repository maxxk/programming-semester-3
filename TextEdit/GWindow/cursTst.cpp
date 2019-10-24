//
// File "bezier.cpp"
// Show Cursor Shapes
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <X11/cursorfont.h>
#include "gwindow.h"

static const int NUM_CURSORS = XC_num_glyphs;

static const char * const cursorNames[NUM_CURSORS + 1] = {
    "XC_X_cursor",
    "XC_X_cursor+1",
    "XC_arrow",
    "XC_arrow+1",
    "XC_based_arrow_down",
    "XC_based_arrow_down+1",
    "XC_based_arrow_up",
    "XC_based_arrow_up+1",
    "XC_boat",
    "XC_boat+1",
    "XC_bogosity",
    "XC_bogosity+1",
    "XC_bottom_left_corner",
    "XC_bottom_left_corner+1",
    "XC_bottom_right_corner",
    "XC_bottom_right_corner+1",
    "XC_bottom_side",
    "XC_bottom_side+1",
    "XC_bottom_tee",
    "XC_bottom_tee+1",
    "XC_box_spiral",
    "XC_box_spiral+1",
    "XC_center_ptr",
    "XC_center_ptr+1",
    "XC_circle",
    "XC_circle+1",
    "XC_clock",
    "XC_clock+1",
    "XC_coffee_mug",
    "XC_coffee_mug+1",
    "XC_cross",
    "XC_cross+1",
    "XC_cross_reverse",
    "XC_cross_reverse+1",
    "XC_crosshair",
    "XC_crosshair+1",
    "XC_diamond_cross",
    "XC_diamond_cross+1",
    "XC_dot",
    "XC_dot+1",
    "XC_dotbox",
    "XC_dotbox+1",
    "XC_double_arrow",
    "XC_double_arrow+1",
    "XC_draft_large",
    "XC_draft_large+1",
    "XC_draft_small",
    "XC_draft_small+1",
    "XC_draped_box",
    "XC_draped_box+1",
    "XC_exchange",
    "XC_exchange+1",
    "XC_fleur",
    "XC_fleur+1",
    "XC_gobbler",
    "XC_gobbler+1",
    "XC_gumby",
    "XC_gumby+1",
    "XC_hand1",
    "XC_hand1+1",
    "XC_hand2",
    "XC_hand2+1",
    "XC_heart",
    "XC_heart+1",
    "XC_icon",
    "XC_icon+1",
    "XC_iron_cross",
    "XC_iron_cross+1",
    "XC_left_ptr",
    "XC_left_ptr+1",
    "XC_left_side",
    "XC_left_side+1",
    "XC_left_tee",
    "XC_left_tee+1",
    "XC_leftbutton",
    "XC_leftbutton+1",
    "XC_ll_angle",
    "XC_ll_angle+1",
    "XC_lr_angle",
    "XC_lr_angle+1",
    "XC_man",
    "XC_man+1",
    "XC_middlebutton",
    "XC_middlebutton+1",
    "XC_mouse",
    "XC_mouse+1",
    "XC_pencil",
    "XC_pencil+1",
    "XC_pirate",
    "XC_pirate+1",
    "XC_plus",
    "XC_plus+1",
    "XC_question_arrow",
    "XC_question_arrow+1",
    "XC_right_ptr",
    "XC_right_ptr+1",
    "XC_right_side",
    "XC_right_side+1",
    "XC_right_tee",
    "XC_right_tee+1",
    "XC_rightbutton",
    "XC_rightbutton+1",
    "XC_rtl_logo",
    "XC_rtl_logo+1",
    "XC_sailboat",
    "XC_sailboat+1",
    "XC_sb_down_arrow",
    "XC_sb_down_arrow+1",
    "XC_sb_h_double_arrow",
    "XC_sb_h_double_arrow+1",
    "XC_sb_left_arrow",
    "XC_sb_left_arrow+1",
    "XC_sb_right_arrow",
    "XC_sb_right_arrow+1",
    "XC_sb_up_arrow",
    "XC_sb_up_arrow+1",
    "XC_sb_v_double_arrow",
    "XC_sb_v_double_arrow+1",
    "XC_shuttle",
    "XC_shuttle+1",
    "XC_sizing",
    "XC_sizing+1",
    "XC_spider",
    "XC_spider+1",
    "XC_spraycan",
    "XC_spraycan+1",
    "XC_star",
    "XC_star+1",
    "XC_target",
    "XC_target+1",
    "XC_tcross",
    "XC_tcross+1",
    "XC_top_left_arrow",
    "XC_top_left_arrow+1",
    "XC_top_left_corner",
    "XC_top_left_corner+1",
    "XC_top_right_corner",
    "XC_top_right_corner+1",
    "XC_top_side",
    "XC_top_side+1",
    "XC_top_tee",
    "XC_top_tee+1",
    "XC_trek",
    "XC_trek+1",
    "XC_ul_angle",
    "XC_ul_angle+1",
    "XC_umbrella",
    "XC_umbrella+1",
    "XC_ur_angle",
    "XC_ur_angle+1",
    "XC_watch",
    "XC_watch+1",
    "XC_xterm",
    "XC_xterm+1",
    0
};

//--------------------------------------------------
// Definition of our main class "MyWindow"
//
class MyWindow: public GWindow {    // Our main class
    Cursor currentCursor;
    int currentCursorIdx;
public:
    MyWindow():                     // Constructor
        currentCursor(0),
        currentCursorIdx(-1)
    {}

    bool setCursor(int shape);

    virtual void onExpose(XEvent& event);
    virtual void onKeyPress(XEvent& event);
};

//----------------------------------------------------------
// Implementation of class "MyWindow"

//
// Process the Expose event: draw in the window
//
void MyWindow::onExpose(XEvent& /* event */) {
    // Erase a window
    setForeground(getBackground());
    fillRectangle(m_RWinRect);

    setForeground("black");

    drawString(10, 20, "Show the Shapes of Cursors");
    drawString(10, 40, "Use arrows to go to the next/prev. cusror");
    if (currentCursorIdx >= 0) {
        char line[128];
        sprintf(
            line, "Current cursor shape: %4d = %s",
            currentCursorIdx, cursorNames[currentCursorIdx]
        );

        setForeground("blue");
        drawString(10, 60, line);
    }
}

bool MyWindow::setCursor(int shape) {
    Cursor res = XCreateFontCursor(m_Display, shape);
    bool err = false;
    if (res == BadAlloc) {
        printf("XCreateFontCursor: BadAlloc error\n");
        err = true;
    } else if (res == BadValue) {
        printf("XCreateFontCursor: BadValue error\n");
        err = true;
    }
    if (!err) {
        if (currentCursorIdx >= 0)
            XFreeCursor(m_Display, currentCursor);
        currentCursor = res;
        currentCursorIdx = shape;
        XDefineCursor(m_Display, m_Window, currentCursor);
    }
    return !err;
}

//
// Process the KeyPress event:
// if "q" is pressed, then close the window
//
void MyWindow::onKeyPress(XEvent& event) {
    KeySym key;
    char keyName[256];
    int nameLen = XLookupString(&(event.xkey), keyName, 255, &key, 0);
    printf("KeyPress: keycode=0x%x, state=0x%x, KeySym=0x%x\n",
        event.xkey.keycode, event.xkey.state, (int) key);
    if (nameLen > 0) {
        keyName[nameLen] = 0;
        printf("\"%s\" button pressed.\n", keyName);
        if (keyName[0] == 'q') { // quit => close window
            destroyWindow();
            return;
        }
    }
    if (
        (key == XK_Down || key == XK_KP_Down ||
        key == XK_Right || key == XK_KP_Right) &&
        currentCursorIdx < NUM_CURSORS - 1
    ) {
        setCursor(currentCursorIdx + 1);
        redraw();
    } else if (
        (key == XK_Up || key == XK_KP_Up ||
        key == XK_Left || key == XK_KP_Left) &&
        currentCursorIdx > 0
    ) {
        setCursor(currentCursorIdx - 1);
        redraw();
    }
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
    w.createWindow(
        I2Rectangle(                    // Window frame rectangle:
            I2Point(10, 10),            //     left-top corner
            GWindow::screenMaxX()/3,    //     width
            GWindow::screenMaxY()/4     //     height
        ),
        R2Rectangle(                    // Coordinate rectangle:
            R2Point(-12., -9.),         //     bottom-right corner
            24., 18.                    //     width, height
         ),
        "Cursor Shapes"                 // Window title
    );
    w.setBackground("lightGray");
    w.setCursor(XC_X_cursor);

    GWindow::messageLoop();

    GWindow::closeX();
    return 0;
}
