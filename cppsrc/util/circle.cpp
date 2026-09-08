#include "circle.hpp"
#include "../geometry.hpp"

Circle circleFromOne(const GeoPoint* p) {
    return Circle(p->getX(), p->getY(), 0);
}

Circle circleFromTwo(const GeoPoint* a, const GeoPoint* b) {
    double cx = (a->getX() + b->getX()) / 2.0;
    double cy = (a->getY() + b->getY()) / 2.0;
    double dx = a->getX() - b->getX();
    double dy = a->getY() - b->getY();
    return Circle(cx, cy, std::sqrt(dx*dx + dy*dy) / 2.0);
}

Circle circleFromThree(const GeoPoint* a, const GeoPoint* b, const GeoPoint* c) {
    double ax = a->getX(), ay = a->getY();
    double bx = b->getX(), by = b->getY();
    double cx = c->getX(), cy = c->getY();

    double d = 2 * (ax*(by-cy) + bx*(cy-ay) + cx*(ay-by));
    if (std::abs(d) < 1e-10)
        return circleFromTwo(a, c); // collinear fallback

    double ux = ((ax*ax + ay*ay)*(by-cy) + (bx*bx + by*by)*(cy-ay) + (cx*cx + cy*cy)*(ay-by)) / d;
    double uy = ((ax*ax + ay*ay)*(cx-bx) + (bx*bx + by*by)*(ax-cx) + (cx*cx + cy*cy)*(bx-ax)) / d;

    double dx = ux - ax;
    double dy = uy - ay;
    return Circle(ux, uy, std::sqrt(dx*dx + dy*dy));
}

Circle welzlHelper(std::vector<const GeoPoint*>& P, std::vector<const GeoPoint*> R, int n) {
    if (n == 0 || R.size() == 3) {
        if (R.size() == 0) return Circle();
        if (R.size() == 1) return circleFromOne(R[0]);
        if (R.size() == 2) return circleFromTwo(R[0], R[1]);
        return circleFromThree(R[0], R[1], R[2]);
    }

    int idx = rand() % n;
    const GeoPoint* p = P[idx];
    std::swap(P[idx], P[n-1]);

    Circle d = welzlHelper(P, R, n-1);

    if (d.contains(p->getX(), p->getY())) return d;

    R.push_back(p);
    return welzlHelper(P, R, n-1);
}

Circle calcMinimumBoundingCircle(std::vector<const GeoPoint*> points) {
    std::shuffle(points.begin(), points.end(), std::default_random_engine(42));
    return welzlHelper(points, {}, points.size());
}
