#include "geometry.hpp"

std::unordered_map<GeoPoint, GeoLine> lineRegister;
std::unordered_map<GeoPoint, std::set<GeoLine*>> endpointMap;
std::set<GeoPoint> pointRegister;

GeoLine::GeoLine(double x1, double x2, double y1, double y2) {
    auto [it1, _1] = pointRegister.emplace(x1, y1);
    p1 = &(*it1);
    auto [it2, _2] = pointRegister.emplace(x2, y2);
    p2 = &(*it2);
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

void GeoLine::removeOwner(UntypedGeometry* ug) {
    owners.erase(ug);
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

bool GeoPoint::operator<(const GeoPoint& other) const {
    return x == other.x ? y < other.y : x < other.x;
}