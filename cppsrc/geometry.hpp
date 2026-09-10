/**
 * @file geometry.hpp
 * @brief Core geometric primitives (points, lines, and composite geometries)
 *        used to represent electoral district boundaries.
 *
 * Provides GeoPoint and GeoLine as deduplicated, registry-backed primitives
 * (see lineRegister, endpointMap, and pointRegister), and Geometry as a
 * composite polygon/multipolygon built from shared GeoLine instances. Geometry
 * exposes cached derived quantities (perimeter, area, centroid, contiguity,
 * and compactness scores such as Polsby-Popper and Reock) that are lazily
 * recomputed via updateCached() when the underlying line set changes.
 */
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

/**
 * @copydoc ElectoralEntity
 */
class ElectoralEntity;

/**
 * @class GeoPoint
 * @brief An immutable 2D Cartesian point (x, y).
 *
 * GeoPoint instances are deduplicated via pointRegister: distinct GeoLine
 * endpoints that share the same coordinates resolve to the same GeoPoint
 * instance, which allows adjacency between lines to be detected by pointer
 * equality via endpointMap.
 */
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

/**
 * @class GeoLine
 * @brief A line segment between two registered GeoPoint endpoints.
 *
 * GeoLine instances are stored by value in lineRegister, keyed by midpoint,
 * and referenced elsewhere by pointer. Each GeoLine tracks the set of
 * Geometry objects that currently claim it as a boundary edge (owners),
 * which supports shared-edge detection when merging or splitting districts.
 */
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

/**
 * @brief Hash specialization enabling GeoPoint to be used as an
 *        unordered_map/unordered_set key.
 *
 * Combines the hashes of the x and y coordinates using a boost-style
 * combine function, which avoids collapsing to zero when hx == hy.
 */
template <>
struct std::hash<GeoPoint> {
    std::size_t operator()(const GeoPoint& p) const noexcept {
        std::size_t hx = std::hash<double>{}(p.getX());
        std::size_t hy = std::hash<double>{}(p.getY());
        // boost-style combine, avoids hx==hy collapsing to 0
        return hx ^ (hy + 0x9e3779b97f4a7c15ULL + (hx << 6) + (hx >> 2));
    }
};

/**
 * @brief Global registry mapping each unique line midpoint to its GeoLine.
 *
 * Used to deduplicate line segments so that boundary edges shared between
 * adjacent Geometry instances are represented by a single GeoLine object.
 * Should be treated as private to geometry code.
 */
extern std::unordered_map<GeoPoint, GeoLine> lineRegister;

/**
 * @brief Global registry mapping each GeoPoint to the set of GeoLine
 *        pointers that touch it as an endpoint.
 *
 * Used during contiguity traversal to find the next connected line when
 * walking a Geometry's boundary. Should be treated as private to geometry
 * code.
 */
extern std::unordered_map<GeoPoint, std::set<GeoLine*>> endpointMap;

/**
 * @brief Global registry of all unique GeoPoint coordinates in use.
 *
 * Ensures that GeoLine endpoints sharing the same coordinates resolve to
 * a single canonical GeoPoint instance. Should be treated as private to
 * geometry code.
 */
extern std::set<GeoPoint> pointRegister;


/**
 * @class Geometry
 * @brief A composite polygon or multipolygon built from shared GeoLine edges.
 *
 * A Geometry owns a set of GeoLine pointers representing its boundary. It
 * lazily computes and caches derived properties -- perimeter, area,
 * centroid, contiguity, minimum bounding circle, and compactness scores
 * (Polsby-Popper and Reock) -- via updateCached(), which is invoked whenever
 * one of those properties is requested after the geometry has changed
 * (tracked by the cached flag).
 */
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
