#ifndef GEOMETRY
#define GEOMETRY

#include <set>
#include <stack>
#include <cmath>
#include <unordered_map>
#include <string>

#include "json.hpp"

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
        GeoPoint centroid;
        void updateCached();
        const std::vector<const GeoPoint*> getOrderedVertices();
    
    public:
        Geometry(T& owner);
        void loadGeometry(const JsonValue& json);
        GeoLine& addLine(double x1, double x2, double y1, double y2);
        double getPerimeter();
        GeoPoint& getCentroid();
        double getArea();
        const std::set<GeoLine*> getLines() const { return lines; }
        T& getOwner() { return owner; }
        template <typename K>
        void mergeGeometry(Geometry<K> other);
};

template <typename T>
Geometry<T>::Geometry(T& owner) : owner(owner) {
    cached = false; 
    
}

template <typename T>
void Geometry<T>::loadGeometry(const JsonValue& json) {
    if(json["type"].asString() == "Polygon") {
        const JsonArray coordArray = json["coordinates"][0].asArray();
        for(int i = 0; i < coordArray.size() - 1; i++) {
            addLine(coordArray[i][0].asNumber(), coordArray[i+1][0].asNumber(), coordArray[i][1].asNumber(), coordArray[i+1][1].asNumber());
        }
    }
    else if(json["type"].asString() == "MultiPolygon") {
        const JsonArray polyArray = json["coordinates"][0].asArray();
        for(int i = 0; i < polyArray.size(); i++) {
            const JsonArray coordArray = polyArray[i].asArray();
            for(int j = 0; j < coordArray.size()-1; j++) {
                addLine(coordArray[j][0].asNumber(), coordArray[j+1][0].asNumber(), coordArray[j][1].asNumber(), coordArray[j+1][1].asNumber());
            }
        }
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

/**
 * Update Geometry perimeter length, centroid, and contiguity information
 */
template <typename T>
void Geometry<T>::updateCached() {
    if(cached) {
        return;
    }
    double x=0, y=0;
    perimeter = 0;
    contiguous = false;

    if(lines.empty()) {
        cached = true;
        return;
    }

    for(GeoLine *l : lines) {
        x += (l->getMidpoint().getX()*l->getLength());
        y += (l->getMidpoint().getY()*l->getLength());
        perimeter += l->getLength();
    }
    if(perimeter > 0) {
        x /= perimeter;
        y /= perimeter;
    }

    //continuity calcs
    std::set<GeoLine*> discovered;
    std::stack<GeoLine*> visitQueue;
    visitQueue.push(*lines.begin());
    while(!visitQueue.empty()) {
        GeoLine* curr = visitQueue.top();
        visitQueue.pop();
        if (discovered.count(curr)) continue;
        discovered.insert(curr);
        // check both endpoints
        for (const GeoPoint* endpt : {curr->getP1(), curr->getP2()}) {
            for (GeoLine* ln : endpointMap[*endpt]) {
                if (!discovered.count(ln)) {
                    visitQueue.push(ln);
                }
            }
        }
    }
    contiguous = discovered.size() == lines.size();

    area = -1;
    if(contiguous) { // area calcs
        std::vector<const GeoPoint*> ordered = getOrderedVertices();
        area = 0.0;
        int n = ordered.size();
        for (int i = 0; i < n; i++) {
            const GeoPoint* curr = ordered[i];
        const GeoPoint* next = ordered[(i + 1) % n];
            area += curr->getX() * next->getY() - next->getX() * curr->getY();
    }
    area = std::abs(area) / 2.0;
    }

    cached = true;
    centroid = GeoPoint(x,y);
}


template <typename T>
const std::vector<const GeoPoint*> Geometry<T>::getOrderedVertices() {
    std::vector<const GeoPoint*> vertices;
    GeoLine* start = *(lines.begin());
    GeoLine* curr = start;
    do {
        vertices.push_back(curr->getP1());
        const GeoPoint* nextPt = curr->getP2();
        for(GeoLine* ln : endpointMap[*nextPt]) {
            if(ln != curr)
                curr = ln;
        }
    } while (curr != start);


    return vertices;
}

template <typename T>
template <typename K>
void Geometry<T>::mergeGeometry(Geometry<K> other) {
    for(GeoLine *l : other.getLines()) {
        if(lines.find(l) != lines.end()) {
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


#endif