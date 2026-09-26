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
        /** @brief The point's Cartesian coordinates. */
        double x, y;
    public:
        /** @brief Construct a point at the origin (0, 0). */
        GeoPoint() : x(0), y(0) {}
        /**
         * @brief Construct a point from explicit coordinates.
         * @param x The x-coordinate.
         * @param y The y-coordinate.
         */
        GeoPoint(double x, double y) : x(x), y(y) {}
        /**
         * @brief Get the point's x-coordinate.
         * @return The x-coordinate.
         */
        double getX() const;
        /**
         * @brief Get the point's y-coordinate.
         * @return The y-coordinate.
         */
        double getY() const;
        /**
         * @brief Exact equality comparison of coordinates.
         * @param other The point to compare against.
         * @return True if both x and y coordinates match exactly.
         */
        bool operator==(const GeoPoint& other) const;
        /**
         * @brief Strict weak ordering over points, primarily by x then y.
         *
         * Used to allow GeoPoint to serve as a key in ordered containers
         * (e.g. pointRegister).
         *
         * @param other The point to compare against.
         * @return True if this point sorts before other.
         */
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
        /** Pointers into pointRegister for this line's two endpoints. */
        const GeoPoint *p1, *p2;
        /** Precomputed midpoint of the line, used as its registry key. */
        GeoPoint midpoint;
        /** Precomputed Euclidean length of the line. */
        double length;
        /** Geometry objects that currently include this line as a boundary edge. */
        std::set<Geometry*> owners;
    public:
        /** @brief Construct an empty/default line with no endpoints (p1/p2 left uninitialized). */
        GeoLine(){}
        /**
         * @brief Construct a line from two endpoint coordinates.
         *
         * Registers (or reuses) both endpoints in pointRegister, and
         * precomputes the line's midpoint and length.
         *
         * @param x1 X-coordinate of the first endpoint.
         * @param x2 X-coordinate of the second endpoint.
         * @param y1 Y-coordinate of the first endpoint.
         * @param y2 Y-coordinate of the second endpoint.
         */
        GeoLine(double x1, double x2, double y1, double y2);
        /**
         * @brief Get this line's precomputed midpoint.
         * @return A const reference to the midpoint.
         */
        const GeoPoint& getMidpoint() const;
        /**
         * @brief Get this line's precomputed Euclidean length.
         * @return The length.
         */
        double getLength();
        /**
         * @brief Record a Geometry as currently claiming this line as a boundary edge.
         * @param ug The owning Geometry to add.
         */
        void addOwner(Geometry* ug);
        /**
         * @brief Remove a Geometry from the set of owners currently claiming this line as a boundary edge.
         * @param ug The owning Geometry to remove.
         */
        void removeOwner(Geometry* ug);
        /**
         * @brief Get the Geometry objects that currently claim this line as a boundary edge.
         * @return A const reference to the set of owners.
         */
        const std::set<Geometry*>& getOwners() const;
        /**
         * @brief Get this line's first endpoint.
         * @return Pointer to the first endpoint, registered in pointRegister.
         */
        const GeoPoint* getP1() const { return p1; }
        /**
         * @brief Get this line's second endpoint.
         * @return Pointer to the second endpoint, registered in pointRegister.
         */
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
        /** @brief The ElectoralEntity (Precinct/District/State) that this Geometry represents the boundary of. */
        ElectoralEntity *owner;           
        /** @brief The set of GeoLine boundary edges currently making up this geometry. */
        std::set<GeoLine*> lines;
        /** @brief Whether the derived quantities (perimeter, area, centroid, contiguity, compactness, etc.) are up to date with the current line set. */
        bool cached; //has this Geometry been changed since the last time the centroid or perimeter has been calculated
        /** @brief Cached result of the most recent contiguity check: whether the boundary forms a single closed loop touching every line exactly once. */
        bool contiguous;
        /** @brief Cached total perimeter length, summed over all boundary lines. */
        double perimeter;
        /** @brief Cached polygon area, computed via the shoelace formula over the ordered boundary vertices; -1 if not yet computed. */
        double area;
        /** @brief Cached Polsby-Popper compactness score (4*pi*area / perimeter^2). */
        double polsbyPopper;
        /** @brief Cached Reock compactness score (area / minimum bounding circle area). */
        double reock;
        /** @brief Cached perimeter-length-weighted centroid of the boundary. */
        GeoPoint centroid;
        /**
         * @brief Recompute all cached derived quantities (contiguity, perimeter, centroid, area, minimum bounding circle, and compactness scores) if the line set has changed since the last computation.
         *
         * Walks the boundary lines via endpointMap starting from an arbitrary line to determine contiguity and vertex order; if the walk does not visit every line exactly once, the geometry is marked non-contiguous and area/compactness are left uncomputed for this pass.
         */
        void updateCached();
        /** @brief Cached smallest circle enclosing the ordered boundary vertices. */
        Circle minimumBoundingCircle;
        /** @brief Cached ordered list of boundary vertices, produced by the contiguity walk in updateCached(). */
        std::vector<const GeoPoint*> vertices;
        /**
         * @brief Get the minimum bounding circle of this geometry, recomputing cached quantities first if needed.
         * @return The minimum bounding circle.
         */
        Circle getMinimumBoundingCircle();
        /**
         * @brief Get the ordered boundary vertices of this geometry, recomputing cached quantities first if needed.
         * @return The ordered vertices.
         */
        std::vector<const GeoPoint*> getOrderedVertices();
    
    public:
        /**
         * @brief Construct an empty Geometry owned by the given entity.
         * @param owner The ElectoralEntity this geometry represents the boundary of.
         */
        Geometry(ElectoralEntity *owner);
        /**
         * @brief Populate this geometry's boundary lines from a GeoJSON "Polygon" or "MultiPolygon" geometry object.
         * @param json The GeoJSON geometry object, with a "type" of "Polygon" or "MultiPolygon" and matching "coordinates".
         */
        void loadGeometry(const JsonValue& json);
        /**
         * @brief Add a boundary line segment to this geometry, registering (or reusing) it in the global line registry and marking this geometry as its owner.
         * @param x1 X-coordinate of the first endpoint.
         * @param x2 X-coordinate of the second endpoint.
         * @param y1 Y-coordinate of the first endpoint.
         * @param y2 Y-coordinate of the second endpoint.
         * @return A reference to the registered GeoLine.
         */
        GeoLine& addLine(double x1, double x2, double y1, double y2);
        /**
         * @brief Get this geometry's total perimeter length, recomputing cached quantities first if needed.
         * @return The perimeter.
         */
        double getPerimeter();
        /**
         * @brief Get this geometry's centroid, recomputing cached quantities first if needed.
         * @return A reference to the centroid.
         */
        GeoPoint& getCentroid();
        /**
         * @brief Get this geometry's area, recomputing cached quantities first if needed.
         * @return The area.
         */
        double getArea();
        /**
         * @brief Determine whether this geometry's boundary is a single contiguous loop with no holes, recomputing cached quantities first if needed.
         * @return True if contiguous.
         */
        bool isContiguous();
        /**
         * @brief Get the boundary lines currently making up this geometry.
         * @return A copy of the set of GeoLine pointers.
         */
        const std::set<GeoLine*> getLines() const { return lines; }
        /**
         * @brief Get the ElectoralEntity that owns this geometry.
         * @return Pointer to the owning entity.
         */
        ElectoralEntity *getOwner() { return owner; }
        /**
         * @brief Merge another geometry's boundary lines into this one: shared edges (owned by both) are removed from both (since they are now interior), while unshared edges are added.
         * @param other The geometry to merge in.
         */
        void mergeGeometry(Geometry& other);
        /**
         * @brief Get this geometry's Polsby-Popper compactness score, recomputing cached quantities first if needed.
         * @return The Polsby-Popper score.
         */
        double getPolsbyPopper();
        /**
         * @brief Get this geometry's Reock compactness score, recomputing cached quantities first if needed.
         * @return The Reock score.
         */
        double getReock();
        /**
         * @brief Compute the Euclidean distance between this geometry's centroid and another's.
         * @param other The geometry to measure distance to.
         * @return The distance between centroids.
         */
        double distance (Geometry *other);
};