//
// File "mondrian.cpp"
// Simple screen saver: test of drawing/filling of various shapes
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <signal.h>
#include <deque>
#include "gwindow.h"

class Shape {
public:
    R2Rectangle rect;
    const char *color;
    virtual void draw(GWindow* w) = 0;
    Shape():
        color("black")
    {}
    virtual ~Shape() {}
};

class RectangleShape: public Shape {
public:
    virtual void draw(GWindow* w) {
        bool offscreen = (w->m_Pixmap != 0);
        w->setForeground(color);
        w->fillRectangle(rect, offscreen);
    }
};

class EllipticShape: public Shape {
public:
    virtual void draw(GWindow* w) {
        bool offscreen = (w->m_Pixmap != 0);
        w->setForeground(color);
        w->fillEllipse(rect, offscreen);
    }
};

class TriangleShape: public Shape {
public:
    virtual void draw(GWindow* w) {
        bool offscreen = (w->m_Pixmap != 0);
        R2Point triangle[3];
        triangle[0] = R2Point(rect.left(), rect.bottom());
        triangle[1] = R2Point(rect.right(), rect.bottom());
        triangle[2] = R2Point((rect.left() + rect.right())/2., rect.top());
        w->setForeground(color);
        w->fillPolygon(triangle, 3, offscreen);
    }
};

// Colors defined in "/usr/share/X11/rgb.txt"
const char * const COLORS[] = {
    "red", "green", "blue", "magenta", "yellow", "brown",
    "cyan", "navy", "gray", "silver", "white", "black",
    "ivory", "AliceBlue", "lavender", "MistyRose", "SlateGrey",
    "MidnightBlue", "SlateBlue", "SkyBlue", "turquoise",
    "aquamarine", "MediumSeaGreen", "LawnGreen", "LimeGreen",
    "goldenrod", "RosyBrown", "IndianRed", "wheat", "salmon",
    "orange", "coral", "violet", "plum", "purple", "seashell1", "moccasin"
};
int NUM_COLORS = sizeof(COLORS) / sizeof(const char *);

size_t MAX_SHAPES = 256;

static void sigHandler(int sigID);      // Handler of Ctrl+C

static clock_t clocks_per_sec = CLOCKS_PER_SEC;
static const int ANIMATION_DT = 333;    // milliseconds
static bool finished = false;

//--------------------------------------------------
// Definition of our main class "Mondrian"
//
class Mondrian: public GWindow { // Our main class
public:
    std::deque<Shape*> shapes;

    // Animation
    clock_t prevMoment;
    clock_t currentMoment;
    clock_t animation_dt;

    bool stopped;

    // Offscreen buffer
    bool initialUpdate;
    bool offscreenDrawing;

    Mondrian():                     // Constructor
        shapes(),
        prevMoment(0),
        currentMoment(0),
        animation_dt(CLOCKS_PER_SEC / 2),
        stopped(false),
        initialUpdate(true),
        offscreenDrawing(false)
    {}

    void animate();
    void drawInOffscreen();

    virtual void onExpose(XEvent& event);
    virtual void onKeyPress(XEvent& event);
    virtual void onButtonPress(XEvent& event);
    virtual bool onWindowClosing();
    virtual void onResize(XEvent& event);
};

//
// Process the Expose event: draw in the window
//
void Mondrian::onExpose(XEvent& /* event */) {
    if (initialUpdate) {
        initialUpdate = false;

        // Try to create the offscreen buffer
        if (!offscreenDrawing)
            offscreenDrawing = createOffscreenBuffer();
        if (offscreenDrawing)
            drawInOffscreen();
    }

    if (offscreenDrawing) {
        swapBuffers();
        return;
    }

    // Erase a window
    setForeground(getBackground());
    fillRectangle(m_RWinRect);

    for (size_t i = 0; i < shapes.size(); ++i)
        shapes[i]->draw(this);

    struct tms tim;
    prevMoment = times(&tim); // Remember the moment of drawing
}

void Mondrian::drawInOffscreen() {
    // Erase a window
    setForeground(getBackground());
    fillRectangle(m_RWinRect, true);

    for (size_t i = 0; i < shapes.size(); ++i)
        shapes[i]->draw(this);

    struct tms tim;
    prevMoment = times(&tim); // Remember the moment of drawing
}

