#include <string>

#include "json.hpp"

enum DataType {
    //demographic data types
    CVAP, //Citizen VAP data
    VAP,  //VAP data
    VAP_NH, //VAP non-hisp.
    ACS, //ACS pop estimate
    CENS, //census population

    //election data types
    COMP, //multi-election composite containing any number of elections
    PRES, //president
    GOV, //governor
    SEN, //senate
    LT_GOV, //lt. gov
    AG, //atty gen
    CONG, //house composite
    //others as needed

    ERR //generic error type
};

class DataSet {
    protected:
        const unsigned int year;
        const DataType type;
    
    public:
        DataSet() : year(0), type(ERR) {}
        DataSet(unsigned year, DataType type) : year(year), type(type) {}
        const unsigned int getYr();
        const DataType getType();
        virtual bool isDemographic() const = 0;
        bool operator<(const DataSet& other) const {
            if(type != other.type) return type < other.type;
            return year < other.year;
        }
};

bool isDemographic(const JsonValue& json);

class DemographicData : public DataSet {
    private:
        bool votingAge;
        int total, white, hispanic, black, asian, pacific, native, other, mixed;

    public:
        DemographicData(const DemographicData& schema, const JsonValue& json);
        DemographicData(const std::string& name, const JsonValue& json);
        DemographicData(unsigned int year, bool votingAge, DataType type) : DataSet(year, type), votingAge(votingAge) {
            total = 0; white = 0; hispanic = 0; black = 0; asian = 0; pacific = 0; native = 0; other = 0; mixed = 0;
        };
        void mergeData(DemographicData target);
        bool isDemographic() const override { return true; };
};

class ElectionData : public DataSet {
    private:
        bool composite; //if true, then yr is the start year and officeID is the end year.
        unsigned int dem, rep, total;
    public:
        ElectionData(const ElectionData& schema, const JsonValue& json);
        ElectionData(const std::string& name, const JsonValue& json);
        ElectionData(unsigned int year, bool composite, DataType type) : DataSet(year, type), composite(composite) {
            dem = 0; rep = 0; total = 0;
        };
        void mergeData(ElectionData target);
        bool isDemographic() const override { return false; };

};

DataType parseDataType(const std::string& name, const JsonValue& json);