#include <stdio.h>
#include "R2Graph.h"

int main() {
    R2Point p1, p2;
    R2Vector v1, v2;
    printf("Input line 1:\n");
    scanf("%lf%lf%lf%lf", &p1.x, &p1.y, &v1.x, &v1.y);
    printf("Input line 2:\n");
    scanf("%lf%lf%lf%lf", &p2.x, &p2.y, &v2.x, &v2.y);
    R2Vector n2 = v2.normal();
    double t = n2*(p2 - p1) / (n2*v1);
    R2Point q = p1 + v1*t;
    printf("Intersection: (%f, %f)\n", q.x, q.y);
    return 0;
}
