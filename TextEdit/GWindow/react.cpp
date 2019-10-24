//
// File "func.cpp"
// Test of small graphic package: Draw a graph of function y = f(x)
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <sys/times.h>
#include "gwindow.h"

static const double DT_MAX = 10.;
bool finished = false;

//--------------------------------------------------
// Definition of our main class "MyWindow"
//
class MyWindow: public GWindow {    // Our main class
public:
    bool started;       // Red light is fired
    bool measuring;     // Green light
    bool measured;      // Light is turned off, time is measured
    bool falstart;
    timer_t timerid;
    double dt;
    I2Rectangle lightRect;
public:
    MyWindow():
        started(false),
        measuring(false),
        measured(false),
        falstart(false),
        timerid(0),
        dt(0.),
        lightRect(I2Point(10, 100), 100, 40)
    {}

    virtual void onExpose(XEvent& event);
    virtual void onKeyPress(XEvent& event);
    virtual void onButtonPress(XEvent& event);
    virtual bool onWindowClosing();
};

//
// Process the Expose event: draw in the window
//
void MyWindow::onExpose(XEvent& /* event */) {
    // Erase a window
    setForeground(getBackground());
    fillRectangle(m_RWinRect);

    if (started) {
        if (!measuring) {
            setForeground("red");
        } else {
            setForeground("green");
        }
        fillRectangle(lightRect);
    }

    setForeground("black");
    drawString(
        10, 20, 
        "Measure your reaction!" 
    );
    drawString(
        10, 40, 
        "First, press any key (\'q\' for quit)." 
    );
    drawString(
        10, 60, 
        "Then, when light will change to green,"
    );
    drawString(10, 80, "press any key again.");

    if (measured) {
        char line[64];
        if (falstart) {
            strcpy(line, "Falstart!");
        } else {
            sprintf(line, "Your reaction: %.3f sec", dt);
            if (dt >= 10.)
                strcpy(line, "Your reaction is more than 10 sec");
        }
        setForeground("red");
        drawString(10, 110, line);
    } 
}

//
// Process the KeyPress event:
// if "q" is pressed, then close the window
//
void MyWindow::onKeyPress(XEvent& event) {
    KeySym key;
    char keyName[256];
    if (!started) {
        int nameLen = XLookupString(&(event.xkey), keyName, 255, &key, 0);
        printf("KeyPress: keycode=0x%x, state=0x%x, KeySym=0x%x\n",
            event.xkey.keycode, event.xkey.state, (int) key);
        if (nameLen > 0) {
            keyName[nameLen] = 0;
            printf("\"%s\" button pressed.\n", keyName);
            if (keyName[0] == 'q' || keyName[0] == 'Q') { 
                // quit => close window
                destroyWindow();
                finished = true;
            }
        }
        started = true;
        measured = false;
        measuring = false;
        falstart = false;
    } else if (!measuring) {
        started = false;
        falstart = true;
        measuring = false;
        measured = true;
        printf("Falstart!\n");
    } else {
        struct itimerspec curtim;
        timer_gettime(timerid, &curtim);
        dt = DT_MAX;
        dt -= (double) curtim.it_value.tv_sec +
            (double) curtim.it_value.tv_nsec / 1e9;
        if (dt < 0.1) {
            falstart = true;
            printf("Falstart!\n");
        } else {
            printf("Measured: dt=%.3f\n", dt);
        }
        started = false;
        measuring = false;
        measured = true;
    }
    redraw();
}

// Process mouse click
void MyWindow::onButtonPress(XEvent& event) {
    int x = event.xbutton.x;
    int y = event.xbutton.y;
    int mouseButton = (int) event.xbutton.button;

    printf(
        "Mouse click: x=%d, y=%d, button=%d\n", 
        x, y, mouseButton
    );
}

bool MyWindow::onWindowClosing() {
    finished = true;
    return true;
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

    struct tms tim;
    srand(times(&tim));

    MyWindow w;
    w.createWindow(
        I2Rectangle(            // Window frame rectangle:
            I2Point(10, 10),    //     left-top corner
            400,                //     width
            200                 //     height
        ),
        R2Rectangle(                    // Coordinate rectangle:
            R2Point(-12., -9.),         //     bottom-right corner
            24., 18.                    //     width, height
        ),
        "Measure your Reaction"         // Window title
    );
    w.setBackground("lightGray");

    struct sigevent sev;
    memset(&sev, 0, sizeof(sev));
    timer_t timerid;
    sev.sigev_notify = SIGEV_NONE;
    
    int res = timer_create(
        CLOCK_REALTIME,
        &sev,
        &timerid
    );
    if (res != 0) {
        perror("Could not create a timer");
        exit(-1);
    }
    w.timerid = timerid;

    //... GWindow::messageLoop();
    // Message loop, animation
    XEvent e;
    struct itimerspec timerspc;
    memset(&timerspc, 0, sizeof(timerspc));
    bool starting = false;
    while (!finished) {
        if (GWindow::getNextEvent(e)) {
            GWindow::dispatchEvent(e);
        } else {
            if (w.started && !starting && !w.measuring) {
                // Wait random time 1..5 sec
                int i = rand()%400 + 100;   // in 0.01 sec
                timerspc.it_value.tv_sec = i / 100;
                timerspc.it_value.tv_nsec = (i % 100)
                    * 10000000;
                res = timer_settime(
                    timerid,
                    0,
                    &timerspc,
                    NULL
                );
                if (res != 0) {
                    perror("Could not start timer");
                    exit(-1);
                }
                starting = true;
                w.measuring = false;
            } else if (w.started && starting && !w.measuring) {
                res = timer_gettime(
                    timerid,
                    &timerspc
                );
                if (res != 0) {
                    perror("Could not get time");
                    exit(-1);
                }
                if (
                    timerspc.it_value.tv_sec == 0 &&
                    timerspc.it_value.tv_nsec == 0
                ) {
                    printf("Starting the measurement.\n");
                    starting = false;
                    w.measuring = true;
                    timerspc.it_value.tv_sec = DT_MAX;
                    timerspc.it_value.tv_nsec = 0;
                    res = timer_settime(
                        timerid,
                        0,
                        &timerspc,
                        NULL
                    );
                    if (res != 0) {
                        perror("Could not start timer");
                        exit(-1);
                    }
                    w.redraw();
                    continue;
                }
            }

            // Sleep a bit (we use select for sleeping)
            timeval dt;

            // Sleep 0.01 sec
            dt.tv_sec = 0;
            dt.tv_usec = 10000; // sleeping time 0.01 sec

            select(1, 0, 0, 0, &dt);
        }
    }

    GWindow::closeX();
    return 0;
}