void Mondrian::animate() {
    if (finished || stopped)
        return;

    struct tms tim;
    currentMoment = times(&tim);
    if (prevMoment == 0)
        prevMoment = currentMoment;     // Initialization
    clock_t dt = currentMoment - prevMoment;
    if (dt >= animation_dt) {
        double x = (double) rand() / double(RAND_MAX);
        double y = (double) rand() / double(RAND_MAX);
        double w = 0.1 + 0.4 * (double) rand() / double(RAND_MAX);
        double h = 0.1 + 0.4 * (double) rand() / double(RAND_MAX);

        int t = rand() % 4;
        Shape* s;
        if (t <= 1)
            s = new RectangleShape();
        else if (t <= 2)
            s = new EllipticShape();
        else
            s = new TriangleShape();

        s->rect.setLeft(m_RWinRect.left() + m_RWinRect.width() * x);
        s->rect.setBottom(m_RWinRect.bottom() + m_RWinRect.height() * y);
        s->rect.setWidth(
            (
                m_RWinRect.width() -
                (s->rect.left() - m_RWinRect.left())
            ) * w
        );
        s->rect.setHeight(
            (
                m_RWinRect.height() -
                (s->rect.bottom() - m_RWinRect.bottom())
            ) * h
        );
        int c = rand() % NUM_COLORS;
        s->color = COLORS[c];

        if (shapes.size() >= MAX_SHAPES) {
            delete shapes.front();
            shapes.pop_front();
        }
        shapes.push_back(s);

        s->draw(this);

        prevMoment = times(&tim);
        if (offscreenDrawing)
            swapBuffers();
    }
}

void Mondrian::onResize(XEvent& /* event */) {
    if (offscreenDrawing) {
        drawInOffscreen();
        swapBuffers();
    }
}

//
// Process the KeyPress event: 
// if "q" is pressed, then close the window
//
void Mondrian::onKeyPress(XEvent& event) {
    KeySym key;
    char keyName[256];
    int nameLen = XLookupString(&(event.xkey), keyName, 255, &key, 0);
    printf("KeyPress: keycode=0x%x, state=0x%x, KeySym=0x%x\n",
        event.xkey.keycode, event.xkey.state, (int) key);
    if (nameLen > 0) {
        keyName[nameLen] = 0;
        printf("\"%s\" button pressed.\n", keyName);
        if (keyName[0] == 'q' || keyName[0] == 'Q') { // quit => close window
            finished = true;
            destroyWindow();
        } else if (keyName[0] == ' ') { // space => pause/run
            stopped = !stopped;
        } else if (keyName[0] == 'f') { // faster
            animation_dt /= 2;
            if (animation_dt < 1)
                animation_dt = 1;
        } else if (keyName[0] == 's') { // slower
            animation_dt *= 2;
            if (animation_dt > 10*clocks_per_sec)
                animation_dt = 10*clocks_per_sec;
        }
    }
}

// Process mouse click
void Mondrian::onButtonPress(XEvent& event) {
    int x = event.xbutton.x;
    int y = event.xbutton.y;
    unsigned int mouseButton = event.xbutton.button;

    printf("Mouse click: x=%d, y=%d, button=%d\n", x, y, mouseButton);

    //... lastClick = invMap(I2Point(x, y));
    //... clicked = true;
    redraw();
}

bool Mondrian::onWindowClosing() {
    finished = true;
    return true;
}

//
// End of implementation of class Mondrian
//----------------------------------------------------------

/////////////////////////////////////////////////////////////
// Main: initialize X, create an instance of Mondrian class,
//       and start the message loop
int main() {
    // Initialize random generator
    struct tms tim;
    srand(times(&tim));

    // Set signal handler for Ctrl+C
    if (signal(SIGINT, &sigHandler) == SIG_ERR) {
        perror("Cannot install a signal handler");
    }

    // Initialize X stuff
    if (!GWindow::initX()) {
        printf("Could not connect to X-server.\n");
        exit(1);
    }

    Mondrian w;
    int s = GWindow::screenMaxX()*2/3;
    if (GWindow::screenMaxY() < GWindow::screenMaxX())
        s = GWindow::screenMaxY()*2/3;

    w.createWindow(
        I2Rectangle(                    // Window frame rectangle:
            I2Point(10, 10),            //     left-top corner
            s,                          //     width
            s                           //     height
        ),
        R2Rectangle(                    // Coordinate rectangle:
            R2Point(-10., -10.),        //     bottom-right corner
            20., 20.                    //     width, height
         ),
        "Clock"                         // Window title
    );
    w.setBackground("black");

    // GWindow::messageLoop();
    clocks_per_sec = (clock_t) sysconf(_SC_CLK_TCK);
    w.animation_dt = (clocks_per_sec*ANIMATION_DT)/1000;

    // Offscreen buffer
    if (w.createOffscreenBuffer()) {
        w.offscreenDrawing = true;
        w.drawInOffscreen();            // Initial drawing
    }

    // Message loop, animation
    XEvent e;
    while (!finished) {
        if (GWindow::getNextEvent(e)) {
            GWindow::dispatchEvent(e);
        } else {

            // Sleep a bit (we use select for sleeping)
            timeval dt;

            // Sleep 0.01 sec
            dt.tv_sec = 0;
            dt.tv_usec = 10000; // sleeping time 0.01 sec

            select(1, 0, 0, 0, &dt);

            w.animate();
        }
    }

    GWindow::closeX();
    return 0;
}

static void sigHandler(int /* sigID */) {
    finished = true;
}
