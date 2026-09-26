/**
 * @file circle.hpp
 * @brief Circle primitive and minimum bounding circle computation
 *        (Welzl's algorithm) used for Reock compactness scoring.
 */
#pragma once
#include <cmath>
#include <numbers>
#include <vector>
#include <algorithm>
#include <random>

class GeoPoint;

/**
 * @class Circle
 * @brief An immutable 2D circle defined by a center point and radius.
 */
class Circle {
    private:
        /** @brief The circle's center coordinates (cx, cy) and its radius. */
        double cx, cy, radius;
    
    public:
        /** @brief Construct a degenerate circle at the origin with zero radius. */
        Circle() : cx(0), cy(0), radius(0) {}
        /**
         * @brief Construct a circle from an explicit center and radius.
         * @param cx The center x-coordinate.
         * @param cy The center y-coordinate.
         * @param radius The radius.
         */
        Circle(double cx, double cy, double radius) : cx(cx), cy(cy), radius(radius) {}
        
        /**
         * @brief Get the center's x-coordinate.
         * @return The center x-coordinate.
         */
        double getCx() const { return cx; }
        /**
         * @brief Get the center's y-coordinate.
         * @return The center y-coordinate.
         */
        double getCy() const { return cy; }
        /**
         * @brief Get the circle's radius.
         * @return The radius.
         */
        double getRadius() const { return radius; }
        /**
         * @brief Compute the circle's area.
         * @return pi * radius^2.
         */
        double getArea() const { return std::numbers::pi * radius * radius; }
        
        /**
         * @brief Determine whether a point lies within (or on) the circle, with a small tolerance for floating-point error.
         * @param x The point's x-coordinate.
         * @param y The point's y-coordinate.
         * @return True if the point is within radius (plus tolerance) of the center.
         */
        bool contains(double x, double y) const {
            double dx = x - cx;
            double dy = y - cy;
            return dx*dx + dy*dy <= radius*radius + 1e-10;
        }
};

/**
 * @brief Construct the trivial bounding circle for a single point: zero radius, centered on the point.
 * @param p The point.
 * @return A zero-radius circle centered at p.
 */
Circle circleFromOne(const GeoPoint* p);
/**
 * @brief Construct the minimum bounding circle for two points, centered at their midpoint with a radius of half their distance apart.
 * @param a The first point.
 * @param b The second point.
 * @return The circle having a and b as diametrically opposite points.
 */
Circle circleFromTwo(const GeoPoint* a, const GeoPoint* b);
/**
 * @brief Construct the circumscribed circle passing through three points.
 * @param a The first point.
 * @param b The second point.
 * @param c The third point.
 * @return The circle through a, b, and c; falls back to circleFromTwo(a, c) if the points are (nearly) collinear.
 */
Circle circleFromThree(const GeoPoint* a, const GeoPoint* b, const GeoPoint* c);
/**
 * @brief Recursive helper implementing Welzl's randomized minimum enclosing circle algorithm.
 * @param P The candidate points, considered up to index n; may be reordered in place as points are swapped to the back.
 * @param R The set of up to 3 points currently known to lie on the boundary of the minimum enclosing circle.
 * @param n The number of points in P still to be considered.
 * @return The minimum enclosing circle for the first n points of P together with R.
 */
Circle welzlHelper(std::vector<const GeoPoint*>& P, std::vector<const GeoPoint*> R, int n);
/**
 * @brief Compute the minimum bounding circle enclosing a set of points, using a fixed-seed shuffled point order to drive Welzl's algorithm.
 * @param points The points to enclose.
 * @return The minimum bounding circle.
 */
Circle calcMinimumBoundingCircle(std::vector<const GeoPoint*> points);