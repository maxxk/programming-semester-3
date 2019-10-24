//
// File "func.cpp"
// Simple animation example: Draw a clock with moving arrows
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <signal.h>
#include "gwindow.h"

static void sigHandler(int sigID);      // Handler of Ctrl+C

static const double PI = 3.14159265358979323846;
static clock_t clocks_per_sec = CLOCKS_PER_SEC;
static const int ANIMATION_DT = 1000;   // milliseconds
static bool finished = false;

//--------------------------------------------------
// Definition of our main class "ClockWindow"
//
class ClockWindow: public GWindow { // Our main class
public:
    // Cosines used to draw the clock face
    static double cosines[360], sines[360];
    static bool cosinesInitialized;
    static void initializeCosines();

    // Position of arrows radians
    double hourAngle;
    double minuteAngle;
    double secondAngle;

    // Current time
    int hours;
    int minutes;
    int seconds;

    // Animation
    clock_t prevMoment;
    clock_t currentMoment;

    // Arrows positions
    R2Vector secondArrow;
    R2Vector minuteArrow;
    R2Vector hourArrow;

    // Offscreen buffer
    bool initialUpdate;
    bool offscreenDrawing;

    ClockWindow():                     // Constructor
        hourAngle(0.),
        minuteAngle(0.),
        secondAngle(0.),
        hours(0),
        minutes(0),
        seconds(0),
        prevMoment(0),
        currentMoment(0),
        secondArrow(),
        minuteArrow(),
        hourArrow(),
        initialUpdate(true),
        offscreenDrawing(false)
    {}

    void animate();
    void defineCurrentTime();
    void drawFace();
    void drawArrows(bool erase = false);
    void drawArrow(
        const R2Point& center,
        const R2Vector& arrow,
        double widthCoeff
    );
    void drawInOffscreen();

    virtual void onExpose(XEvent& event);
    virtual void onKeyPress(XEvent& event);
    virtual void onButtonPress(XEvent& event);
    virtual bool onWindowClosing();
    virtual void onResize(XEvent& event);
};

double ClockWindow::cosines[360], ClockWindow::sines[360];
bool ClockWindow::cosinesInitialized = false;

void ClockWindow::initializeCosines() {
    if (!cosinesInitialized) {
        cosinesInitialized = true;
        double dAlpha = M_PI / 180.;    // 1 degree
        double alpha = 0.;
        for (int i = 0; i < 360; ++i) {
            cosines[i] = cos(alpha);
            sines[i] = sin(alpha);
            alpha += dAlpha;
        }
    }
}

//
// Process the Expose event: draw in the window
//
void ClockWindow::onExpose(XEvent& /* event */) {
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

    drawFace();

    struct tms tim;
    prevMoment = times(&tim);  // Remember the moment of drawing
    defineCurrentTime();
    drawArrows(false);
}

void ClockWindow::drawInOffscreen() {
    // Erase a window
    setForeground(getBackground());
    fillRectangle(m_RWinRect, true);

    drawFace();

    struct tms tim;
    prevMoment = times(&tim);  // Remember the moment of drawing
    defineCurrentTime();
    drawArrows(false);
}

void ClockWindow::animate() {
    if (finished)
        return;

    struct tms tim;
    currentMoment = times(&tim);
    if (prevMoment == 0)
        prevMoment = currentMoment;     // Initialization
    clock_t dt = currentMoment - prevMoment;
    if (dt >= clocks_per_sec) {
        if (!offscreenDrawing) {
            drawArrows(true);
        }
        prevMoment = times(&tim);
        defineCurrentTime();
        if (!offscreenDrawing) {
            drawArrows(false);
        } else {
            drawInOffscreen();
            swapBuffers();
        }
    }
}

void ClockWindow::defineCurrentTime() {
    time_t t = time(0);  // Time since 1 Jan 1970
    if (t == ((time_t) -1)) {
        perror("Cannot define a time");
        exit(1);
    }
    struct tm curTime = *localtime(&t);
    hours = curTime.tm_hour;
    minutes = curTime.tm_min;
    seconds = curTime.tm_sec;

    // Angles of arrows
    hourAngle = (double)hours * 3600. +
        (double)minutes * 60. + (double)seconds;
    // Convert to 12-hours
    hourAngle = fmod(hourAngle, 43200.);    // 12 hours == 43200 sec

    hourAngle = PI/2. - hourAngle * (2.*PI)/43200.;

    minuteAngle = (double)minutes * 60. + (double)seconds;
    minuteAngle = PI/2. - minuteAngle * (2.*PI)/3600.;

    secondAngle = (double)seconds;
    secondAngle = PI/2. - secondAngle * (2.*PI)/60.;
}

static double cosAlpha[60];
static double sinAlpha[60];
static bool cosDefined = false;
static void defineCosines() {
    if (cosDefined)
        return;
    for (int i = 0; i < 60; ++i) {
        double alpha = (PI/2.) - ((double)i * 6. * PI / 180.);
        cosAlpha[i] = cos(alpha);
        sinAlpha[i] = sin(alpha);
    }
    cosDefined = true;
}

