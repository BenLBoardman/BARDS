#include <set>
#include <cmath>
#include <unordered_map>

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
};


class GeoLine {
    private:
        GeoPoint p1, p2, midpoint;
        double length;
        std::set<UntypedGeometry*> owners;
    public:
        GeoLine(){}
        GeoLine(double x1, double x2, double y1, double y2);
        GeoPoint& getMidpoint();
        double getLength();
        void addOwner(UntypedGeometry* ug);
        const std::set<UntypedGeometry*>& getOwners() const;
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

std::unordered_map<GeoPoint, GeoLine> lineRegister;

template <typename T>
class Geometry : public UntypedGeometry {
    private:
        T owner;           
        std::set<GeoLine*> lines;
        bool cached; //has this Geometry been changed since the last time the centroid or perimeter has been calculated
        double perimeter;
        GeoPoint centroid;
    
    public:
        Geometry(T& owner) : owner(owner) {cached = false; }
        GeoLine& addLine(double x1, double x2, double y1, double y2);
        double getPerimeter();
        GeoPoint& getCentroid();
        
};

template <typename T>
GeoLine& Geometry<T>::addLine(double x1, double x2, double y1, double y2) {
    GeoLine candidate(x1, x2, y1, y2);
    auto [it, inserted] = lineRegister.emplace(candidate.getMidpoint(), candidate);
    GeoLine& line = it->second;
    line.addOwner(this);
    lines.insert(&line);
    cached = false;
    return line;
}

template <typename T>
GeoPoint& Geometry<T>::getCentroid() {
    if(cached) {
        return centroid;
    }
    double x=0, y=0;
    perimeter = 0;
    for(GeoLine *l : lines) {
        x += (l->getMidpoint().getX()*l->getLength());
        y += (l->getMidpoint().getY()*l->getLength());
        perimeter += l->getLength();
    }
    if(perimeter > 0) {
        x /= perimeter;
        y /= perimeter;
    }

    cached = true;
    centroid = GeoPoint(x,y);
    return centroid;
}

template <typename T>
double Geometry<T>::getPerimeter() {
    getCentroid(); //update perimeter and centroid if necessary
    return perimeter;
}

GeoLine::GeoLine(double x1, double x2, double y1, double y2) {
    p1 = GeoPoint(x1, y1);
    p2 = GeoPoint(x2, y2);
    midpoint = GeoPoint((x2+x1)/2, (y2+y1)/2);
    length = std::sqrt(pow((x2-x1),2)+pow((y2-y1),2));
}

GeoPoint& GeoLine::getMidpoint() {
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