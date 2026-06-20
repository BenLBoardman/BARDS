
enum DemographicType {
    CVAP, //Citizen VAP data
    VAP,  //VAP data
    VAP_NH, //VAP non-hisp.
    ACS, //ACS pop estimate
    CENS, //census population
};


enum OfficeType {
    PRES, //president
    GOV, //governor
    SEN, //senate
    LT_GOV, //lt. gov
    AG, //atty gen
    CONG //house composite
    //others as needed
};

class DataSet {
    private:
        unsigned int yr;
};

template <typename T>
class DemographicData : public DataSet {
    private:
        bool votingAge;
        DemographicType type;
        int total, white, hispanic, black, asian, pacific, native, other, mixed;

    public:
        DemographicData(unsigned int yr, bool votingAge, DemographicType type, int total, int white,
            int black, int asian, int pacific, int native, int other, int mixed);
};

template <typename T>
class ElectionData : public DataSet {
    private:
        bool composite; //if true, then yr is the start year and officeID is the end year.
        OfficeType type;
        unsigned int dem, rep, total;
    public:
        ElectionData(unsigned int yr, bool composite, OfficeType type, unsigned int dem, unsigned int rep, unsigned int total);

};

template <typename T>
DemographicData<T>::DemographicData(unsigned int yr, bool votingAge, DemographicType type, int total, int white,
            int black, int asian, int pacific, int native, int other, int mixed)
    : yr(yr), votingAge(votingAge), type(type), total(total), white(white), black(black), asian(asian), pacific(pacific), native(native), other(other), mixed(mixed) {}

template <typename T>
ElectionData<T>::ElectionData(unsigned int yr, bool composite, OfficeType type, unsigned int dem, unsigned int rep, unsigned int total) : 
    yr(yr), composite(composite), type(type), dem(dem), rep(rep), total(total){}