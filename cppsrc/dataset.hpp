#include <string>


enum DataType {
    //demographic data types
    CVAP, //Citizen VAP data
    VAP,  //VAP data
    VAP_NH, //VAP non-hisp.
    ACS, //ACS pop estimate
    CENS, //census population

    //election data types
    PRES, //president
    GOV, //governor
    SEN, //senate
    LT_GOV, //lt. gov
    AG, //atty gen
    CONG //house composite
    //others as needed
};

class DataSet {
    protected:
        const unsigned int year;
        const DataType type;
    
    public:
        const unsigned int getYr();
        const DataType getType();
};

DataSet processSet(std::string json) {
    //TODO - parse enough JSON to figure out if we are dealing with demographic or election data and call/return the appropriate constructor
}

template <typename T>
class DemographicData : public DataSet {
    private:
        bool votingAge;
        int total, white, hispanic, black, asian, pacific, native, other, mixed;

    public:
        DemographicData(std::string json);
        DemographicData(unsigned int year, bool votingAge, DataType type) : votingAge(votingAge) {
            this->year = year;
            this->type = type;
            total = 0; white = 0; hispanic = 0; black = 0; asian = 0; pacific = 0; native = 0; other = 0; mixed = 0;
        };
        void mergeData(DemographicData target);
};

template <typename T>
class ElectionData : public DataSet {
    private:
        bool composite; //if true, then yr is the start year and officeID is the end year.
        unsigned int dem, rep, total;
    public:
        ElectionData(std::string json);
        ElectionData(unsigned int year, bool composite, DataType type) : composite(composite) {
            this->year = year;
            this->type = type;
            dem = 0; rep = 0; total = 0;
        };
        void mergeData(ElectionData target);

};

template <typename T>
DemographicData<T>::DemographicData(std::string json) {
    //TODO - process a JSON block into demographicData
}

template <typename T>
ElectionData<T>::ElectionData(std::string json) {
    //TODO - process a JSON block into ElectionData
}

//Merge a DemographicData's numbers into this one
template <typename T>
void DemographicData<T>::mergeData(DemographicData target) {
    if(type != target.type || year != target.year) {
        //can only merge data of the same type & year
        return;
    }
    total += target.total;
    white += target.white;
    hispanic += target.hispanic;
    black += target.black;
    asian += target.asian;
    pacific += target.pacific;
    native += target.native;
    other += target.other;
    mixed += target.mixed;
}

template <typename T>
void ElectionData<T>::mergeData(ElectionData target) {
    if(type != target.type || year != target.year) {
        //can only merge data of the same type & year
        return;
    }
    total += target.total;
    dem += target.dem;
    rep += target.rep;
}