//
// Classes supporting the 2-dimensional graphics
//
#ifndef R2GRAPH_H
#define R2GRAPH_H

// Contains the definitions of following classes:
//     R2Vector, R2Point, R2Rectangle,
//     I2Vector, I2Point, I2Rectangle
// The "R2" prefix means that the object has real coordinates,
// "I2" means integer coordinates.
//
// In case of real coordinates, we use the normal "mathematical"
// coordinate system, where Y-axis goes up. In case of integer
// coordinates, the Y axis goes down, so that the y-coordinate
// of the bottom of rectangle is greater than the y-coordinate
// of the top. Such integer coordinates are used by default in
// any window system (so called "Text" mode).

#include <math.h>

const double R2GRAPH_EPSILON = 0.0000001;

class R2Vector {
public:
    double x;
    double y;

    R2Vector():                         // Default constructor
        x(0.),
        y(0.)
    {}

    R2Vector(const R2Vector& v):        // Copy-constructor
        x(v.x),
        y(v.y)
    {}

    R2Vector(double xx, double yy):
        x(xx),
        y(yy)
    {}

    R2Vector& operator=(const R2Vector& v) {    // Copy-operator
        x = v.x; y = v.y;
        return *this;
    }

    ~R2Vector() {}                              // Destructor

    R2Vector operator+(const R2Vector& v) const {
        return R2Vector(x+v.x, y+v.y);
    }