void ClockWindow::drawFace() {
    double h = m_RWinRect.height();
    double w = m_RWinRect.width();
    double s = h;
    if (w < s)
        s = w;

    R2Point center(
        (m_RWinRect.left() + m_RWinRect.right()) / 2.,
        (m_RWinRect.top() + m_RWinRect.bottom()) / 2.
    );

    R2Vector ex(s/2., 0.);
    R2Vector ey(0., s/2.);

    ex *= 0.95;
    ey *= 0.95;

    setForeground("navy");

    if (!cosDefined)
        defineCosines();

    for (int i = 0; i < 60; ++i) {
        R2Vector v = ex*cosAlpha[i] + ey*sinAlpha[i];
        double lenCoeff = 0.05;
        if (i % 10 == 0) {
            lenCoeff = 0.16;
        } else if (i % 5 == 0) {
            lenCoeff = 0.12;
        }
        drawLine(
            center + v * (1. - lenCoeff),
            center + v,
            offscreenDrawing
        );
    }
}

void ClockWindow::drawArrows(bool erase /* = false */) {
    double maxlen = m_RWinRect.height();
    double w = m_RWinRect.width();
    if (w < maxlen)
        maxlen = w;
    double secLen = maxlen * 0.40;
    double minLen = secLen * 0.95;
    double hourLen = minLen * 0.7;

    R2Point center(
        (m_RWinRect.left() + m_RWinRect.right()) / 2.,
        (m_RWinRect.top() + m_RWinRect.bottom()) / 2.
    );

    R2Vector secArrow, minArrow, hArrow;
    if (erase) {
        secArrow = secondArrow;
        minArrow = minuteArrow;
        hArrow = hourArrow;
    } else {
        secArrow =
            R2Vector(cos(secondAngle), sin(secondAngle)) * secLen;
        minArrow =
            R2Vector(cos(minuteAngle), sin(minuteAngle)) * minLen;
        hArrow =
            R2Vector(cos(hourAngle), sin(hourAngle)) * hourLen;
    }

    if (erase)
        setForeground(getBackground());

    // Hour arrow
    if (!erase)
        setForeground("navy");
    moveTo(center);
    //... drawLineTo(center + hArrow);
    drawArrow(center, hArrow, 0.1);

    // Minute arrow
    if (!erase)
        setForeground("SlateGray");
    moveTo(center);
    //... drawLineTo(center + minArrow);
    drawArrow(center, minArrow, 0.05);

    // Second arrow
    if (!erase)
        setForeground("red");
    moveTo(center);
    //... drawLineTo(center + secArrow);
    drawArrow(center, secArrow, 0.025);

    if (!erase) {
        secondArrow = secArrow;
        minuteArrow = minArrow;
        hourArrow = hArrow;
    }
}

void ClockWindow::drawArrow(
    const R2Point& center,
    const R2Vector& arrow,
    double widthCoeff
) {
    R2Vector h = arrow.normal();
    h *= widthCoeff;
    R2Vector opp = arrow * (-widthCoeff*1.3);
    /*...
    moveTo(center + opp);
    drawLineTo(center + h);
    drawLineTo(center + arrow);
    drawLineTo(center - h);
    drawLineTo(center + opp);
    ...*/
    R2Point points[4];
    points[0] = center + opp;
    points[1] = center + h;
    points[2] = center + arrow;
    points[3] = center - h;
    fillPolygon(points, 4, offscreenDrawing);
}

//
// Process the KeyPress event: 
// if "q" is pressed, then close the window
//
void ClockWindow::onKeyPress(XEvent& event) {
    KeySym key;
    char keyName[256];
    int nameLen = XLookupString(&(event.xkey), keyName, 255, &key, 0);
    printf("KeyPress: keycode=0x%x, state=0x%x, KeySym=0x%x\n",
        event.xkey.keycode, event.xkey.state, (int) key);
    if (nameLen > 0) {
        keyName[nameLen] = 0;
        printf("\"%s\" button pressed.\n", keyName);
        finished = true;
        /*
        if (keyName[0] == 'q') { // quit => close window
            destroyWindow();
        }
        */
        destroyWindow();
    }
}

// Process mouse click
void ClockWindow::onButtonPress(XEvent& event) {
    int x = event.xbutton.x;
    int y = event.xbutton.y;
    unsigned int mouseButton = event.xbutton.button;

    printf("Mouse click: x=%d, y=%d, button=%d\n", x, y, mouseButton);

    //... lastClick = invMap(I2Point(x, y));
    //... clicked = true;
    redraw();
}

bool ClockWindow::onWindowClosing() {
    finished = true;
    return true;
}

void ClockWindow::onResize(XEvent& /* event */) {
    if (offscreenDrawing) {
        drawInOffscreen();
        swapBuffers();
    }
}

//
// End of implementation of class ClockWindow
//----------------------------------------------------------

/////////////////////////////////////////////////////////////
// Main: initialize X, create an instance of ClockWindow class,
//       and start the message loop
int main() {
    // Set signal handler for Ctrl+C
    if (signal(SIGINT, &sigHandler) == SIG_ERR) {
        perror("Cannot install a signal handler");
    }

    // Initialize X stuff
    if (!GWindow::initX()) {
        printf("Could not connect to X-server.\n");
        exit(1);
    }

    int numDepths = 0;
    int *depths = XListDepths(
        GWindow::m_Display,
        DefaultScreen(GWindow::m_Display),
        &numDepths
    );
    if (depths != 0) {
        printf("Display depths: ");
        for (int i = 0; i < numDepths; ++i) {
            printf(" %d", depths[i]);
        }
        printf("\n");
    }


    ClockWindow w;
    int s = GWindow::screenMaxX()/3;
    if (GWindow::screenMaxY() < GWindow::screenMaxX())
        s = GWindow::screenMaxY()/3;

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
    w.setBackground("lightGray");

    // GWindow::messageLoop();
    clocks_per_sec = (clock_t) sysconf(_SC_CLK_TCK);

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
