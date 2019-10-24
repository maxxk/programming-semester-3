#include <stdio.h>
#include "gwindow.h"

class MyWindow: public GWindow {
protected:
    void onExpose(XEvent&) {
        setForeground(getBackground());
        fillRectangle(m_IWinRect);
    }

    void onButtonPress(XEvent& event) {
        char line[256];
        int x = event.xbutton.x;
        int y = event.xbutton.y;
        setForeground("white");
        sprintf(
            line, "Click in %s, m_Window=%d",
            m_WindowTitle, (int) m_Window
        );
        drawString(x, y, line);
    }

    void onKeyPress(XEvent&) {
        char line[256];
        setForeground("white");
        sprintf(
            line, "Key pressed in %s, m_Window=%d", 
            m_WindowTitle, (int) m_Window
        );
        drawString(10, 10, line);
    }
};

int main() {
    GWindow::initX();
    MyWindow w1;
    w1.setBgColorName("brown");
    w1.createWindow(
        I2Rectangle(100, 100, 400, 300),
        R2Rectangle(-2., -1.5, 4., 3.),
        "Brown Window"
    );
    MyWindow w2;
    w2.setBgColorName("SeaGreen");
    w2.setWindowTitle("Green Subwindow");
    w2.m_WindowPosition = I2Point(30, 30);
    w2.m_IWinRect = I2Rectangle(I2Point(20, 20), 300, 150);

    XSetWindowAttributes attributes;
    memset(&attributes, 0, sizeof(attributes));
    attributes.win_gravity = CenterGravity;
    w2.createWindow(
        &w1,                                    // parent window
        1,                                      // border width
        CopyFromParent,                         // window class
        CopyFromParent,                         // visual
        CWWinGravity,                           // which attributes are defined
        &attributes                             // attributes structure
    );

    GWindow::messageLoop();
    GWindow::closeX();
    return 0;
}
