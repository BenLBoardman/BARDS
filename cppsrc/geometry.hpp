#pragma once

#include <set>
#include <stack>
#include <cmath>
#include <numbers>
#include <unordered_map>
#include <string>
#include <iostream>
#include <iomanip>

#include "json.hpp"
#include "circle.hpp"

class UntypedGeometry {
    public:
        virtual ~UntypedGeometry() = default;
};

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
        std::set<UntypedGeometry*> owners;
    public:
        GeoLine(){}
        GeoLine(double x1, double x2, double y1, double y2);
        const GeoPoint& getMidpoint() const;
        double getLength();
        void addOwner(UntypedGeometry* ug);
        void removeOwner(UntypedGeometry* ug);
        const std::set<UntypedGeometry*>& getOwners() const;
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

template <typename T>
class Geometry : public UntypedGeometry {
    private:
        T& owner;           
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
        Geometry(T& owner);
        void loadGeometry(const JsonValue& json);
        GeoLine& addLine(double x1, double x2, double y1, double y2);
        double getPerimeter();
        GeoPoint& getCentroid();
        double getArea();
        bool isContiguous();
        const std::set<GeoLine*> getLines() const { return lines; }
        T& getOwner() { return owner; }
        template <typename K>
        void mergeGeometry(Geometry<K>& other);
        double getPolsbyPopper();
        double getReock();
};

template <typename T>
Geometry<T>::Geometry(T& owner) : owner(owner) {
    cached = false; 
    
}

template <typename T>
void Geometry<T>::loadGeometry(const JsonValue& json) {
    if(json["type"].asString() == "Polygon") {
        const JsonArray ringArray = json["coordinates"].asArray();
        for(int i = 0; i < ringArray.size(); i++) {
            const JsonArray coordArray = ringArray[i].asArray();
            for(int j = 0; j < coordArray.size() - 1; j++) {
                addLine(coordArray[j][0].asNumber(), coordArray[j+1][0].asNumber(), coordArray[j][1].asNumber(), coordArray[j+1][1].asNumber());
            }
        }
    }
    else if(json["type"].asString() == "MultiPolygon") {
        const JsonArray multiArray = json["coordinates"].asArray();
        for(int k = 0; k < multiArray.size(); k++) {
            const JsonArray polyArray = multiArray[k].asArray();
            for(int i = 0; i < polyArray.size(); i++) {
                const JsonArray coordArray = polyArray[i].asArray();
                for(int j = 0; j < coordArray.size()-1; j++) {
                    addLine(coordArray[j][0].asNumber(), coordArray[j+1][0].asNumber(), coordArray[j][1].asNumber(), coordArray[j+1][1].asNumber());
                }
            }
        }
        //std::cout << "Non-contiguous geometry parsing found " << multiArray.size() << " independent sub-geometries." << std::endl;
    }
    else {
        std::cout << "Unrecognized geometry type!" << std::endl;
        return;
    }
    updateCached();
}

template <typename T>
GeoLine& Geometry<T>::addLine(double x1, double x2, double y1, double y2) {
    GeoLine candidate(x1, x2, y1, y2);
    auto [it, inserted] = lineRegister.emplace(candidate.getMidpoint(), candidate);
    GeoLine& line = it->second;
    endpointMap[*line.getP1()].insert(&line);
    endpointMap[*line.getP2()].insert(&line);
    line.addOwner(this);
    lines.insert(&line);
    cached = false;
    return line;
}

template <typename T>
GeoPoint& Geometry<T>::getCentroid() {
    updateCached();
    return centroid;
}

template <typename T>
double Geometry<T>::getPerimeter() {
    updateCached();
    return perimeter;
}

template <typename T>
double Geometry<T>::getArea() {
    updateCached();
    return area;
}

template <typename T>
bool Geometry<T>::isContiguous() {
    updateCached();
    return contiguous;
}


template <typename T>
Circle Geometry<T>::getMinimumBoundingCircle() {
    updateCached();
    return minimumBoundingCircle;
}

/**
 * Update Geometry perimeter length, centroid, and contiguity information
 */
template <typename T>
void Geometry<T>::updateCached() {
    if(cached) {
        return;
    }

    //reset cached data
    double x=0, y=0;
    perimeter = 0;
    contiguous = false;
    vertices.clear();
    minimumBoundingCircle = Circle();
    area = -1;
    centroid = GeoPoint();

    if(lines.empty()) {
        contiguous = true;
        area = 0;
        cached = true;
        return;
    }
 

    GeoLine *start = *(lines.begin());
    GeoLine* curr = start;
    const GeoPoint* arrivedAt = curr->getP1(); // arbitrary starting vertex
    std::set<GeoLine*> visited;

    do {
        vertices.push_back(arrivedAt);
        visited.insert(curr);
        const GeoPoint* otherEnd = (curr->getP1() == arrivedAt) ? curr->getP2() : curr->getP1();

        GeoLine* next = nullptr, *startNeighbor = nullptr;
        for (GeoLine* ln : endpointMap[*otherEnd]) {
            if (ln == curr) continue;
            if (ln == start) startNeighbor = ln; // remember but don't pick yet
            else if (!visited.count(ln) && lines.count(ln)) {
                next = ln;
                break;
            }

        }
        if (next == nullptr) next = startNeighbor;
        if (next == nullptr) {
            break;// dead end - malformed boundary
            std::cout << "Dead end reached in contiguity traversal, geometry is malformed." << std::endl;
        } 
        curr = next;
        arrivedAt = otherEnd;
    } while (curr != start);
    //contiguity calcs
    contiguous = (vertices.size() == lines.size());

    if(!contiguous) {
        cached = true;
        std::cout << "Found " << visited.size() << " lines, expected " << lines.size() << std::endl;
        //TODO emit error/warning
        return;
    }

    //perimeter & centroid calcs
    for(GeoLine *l : lines) {
        x += (l->getMidpoint().getX()*l->getLength());
        y += (l->getMidpoint().getY()*l->getLength());
        perimeter += l->getLength();
    }
    if(perimeter > 0) {
        x /= perimeter;
        y /= perimeter;
    }
    centroid = GeoPoint(x,y);

    // area calcs
        area = 0.0;
        int n = vertices.size();
        for (int i = 0; i < n; i++) {
            const GeoPoint* currPt = vertices[i];
            const GeoPoint* nextPt = vertices[(i + 1) % n];
            area += currPt->getX() * nextPt->getY() - nextPt->getX() * currPt->getY();
        }
        area = std::abs(area) / 2.0;

    minimumBoundingCircle = calcMinimumBoundingCircle(vertices);
    

    //compactness calcs
    polsbyPopper = (4*std::numbers::pi*area) / std::pow(perimeter, 2);
    reock = area/minimumBoundingCircle.getArea();
    cached = true;
}


template <typename T>
std::vector<const GeoPoint*> Geometry<T>::getOrderedVertices() {
    updateCached();
    return vertices;
}

template <typename T>
template <typename K>
void Geometry<T>::mergeGeometry(Geometry<K>& other) {
    for(GeoLine *l : other.getLines()) {
        auto it = lines.find(l);
        if(it != lines.end()) {
            lines.erase(l);
            l->removeOwner(this);
        }
        else {
            lines.insert(l);
            l->addOwner(this);
        }
    }
    cached = false;
}

template <typename T>
double Geometry<T>::getPolsbyPopper() {
    updateCached();
    return polsbyPopper;
}

template <typename T>
double Geometry<T>::getReock() {
    updateCached();
    return reock;
}