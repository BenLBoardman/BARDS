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
        const std::string title;
    
    public:
        DataSet() : year(0), type(ERR) {}
        DataSet(unsigned year, DataType type, std::string title) : year(year), type(type), title(title) {}
        const unsigned int getYr() const { return year; }
        const DataType getType() const { return type; }
        const std::string getTitle() const { return title; }
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
        void mergeData(DemographicData target);
        bool isDemographic() const override { return true; };
        const int getTotal() const { return total; }
};

class ElectionData : public DataSet {
    private:
        bool composite; //if true, then yr is the start year and officeID is the end year.
        unsigned int dem, rep, total;
    public:
        ElectionData(const ElectionData& schema, const JsonValue& json);
        ElectionData(const std::string& name, const JsonValue& json);
        void mergeData(ElectionData target);
        bool isDemographic() const override { return false; };

};

DataType parseDataType(const std::string& name, const JsonValue& json);