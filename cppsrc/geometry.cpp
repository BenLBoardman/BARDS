#include "geometry.hpp"
#include "electoralentity.hpp"

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

void GeoLine::addOwner(Geometry *ug) {
    owners.emplace(ug);
}

void GeoLine::removeOwner(Geometry* ug) {
    owners.erase(ug);
}

const std::set<Geometry*>& GeoLine::getOwners() const {
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


Geometry::Geometry(ElectoralEntity& owner) : owner(owner) {
    cached = false; 
    
}

void Geometry::loadGeometry(const JsonValue& json) {
    logs::info << "Loading geometry for " << owner.name << " (id: " << owner.id << ")" << std::endl;
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
        for(unsigned int k = 0; k < multiArray.size(); k++) {
            const JsonArray polyArray = multiArray[k].asArray();
            for(unsigned int i = 0; i < polyArray.size(); i++) {
                const JsonArray coordArray = polyArray[i].asArray();
                for(unsigned int j = 0; j < coordArray.size()-1; j++) {
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

GeoLine& Geometry::addLine(double x1, double x2, double y1, double y2) {
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

GeoPoint& Geometry::getCentroid() {
    updateCached();
    return centroid;
}

double Geometry::getPerimeter() {
    updateCached();
    return perimeter;
}

double Geometry::getArea() {
    updateCached();
    return area;
}

bool Geometry::isContiguous() {
    updateCached();
    return contiguous;
}


Circle Geometry::getMinimumBoundingCircle() {
    updateCached();
    return minimumBoundingCircle;
}

/**
 * Update Geometry perimeter length, centroid, and contiguity information
 */
void Geometry::updateCached() {
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
        logs::info << "Contiguity check failed on " << owner.name << " (id: " << owner.id << "). Found " << visited.size() << " lines, expected " << lines.size() << "." << std::endl;
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


std::vector<const GeoPoint*> Geometry::getOrderedVertices() {
    updateCached();
    return vertices;
}

void Geometry::mergeGeometry(Geometry& other) {
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

double Geometry::getPolsbyPopper() {
    updateCached();
    return polsbyPopper;
}

double Geometry::getReock() {
    updateCached();
    return reock;
}

double Geometry::distance(Geometry *other) {
    GeoPoint& oCentroid = other->getCentroid();
    getCentroid();
    return std::sqrt(std::pow(centroid.getX() - oCentroid.getX(), 2)+std::pow(centroid.getY()-oCentroid.getY(), 2));
}