//
// File "func.cpp"
// Test of small graphic package: Draw a graph of function y = f(x)
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gwindow.h"

static const int MAX_POINTS = 10;

//--------------------------------------------------
// Definition of our main class "MyWindow"
//
class MyWindow: public GWindow {    // Our main class
    R2Point point[MAX_POINTS];      // Mouse clicks
    unsigned int mouseButton[MAX_POINTS];
    int numPoints;
public:
    MyWindow():                     // Constructor
        numPoints(0)
    {}

    double f(double x);             // Function y = f(x)
    void drawGraphic();             // Draw graph of function

    virtual void onExpose(XEvent& event);
    virtual void onKeyPress(XEvent& event);
    virtual void onButtonPress(XEvent& event);
};

//----------------------------------------------------------
// Implementation of class "MyWindow"

//
// Function y = f(x)
//
double MyWindow::f(double x) {
    return sin(x) * 8./(1. + 0.1*x*x);
}

//
// Process the Expose event: draw in the window
//
void MyWindow::onExpose(XEvent& /* event */) {
    // Erase a window
    setForeground(getBackground());
    fillRectangle(m_RWinRect);

    // Draw the coordinate axes
    setLineWidth(1);
    drawAxes("black", true, "gray");

    // Draw a graph of function
    setLineWidth(2);
    setForeground("blue");
    drawGraphic();
    drawString(
        R2Point(5., 8.), "y = sin(x) * 8/(1 + 0.1*x*x)"
    );

    // Draw mouse clicks
    R2Vector dx(0.3, 0.);
    R2Vector dy(0., 0.3);
    for (int i = 0; i < numPoints; ++i) {
        if (mouseButton[i] == Button1)
            setForeground("red");       // Left button
        else if (mouseButton[i] == Button2)
            setForeground("brown");     // Middle button
        else if (mouseButton[i] == Button3)
            setForeground("SeaGreen");  // Right button
        drawLine(point[i]-dx, point[i]+dx);
        drawLine(point[i]-dy, point[i]+dy);
    }
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
        }
    }
}

// Process mouse click
void MyWindow::onButtonPress(XEvent& event) {
    if (numPoints >= MAX_POINTS)
        numPoints = 0;

    int x = event.xbutton.x;
    int y = event.xbutton.y;
    mouseButton[numPoints] = event.xbutton.button;

    printf(
        "Mouse click: x=%d, y=%d, button=%d\n",
        x, y, mouseButton[numPoints]
    );

    point[numPoints] = invMap(I2Point(x, y));
    ++numPoints;
    redraw();
}

//
// Draw graph of function in a window
//
void MyWindow::drawGraphic() {
    R2Point p;
    double dx = 0.01;
    double xmax = getXMax();

    p.x = getXMin();
    p.y = f(p.x);

    moveTo(p);
    while (p.x < xmax) {
        drawLineTo(p);
        p.x += dx;
        p.y = f(p.x);
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
    double aspect = (double) GWindow::screenMaxX() /
        (double) GWindow::screenMaxY();
    double width = 30.;
    int height = width / aspect;
    w.createWindow(
        I2Rectangle(                    // Window frame rectangle:
            I2Point(10, 10),            //     left-top corner
            GWindow::screenMaxX()/2,    //     width
            GWindow::screenMaxY()/2     //     height
        ),
        R2Rectangle(                        // Coordinate rectangle:
            R2Point(-width/2., -height/2.), //     bottom-right corner
            width, height                   //     width, height
         ),
        "Graph of Function"             // Window title
    );
    w.setBackground("LightGray");

    GWindow::messageLoop();

    GWindow::closeX();
    return 0;
}
