#include "geometry.hpp"

std::unordered_map<GeoPoint, GeoLine> lineRegister;

GeoLine::GeoLine(double x1, double x2, double y1, double y2) {
    p1 = GeoPoint(x1, y1);
    p2 = GeoPoint(x2, y2);
    midpoint = GeoPoint((x2+x1)/2, (y2+y1)/2);
    length = std::sqrt(pow((x2-x1),2)+pow((y2-y1),2));
}

const GeoPoint& GeoLine::getMidpoint() const {
    return midpoint;
}

double GeoLine::getLength() {
    return length;
}

void GeoLine::addOwner(UntypedGeometry *ug) {
    owners.emplace(ug);
}

const std::set<UntypedGeometry*>& GeoLine::getOwners() const {
    return owners;
}


double GeoPoint::getX() const {
    return x;
}

double GeoPoint::getY() const {
    return y;
}

bool GeoPoint::operator==(const GeoPoint& other) const {
    return x == other.x && y == other.y;
}