#include <math.h>
#include <iostream>
#include "R2Graph.h"

using namespace std;

int main() {
    cout << "Test of R2Graph package\n";
    R2Vector u, v;
    R2Point p0, p1, q0, q1;
    R2Point intersection;
    while (true) {
        cout << "Input 2 vectors:\n";
        cin >> u.x >> u.y >> v.x >> v.y;
        if (u == R2Vector(0., 0.) || v == R2Vector(0., 0.))
            break;
        double a = u.angle(v);
        cout << "angle from first to secong = " << a
            << " (" << (int) (floor(a*180./3.14158) + 0.5) 
            << " degrees)" << endl;

        cout << "Input 2 line segments:\n";
        cin >> p0.x >> p0.y >> p1.x >> p1.y;
        if (p0 == p1)
            break;
        cin >> q0.x >> q0.y >> q1.x >> q1.y;
        if (q0 == q1)
            break;
        if (intersectLineSegments(p0, p1, q0, q1, intersection)) {
            cout << "Intersection: (" <<
                intersection.x << ", " << intersection.y << ")\n";
        } else {
            cout << "Line segments do not intersect.\n";
        }
    }
    return 0;
}