    R2Vector& operator+=(const R2Vector& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    R2Vector operator-(const R2Vector& v) const {
        return R2Vector(x-v.x, y-v.y);
    }

    R2Vector& operator-=(const R2Vector& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    R2Vector operator*(double c) const {
        return R2Vector(x*c, y*c);
    }

    R2Vector& operator*=(double c) {
        x *= c;
        y *= c;
        return *this;
    }

    double operator*(const R2Vector& v) const { // Scalar product
        return x*v.x + y*v.y;
    }

    double length() const {
        return sqrt(x*x + y*y);
    }

    R2Vector& normalize() {     // Make length = 1
        //... if (x != 0. || y != 0.) {
        //...     double l = length();
        //...     x /= l;
        //...     y /= l;
        //... }
        double l = length();
        if (l >= R2GRAPH_EPSILON) {
            x /= l;
            y /= l;
        }
        return *this;
    }

    R2Vector normal() const {           // Normal to this vector
        return R2Vector(-y, x);
    }

    double angle(const R2Vector& v) const {  // Angle from this vector to v
        double xx = v * (*this);
        double yy = v * normal();
        return atan2(yy, xx);
    }

    // Comparings
    bool operator==(const R2Vector& v) const {
        //... return (x == v.x && y == v.y);
        return (
            fabs(x - v.x) <= R2GRAPH_EPSILON && 
            fabs(y - v.y) <= R2GRAPH_EPSILON
        );
    }
    bool operator!=(const R2Vector& v) const { return !operator==(v); }
    bool operator>=(const R2Vector& v) const {
        //... return (x > v.x || (x == v.x && y >= v.y));
        return (x > v.x || (x >= v.x && y >= v.y));
    }
    bool operator>(const R2Vector& v) const {
        //... return (x > v.x || (x == v.x && y > v.y));
        return (x > v.x || (x >= v.x && y > v.y));
    }
    bool operator<(const R2Vector& v) const { return !operator>=(v); }
    bool operator<=(const R2Vector& v) const { return !operator>(v); }

    // Area of oriented parallelogram (determinant)
    double signed_area(const R2Vector& v) const {
        return (x * v.y - y * v.x);
    }

    static double signed_area(
        const R2Vector& a, const R2Vector& b
    ) {
        return a.signed_area(b);
    }
};

inline R2Vector operator*(double c, const R2Vector& v) {
    return R2Vector(c*v.x, c*v.y);
}

class R2Point {
public:
    double x;
    double y;

    R2Point():                         // Default constructor
        x(0.),
        y(0.)
    {}

    R2Point(const R2Point& p):        // Copy-constructor
        x(p.x),
        y(p.y)
    {}

    R2Point(double xx, double yy):
        x(xx),
        y(yy)
    {}

    R2Point& operator=(const R2Point& p) {    // Copy-operator
        x = p.x; y = p.y;
        return *this;
    }

    ~R2Point() {}                              // Destructor

    R2Point operator+(const R2Point& p) const {
        return R2Point(x+p.x, y+p.y);
    }

    R2Point operator+(const R2Vector& v) const {
        return R2Point(x+v.x, y+v.y);
    }

    R2Point& operator+=(const R2Point& p) {
        x += p.x;
        y += p.y;
        return *this;
    }

    R2Point& operator+=(const R2Vector& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    R2Vector operator-(const R2Point& p) const {
        return R2Vector(x-p.x, y-p.y);
    }

    R2Point operator-(const R2Vector& v) const {
        return R2Point(x-v.x, y-v.y);
    }

    R2Point& operator-=(const R2Vector& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    R2Point& operator-=(const R2Point& p) {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    R2Point operator*(double c) const {
        return R2Point(x*c, y*c);
    }

    R2Point& operator*=(double c) {
        x *= c;
        y *= c;
        return *this;
    }

    // Comparings
    bool operator==(const R2Point& p) const {
        //... return (x == p.x && y == p.y);
        return (
            fabs(x - p.x) <= R2GRAPH_EPSILON && 
            fabs(y - p.y) <= R2GRAPH_EPSILON
        );
    }
    bool operator!=(const R2Point& p) const { return !operator==(p); }
    bool operator>=(const R2Point& p) const {
        //... return (x > p.x || (x == p.x && y >= p.y));
        return (x > p.x || (x >= p.x && y >= p.y));
    }
    bool operator>(const R2Point& p) const {
        //... return (x > p.x || (x == p.x && y > p.y));
        return (x > p.x || (x >= p.x && y > p.y));
    }
    bool operator<(const R2Point& p) const { return !operator>=(p); }
    bool operator<=(const R2Point& p) const { return !operator>(p); }

    // Area of oriented triangle
    static double signed_area(
        const R2Point& a, const R2Point& b, const R2Point& c
    ) {
        return 0.5 * R2Vector::signed_area(b-a, c-a);
    }

    static double area(
        const R2Point& a, const R2Point& b, const R2Point& c
    ) {
        return fabs(signed_area(a, b, c));
    }

    bool between(const R2Point& a, const R2Point& b) const {
        R2Vector v(b - a);
        R2Vector m(*this - a);
        return (
            fabs(v.normal() * m) <= R2GRAPH_EPSILON && // point on line(a, b)
            m * v >= 0. &&  (*this - b) * v <= 0.      // between (a, b)
        );
    }

    static bool on_line(
        const R2Point& a, const R2Point& b, const R2Point& c
    ) {
        return (area(a, b, c) <= R2GRAPH_EPSILON);
    }

    // Angle from this point between points a and b (counterclockwise)
    double angle(const R2Point& a, const R2Point& b) const {
        return (a - *this).angle(b - *this);
    }

    // Angle with vertex A from AB to AC counterclockwise
    static double angle(
        const R2Point& A, const R2Point& B, const R2Point& C
    ) {
        return A.angle(B, C);
    }

    double distance(const R2Point& p) const {
        return (p - *this).length();
    }

    static double distance(const R2Point& a, const R2Point& b) {
        return a.distance(b);
    }
};


inline R2Point operator*(double c, const R2Point& p) {
    return R2Point(c*p.x, c*p.y);
}

class R2Rectangle {
    double l;   // left
    double b;   // bottom
    double w;   // width
    double h;   // height
public:
    R2Rectangle():
        l(0.),
        b(0.),
        w(0.),
        h(0.)
    {}

    R2Rectangle(double _left, double _bottom, double _width, double _height):
        l(_left),
        b(_bottom),
        w(_width),
        h(_height)
    {}

    R2Rectangle(const R2Point& _leftBottom, double _width, double _height):
        l(_leftBottom.x),
        b(_leftBottom.y),
        w(_width),
        h(_height)
    {}

    R2Rectangle(const R2Rectangle& r):
        l(r.l),
        b(r.b),
        w(r.w),
        h(r.h)
    {}

    ~R2Rectangle() {}

    R2Rectangle& operator=(const R2Rectangle& r) {
        l = r.l;
        b = r.b;
        w = r.w;
        h = r.h;
        return *this;
    }

    double left() const { return l; }
    double right() const { return l + w; }
    double bottom() const { return b; }
    double top() const { return b + h; }
    double width() const { return w; }
    double height() const { return h; }
    R2Point leftBottom() const { return R2Point(l, b); }
    R2Point rightTop() const { return R2Point(l + w, b + h); }

    void setLeft(double _left) { l = _left; }
    void setBottom(double _bottom) { b = _bottom; }
    void setWidth(double _width) { w = _width; }
    void setHeight(double _height) { h = _height; }

    double getXMin() const { return l; }
    double getXMax() const { return l + w; }
    double getYMin() const { return b; }
    double getYMax() const { return b + h; }

    bool contains(const R2Point& p) const {
        return (
            l <= p.x && p.x < l + w &&
            b <= p.y && p.y < b + h
        );
    }

    R2Rectangle& shift(const R2Vector& v) {
        l += v.x;
        b += v.y;
        return *this;
    }

    R2Rectangle& extend(const R2Vector& v) {
        w += v.x;
        h += v.y;
        return *this;
    }

    bool empty() const {
        return (w < 0. || h < 0.);
    }

    R2Rectangle& intersect(const R2Rectangle r) {
        double newLeft = left();
        if (r.left() > newLeft)
            newLeft = r.left();
        double newRight = right();
        if (r.right() < newRight)
            newRight = r.right();
        double newBottom = bottom();
        if (r.bottom() > newBottom)
            newBottom = r.bottom();
        double newTop = top();
        if (r.top() < newTop)
            newTop = r.top();
        l = newLeft;
        b = newBottom;
        w = newRight - newLeft;
        h = newTop - newBottom;
        return *this;
    }

    R2Rectangle& add(const R2Rectangle r) {
        double newLeft = left();
        if (r.left() < newLeft)
            newLeft = r.left();
        double newRight = right();
        if (r.right() > newRight)
            newRight = r.right();
        double newBottom = bottom();
        if (r.bottom() < newBottom)
            newBottom = r.bottom();
        double newTop = top();
        if (r.top() > newTop)
            newTop = r.top();
        l = newLeft;
        b = newBottom;
        w = newRight - newLeft;
        h = newTop - newBottom;
        return *this;
    }

    // Compute an intersection the rectangle and the line (p1, p2).
    // Result: the line (c1, c2).
    // Return value: true, if nonempty, false otherwise.
    bool clip(
        const R2Point& p1, const R2Point& p2,
        R2Point& c1, R2Point& c2
    ) const {
        c1 = p1; c2 = p2;
        double r;

        if (c1.x < getXMin() && c2.x < getXMin()) return 0;
        if (c1.x < getXMin()) {
            // Assert: c2.x >= getXMin()
            r = (c2.x - getXMin()) / (c2.x - c1.x);
            c1 = c2 + (c1-c2)*r;
        }
        if (c2.x < getXMin()) {
            r = (c1.x - getXMin()) / (c1.x - c2.x);
            c2 = c1 + (c2-c1)*r;
        }

        if (c1.x > getXMax() && c2.x > getXMax()) return 0;
        if (c1.x > getXMax()) {
            r = (getXMax() - c2.x) / (c1.x - c2.x);
            c1 = c2 + (c1-c2)*r;
        }
        if (c2.x > getXMax()) {
            r = (getXMax() - c1.x) / (c2.x - c1.x);
            c2 = c1 + (c2-c1)*r;
        }

        //-------------------

        if (c1.y < getYMin() && c2.y < getYMin()) return 0;
        if (c1.y < getYMin()) {
            // Assert: c2.y >= getYMin()
            r = (c2.y - getYMin()) / (c2.y - c1.y);
            c1 = c2 + (c1-c2)*r;
        }
        if (c2.y < getYMin()) {
            r = (c1.y - getYMin()) / (c1.y - c2.y);
            c2 = c1 + (c2-c1)*r;
        }

        if (c1.y > getYMax() && c2.y > getYMax()) return 0;
        if (c1.y > getYMax()) {
            r = (getYMax() - c2.y) / (c1.y - c2.y);
            c1 = c2 + (c1-c2)*r;
        }
        if (c2.y > getYMax()) {
            r = (getYMax() - c1.y) / (c2.y - c1.y);
            c2 = c1 + (c2-c1)*r;
        }

        return true;
    }
};

class I2Vector {
public:
    int x;
    int y;

    I2Vector():                         // Default constructor
        x(0),
        y(0)
    {}

    I2Vector(const I2Vector& v):        // Copy-constructor
        x(v.x),
        y(v.y)
    {}

    I2Vector(int xx, int yy):
        x(xx),
        y(yy)
    {}

    I2Vector& operator=(const I2Vector& v) {    // Copy-operator
        x = v.x; y = v.y;
        return *this;
    }

    ~I2Vector() {}                              // Destructor

    I2Vector operator+(const I2Vector& v) const {
        return I2Vector(x+v.x, y+v.y);
    }

    I2Vector& operator+=(const I2Vector& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    I2Vector operator-(const I2Vector& v) const {
        return I2Vector(x-v.x, y-v.y);
    }

    I2Vector& operator-=(const I2Vector& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    I2Vector operator*(int c) const {
        return I2Vector(x*c, y*c);
    }

    I2Vector& operator*=(int c) {
        x *= c;
        y *= c;
        return *this;
    }

    int operator*(const I2Vector& v) const { // Scalar product
        return x*v.x + y*v.y;
    }

    double length() const {
        return sqrt(x*x + y*y);
    }

    I2Vector normal() const {           // Normal to this vector
        return I2Vector(-y, x);
    }

    double angle(const I2Vector& v) const {  // Angle from this vector to v
        double xx = v * (*this);
        double yy = v * normal();
        return atan2(yy, xx);
    }

    // Comparings
    bool operator==(const I2Vector& v) const {
        return (x == v.x && y == v.y);
    }
    bool operator!=(const I2Vector& v) const { return !operator==(v); }
    bool operator>=(const I2Vector& v) const {
        return (x > v.x || (x == v.x && y >= v.y));
    }
    bool operator>(const I2Vector& v) const {
        return (x > v.x || (x == v.x && y > v.y));
    }
    bool operator<(const I2Vector& v) const { return !operator>=(v); }
    bool operator<=(const I2Vector& v) const { return !operator>(v); }
};

inline I2Vector operator*(int c, const I2Vector& v) {
    return I2Vector(c*v.x, c*v.y);
}

class I2Point {
public:
    int x;
    int y;

    I2Point():                         // Default constructor
        x(0),
        y(0)
    {}

    I2Point(const I2Point& p):        // Copy-constructor
        x(p.x),
        y(p.y)
    {}

    I2Point(int xx, int yy):
        x(xx),
        y(yy)
    {}

    I2Point& operator=(const I2Point& p) {    // Copy-operator
        x = p.x; y = p.y;
        return *this;
    }

    ~I2Point() {}                              // Destructor

    I2Point operator+(const I2Point& p) const {
        return I2Point(x+p.x, y+p.y);
    }

    I2Point operator+(const I2Vector& v) const {
        return I2Point(x+v.x, y+v.y);
    }

    I2Point& operator+=(const I2Point& p) {
        x += p.x;
        y += p.y;
        return *this;
    }

    I2Point& operator+=(const I2Vector& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    I2Vector operator-(const I2Point& p) const {
        return I2Vector(x-p.x, y-p.y);
    }

    I2Point operator-(const I2Vector& v) const {
        return I2Point(x-v.x, y-v.y);
    }

    I2Point& operator-=(const I2Vector& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    I2Point& operator-=(const I2Point& p) {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    I2Point operator*(int c) const {
        return I2Point(x*c, y*c);
    }

    I2Point& operator*=(int c) {
        x *= c;
        y *= c;
        return *this;
    }

    // Comparings
    bool operator==(const I2Point& p) const {
        return (x == p.x && y == p.y);
    }
    bool operator!=(const I2Point& p) const { return !operator==(p); }
    bool operator>=(const I2Point& p) const {
        return (x > p.x || (x == p.x && y >= p.y));
    }
    bool operator>(const I2Point& p) const {
        return (x > p.x || (x == p.x && y > p.y));
    }
    bool operator<(const I2Point& p) const { return !operator>=(p); }
    bool operator<=(const I2Point& p) const { return !operator>(p); }
};


inline I2Point operator*(int c, const I2Point& p) {
    return I2Point(c*p.x, c*p.y);
}

class I2Rectangle {
    // Y-coordinate axis in case of I2Rectangle goes down, so that
    // y-coordinate of top is less than y-coordinate of bottom
    int l;      // left
    int t;      // top
    int w;      // width
    int h;      // height

public:
    I2Rectangle():
        l(0),
        t(0),
        w(0),
        h(0)
    {}

    I2Rectangle(int _left, int _top, int _width, int _height):
        l(_left),
        t(_top),
        w(_width),
        h(_height)
    {}

    I2Rectangle(const I2Point& _leftTop, int _width, int _height):
        l(_leftTop.x),
        t(_leftTop.y),
        w(_width),
        h(_height)
    {}

    I2Rectangle(const I2Rectangle& r):
        l(r.l),
        t(r.t),
        w(r.w),
        h(r.h)
    {}

    ~I2Rectangle() {}

    I2Rectangle& operator=(const I2Rectangle& r) {
        l = r.l;
        t = r.t;
        w = r.w;
        h = r.h;
        return *this;
    }

    int left() const { return l; }
    int right() const { return l + w; }
    int top() const { return t; }
    int bottom() const { return t + h; }
    int width() const { return w; }
    int height() const { return h; }
    I2Point leftTop() const { return I2Point(l, t); }
    I2Point rightBottom() const { return I2Point(l + w, t + h); }

    void setLeft(int _left) { l = _left; }
    void setTop(int _top) { t = _top; }
    void setWidth(int _width) { w = _width; }
    void setHeight(int _height) { h = _height; }

    double getXMin() const { return l; }
    double getXMax() const { return l + w; }
    double getYMin() const { return t; }
    double getYMax() const { return t + h; }

    bool contains(const I2Point& p) const {
        return (
            l <= p.x && p.x < l + w &&
            t <= p.y && p.y < t + h
        );
    }

    I2Rectangle& shift(const I2Vector& v) {
        l += v.x;
        t += v.y;
        return *this;
    }

    I2Rectangle& extend(const I2Vector& v) {
        w += v.x;
        h += v.y;
        return *this;
    }

    bool empty() const {
        return (w < 0. || h < 0.);
    }

    I2Rectangle& intersect(const I2Rectangle r) {
        int newLeft = left();
        if (r.left() > newLeft)
            newLeft = r.left();
        int newRight = right();
        if (r.right() < newRight)
            newRight = r.right();
        int newTop = top();
        if (r.top() > newTop)
            newTop = r.top();
        int newBottom = bottom();
        if (r.bottom() < newBottom)
            newBottom = r.bottom();
        l = newLeft;
        t = newTop;
        w = newRight - newLeft;
        h = newBottom - newTop;
        return *this;
    }

    I2Rectangle& add(const I2Rectangle r) {
        int newLeft = left();
        if (r.left() < newLeft)
            newLeft = r.left();
        int newRight = right();
        if (r.right() > newRight)
            newRight = r.right();
        int newTop = top();
        if (r.top() < newTop)
            newTop = r.top();
        int newBottom = bottom();
        if (r.bottom() > newBottom)
            newBottom = r.bottom();
        l = newLeft;
        t = newTop;
        w = newRight - newLeft;
        h = newBottom - newTop;
        return *this;
    }
};

// Global functions
bool intersectLineSegments(
    const R2Point& p0, const R2Point& p1,   // First line segment
    const R2Point& q0, const R2Point& q1,   // Second line segmant
    R2Point& intersection                   // Result
);

bool intersectLineSegmentAndLine(
    const R2Point& p0, const R2Point& p1,   // Line segment
    const R2Point& q, const R2Vector& v,    // Straight line
    R2Point& intersection                   // Result
);

inline bool intersectLineSegmentAndLine(
    const R2Point& p0, const R2Point& p1,   // Line segment
    const R2Point& q0, const R2Point& q1,   // Straight linedefined by 2 points
    R2Point& intersection                   // Result
) {
    return intersectLineSegmentAndLine(
        p0, p1,
        q0, (q1 - q0),
        intersection
    );
}

bool intersectStraightLines(
    const R2Point& p, const R2Vector& v,    // First line
    const R2Point& q, const R2Vector& w,    // Second line
    R2Point& intersection                   // Result
);

inline bool intersectStraightLines(
    const R2Point& p0, const R2Point& p1,   // First line
    const R2Point& q0, const R2Point& q1,   // Second line
    R2Point& intersection                   // Result
) {
    return intersectStraightLines(
        p0, (p1 - p0),
        q0, (q1 - q0),
        intersection
    );
}

#endif
