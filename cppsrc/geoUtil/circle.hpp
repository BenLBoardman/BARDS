#pragma once
#include <cmath>
#include <numbers>
#include <vector>
#include <algorithm>
#include <random>

class GeoPoint;

class Circle {
    private:
        double cx, cy, radius;
    
    public:
        Circle() : cx(0), cy(0), radius(0) {}
        Circle(double cx, double cy, double radius) : cx(cx), cy(cy), radius(radius) {}
        
        double getCx() const { return cx; }
        double getCy() const { return cy; }
        double getRadius() const { return radius; }
        double getArea() const { return std::numbers::pi * radius * radius; }
        
        bool contains(double x, double y) const {
            double dx = x - cx;
            double dy = y - cy;
            return dx*dx + dy*dy <= radius*radius + 1e-10;
        }
};

Circle circleFromOne(const GeoPoint* p);
Circle circleFromTwo(const GeoPoint* a, const GeoPoint* b);
Circle circleFromThree(const GeoPoint* a, const GeoPoint* b, const GeoPoint* c);
Circle welzlHelper(std::vector<const GeoPoint*>& P, std::vector<const GeoPoint*> R, int n);
Circle calcMinimumBoundingCircle(std::vector<const GeoPoint*> points);
