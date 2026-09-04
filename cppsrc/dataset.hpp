#pragma once

#include <string>

#include "util/json.hpp"

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
    TRE, //treasurer
    SOS, //sec of state
    AUD, //auditor
    //others as needed

    ERR //generic error type
};

class DataSet {
    protected:
        std::string title;
        unsigned int total;
    
    public:
        const unsigned int year;
        const DataType type;
        const std::string name;
        
        DataSet() : year(0), type(ERR) {}
        DataSet(unsigned year, DataType type, std::string title) : title(title), year(year), type(type), name(title) {
            if(type == CENS)
                this->title = name + " (Census)";
            else
                this->title = name;
        }
        const unsigned int getYr() const { return year; }
        const DataType getType() const { return type; }
        const std::string getTitle() const { return title; }
        virtual bool isDemographic() const = 0;
        bool operator<(const DataSet& other) const {
            if(type != other.type) return type < other.type;
            return year < other.year;
        }
        bool operator==(const DataSet& other) const {
            return type==other.type && year==other.year;
        }
        const int getTotal() const { return total; }
};

bool isDemographic(const JsonValue& json);

class DemographicData : public DataSet {
    private:
        bool votingAge;
        unsigned int white, hispanic, black, asian, pacific, native, other, mixed;

    public:
        DemographicData(const DemographicData& schema, const JsonValue& json);
        DemographicData(const DemographicData& schema);
        DemographicData(const std::string& name, const JsonValue& json);
        void mergeData(const DemographicData& target);
        void unmergeData(const DemographicData& target);
        bool isDemographic() const override { return true; };
        const int getWhite() const { return white; }
        const int getHispanic() const { return hispanic; }
        //todo more getters
};

class ElectionData : public DataSet {
    private:
        bool composite; //if true, then yr is the start year and officeID is the end year.
        unsigned int dem, rep, other;
    public:
        ElectionData(const ElectionData& schema, const JsonValue& json);
        ElectionData(const ElectionData& schema);
        ElectionData(const std::string& name, const JsonValue& json);
        void mergeData(const ElectionData& target);
        void unmergeData(const ElectionData& target);
        bool isDemographic() const override { return false; };
        unsigned int getDem() const { return dem; }
        unsigned int getRep() const { return rep; }
        unsigned int getOther() const { return other; }

};

DataType parseDataType(const std::string& name, const JsonValue& json);
