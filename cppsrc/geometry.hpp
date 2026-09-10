#pragma once

#include <set>
#include <stack>
#include <cmath>
#include <numbers>
#include <unordered_map>
#include <string>
#include <iostream>
#include <iomanip>

#include "util/json.hpp"
#include "util/circle.hpp"
#include "util/ioUtil.hpp"

class GeoPoint;
class GeoLine;
class Geometry;

class ElectoralEntity;

class GeoPoint {
    private:
        double x, y;
    public:
        GeoPoint() : x(0), y(0) {}
        GeoPoint(double x, double y) : x(x), y(y) {}
        double getX() const;
        double getY() const;
        bool operator==(const GeoPoint& other) const;
        bool operator<(const GeoPoint& other) const;
};


class GeoLine {
    private:
        const GeoPoint *p1, *p2;
        GeoPoint midpoint;
        double length;
        std::set<Geometry*> owners;
    public:
        GeoLine(){}
        GeoLine(double x1, double x2, double y1, double y2);
        const GeoPoint& getMidpoint() const;
        double getLength();
        void addOwner(Geometry* ug);
        void removeOwner(Geometry* ug);
        const std::set<Geometry*>& getOwners() const;
        const GeoPoint* getP1() const { return p1; }
        const GeoPoint* getP2() const { return p2; }
};

template <>
struct std::hash<GeoPoint> {
    std::size_t operator()(const GeoPoint& p) const noexcept {
        std::size_t hx = std::hash<double>{}(p.getX());
        std::size_t hy = std::hash<double>{}(p.getY());
        // boost-style combine, avoids hx==hy collapsing to 0
        return hx ^ (hy + 0x9e3779b97f4a7c15ULL + (hx << 6) + (hx >> 2));
    }
};

//should be private to geometry code
extern std::unordered_map<GeoPoint, GeoLine> lineRegister;
extern std::unordered_map<GeoPoint, std::set<GeoLine*>> endpointMap;
extern std::set<GeoPoint> pointRegister;

class Geometry {
    private:
        ElectoralEntity *owner;           
        std::set<GeoLine*> lines;
        bool cached; //has this Geometry been changed since the last time the centroid or perimeter has been calculated
        bool contiguous;
        double perimeter;
        double area;
        double polsbyPopper;
        double reock;
        GeoPoint centroid;
        void updateCached();
        Circle minimumBoundingCircle;
        std::vector<const GeoPoint*> vertices;
        Circle getMinimumBoundingCircle();
        std::vector<const GeoPoint*> getOrderedVertices();
    
    public:
        Geometry(ElectoralEntity *owner);
        void loadGeometry(const JsonValue& json);
        GeoLine& addLine(double x1, double x2, double y1, double y2);
        double getPerimeter();
        GeoPoint& getCentroid();
        double getArea();
        bool isContiguous();
        const std::set<GeoLine*> getLines() const { return lines; }
        ElectoralEntity *getOwner() { return owner; }
        void mergeGeometry(Geometry& other);
        double getPolsbyPopper();
        double getReock();
        double distance (Geometry *other);
};
