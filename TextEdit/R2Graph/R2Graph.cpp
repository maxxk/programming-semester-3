#include <assert.h>
#include "R2Graph.h"

bool intersectLineSegments(
    const R2Point& p0, const R2Point& p1,   // First line segment
    const R2Point& q0, const R2Point& q1,   // Second line segmant
    R2Point& intersection                   // Result
) {
    R2Point inter;
    if (!intersectStraightLines(
        p0, p1,
        q0, q1,
        inter
    ))
        return false;

    R2Vector n = (p1 - p0).normal();
    R2Vector m = (q1 - q0).normal();
    if (
        ((p0 - q0)*m) * ((p1 - q0)*m) <= 0. &&
        ((q0 - p0)*n) * ((q1 - p0)*n) <= 0.
    ) {
        intersection = inter;
        return true;
    } else {
        return false;
    }
}

bool intersectLineSegmentAndLine(
    const R2Point& p0, const R2Point& p1,   // Line segment
    const R2Point& q, const R2Vector& v,    // Straight line
    R2Point& intersection                   // Result
) {
    R2Point inter;
    if (!intersectStraightLines(
        p0, (p1 - p0),
        q, v,
        inter
    ))
        return false;

    R2Vector n = v.normal();
    if (
        ((p0 - q)*n) * ((p1 - q)*n) <= 0.
    ) {
        intersection = inter;
        return true;
    } else {
        return false;
    }
}

bool intersectStraightLines(
    const R2Point& p, const R2Vector& v,    // First line
    const R2Point& q, const R2Vector& w,    // Second line
    R2Point& intersection                   // Result
) {
    assert(
        v != R2Vector(0., 0.) &&
        w != R2Vector(0., 0.)
    );
    if (
        v == R2Vector(0., 0.) ||
        w == R2Vector(0., 0.)
    )
        return false;
    R2Vector n = v.normal();
    // Point s on the second line:
    //     s = q + w*t
    // Intersection: scalar product equals zero
    // (s - p, n) == 0
    // (q + w*t - p, n) = 0
    // t = (p - q, n) / (w, n)
    double wn = w*n;
    double qpn = (p - q)*n;

    if (fabs(wn) <= R2GRAPH_EPSILON) {
        // Parallel lines
        if (fabs(qpn) <= R2GRAPH_EPSILON) {
            // Equal lines
            intersection = p;
            return true;
        }
        return false;
    }
    double t = qpn / wn;
    intersection = q + w*t;
    return true;
}
