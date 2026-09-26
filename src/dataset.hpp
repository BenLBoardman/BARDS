/**
 * @file dataset.hpp
 * @brief Demographic and election data set classes attached to electoral
 *        entities (precincts, districts, states), the DataType enum used
 *        to classify them, and helpers for parsing dataset schema JSON.
 */
#pragma once

#include <string>

#include "util/json.hpp"

/**
 * @brief Classifies the kind and source of a DataSet: demographic survey
 *        types followed by election office types.
 */
enum DataType {
    //demographic data types
    CVAP, ///< Citizen voting-age population data.
    VAP,  ///< Voting-age population data.
    VAP_NH, ///< Voting-age population data, non-Hispanic breakdown.
    ACS, ///< American Community Survey population estimate.
    CENS, ///< Decennial census population data.

    //election data types
    COMP, ///< Multi-election composite containing any number of elections.
    PRES, ///< Presidential election.
    GOV, ///< Gubernatorial election.
    SEN, ///< U.S. Senate election.
    LT_GOV, ///< Lieutenant governor election.
    AG, ///< Attorney general election.
    CONG, ///< U.S. House (congressional) composite election.
    TRE, ///< Treasurer election.
    SOS, ///< Secretary of State election.
    AUD, ///< Auditor election.
    //others as needed

    ERR ///< Generic/unrecognized data type, used as a default or error value.
};

/**
 * @class DataSet
 * @brief Abstract base class for a single named/typed/year-stamped data
 *        set (demographic or election) that can be attached to an
 *        ElectoralEntity and merged/unmerged as precincts are assigned to
 *        or removed from districts.
 */
class DataSet {
    protected:
        /** @brief Human-readable title for this data set (may differ from name; census-type data appends " (Census)"). */
        std::string title;
        /** @brief Aggregate total value tracked by this data set (e.g. total population or total votes). */
        unsigned int total;
    
    public:
        /** @brief The year this data set's data corresponds to. */
        const unsigned int year;
        /** @brief The DataType classification of this data set. */
        const DataType type;
        /** @brief The short/internal name of this data set (used as its map key). */
        const std::string name;
        
        /** @brief Construct a default, empty/error DataSet. */
        DataSet() : year(0), type(ERR) {}
        /**
         * @brief Construct a DataSet schema with a year, type, and title.
         * @param year The year this data corresponds to.
         * @param type The DataType classification.
         * @param title The display title (and initial name) for this data set; census-type data gets " (Census)" appended to its title.
         */
        DataSet(unsigned year, DataType type, std::string title) : title(title), year(year), type(type), name(title) {
            if(type == CENS)
                this->title = name + " (Census)";
            else
                this->title = name;
        }
        /**
         * @brief Get the year this data set corresponds to.
         * @return The year.
         */
        unsigned int getYr() const { return year; }
        /**
         * @brief Get the DataType classification of this data set.
         * @return The DataType.
         */
        DataType getType() const { return type; }
        /**
         * @brief Get the display title of this data set.
         * @return The title.
         */
        std::string getTitle() const { return title; }
        /**
         * @brief Determine whether this data set is demographic (as opposed to election) data.
         * @return True if demographic.
         */
        virtual bool isDemographic() const = 0;
        /**
         * @brief Order data sets by type, then by year.
         * @param other The data set to compare against.
         * @return True if this data set sorts before other.
         */
        bool operator<(const DataSet& other) const {
            if(type != other.type) return type < other.type;
            return year < other.year;
        }
        /**
         * @brief Compare data sets for equality of type and year.
         * @param other The data set to compare against.
         * @return True if type and year match.
         */
        bool operator==(const DataSet& other) const {
            return type==other.type && year==other.year;
        }
        /**
         * @brief Get the aggregate total value for this data set.
         * @return The total.
         */
        int getTotal() const { return total; }
};

/**
 * @brief Determine whether a dataset's JSON schema describes a demographic (vs. election) data set.
 * @param json The dataset schema JSON, expected to have a "type" field.
 * @return True if the "type" field equals "demographic".
 */
bool isDemographic(const JsonValue& json);

/**
 * @class DemographicData
 * @brief A DataSet holding racial/ethnic population breakdowns for an
 *        electoral entity (e.g. VAP, CVAP, ACS, or Census data).
 */
class DemographicData : public DataSet {
    private:
        /** @brief Whether this data set represents voting-age population figures. */
        bool votingAge;
        /** @brief Population counts by racial/ethnic category. */
        unsigned int white, hispanic, black, asian, pacific, native, other, mixed;

    public:
        /**
         * @brief Construct a DemographicData instance for a specific entity by extracting its category values from JSON, using another instance as the schema (year/type/name/votingAge).
         * @param schema An existing DemographicData providing the year, type, name, and votingAge flag.
         * @param json The JSON object of per-category values for this entity (keyed by category name, varying by schema type).
         */
        DemographicData(const DemographicData& schema, const JsonValue& json);
        /**
         * @brief Construct an empty (all-zero) DemographicData sharing the year/type/name/votingAge flag of a schema instance.
         * @param schema An existing DemographicData providing the year, type, name, and votingAge flag.
         */
        DemographicData(const DemographicData& schema);
        /**
         * @brief Construct a DemographicData schema (all-zero values) from its dataset definition JSON.
         * @param name The dataset's internal name, used to help infer its DataType.
         * @param json The dataset schema JSON, containing "year", "title", and optionally "votingAge".
         */
        DemographicData(const std::string& name, const JsonValue& json);
        /**
         * @brief Add another DemographicData's category counts into this one, if their types and years match.
         * @param target The DemographicData to merge in.
         */
        void mergeData(const DemographicData& target);
        /**
         * @brief Subtract another DemographicData's category counts from this one, if their types and years match.
         * @param target The DemographicData to unmerge.
         */
        void unmergeData(const DemographicData& target);
        /**
         * @brief Identify this as a demographic data set.
         * @return Always true.
         */
        bool isDemographic() const override { return true; };
        /**
         * @brief Get the white population count.
         * @return The white population count.
         */
        int getWhite() const { return white; }
        /**
         * @brief Get the Hispanic population count.
         * @return The Hispanic population count.
         */
        int getHispanic() const { return hispanic; }
        //todo more getters
};

/**
 * @class ElectionData
 * @brief A DataSet holding two-party (plus other) vote totals for a
 *        specific election or election composite attached to an electoral
 *        entity.
 */
class ElectionData : public DataSet {
    private:
        /** @brief If true, this data set is a composite of multiple elections (year is the start year, type encodes the composite kind). */
        bool composite; //if true, then yr is the start year and officeID is the end year.
        /** @brief Democratic, Republican, and other-party vote totals. */
        unsigned int dem, rep, other;
    public:
        /**
         * @brief Construct an ElectionData instance for a specific entity by extracting its vote totals from JSON, using another instance as the schema (year/type/name/composite).
         * @param schema An existing ElectionData providing the year, type, name, and composite flag.
         * @param json The JSON object containing this entity's "Total", "Dem", and "Rep" vote counts.
         */
        ElectionData(const ElectionData& schema, const JsonValue& json);
        /**
         * @brief Construct an empty (all-zero) ElectionData sharing the year/type/name/composite flag of a schema instance.
         * @param schema An existing ElectionData providing the year, type, name, and composite flag.
         */
        ElectionData(const ElectionData& schema);
        /**
         * @brief Construct an ElectionData schema (all-zero values) from its dataset definition JSON.
         * @param name The dataset's internal name (passed through to type parsing).
         * @param json The dataset schema JSON, containing "year", "title", "type", and "office".
         */
        ElectionData(const std::string& name, const JsonValue& json);
        /**
         * @brief Add another ElectionData's vote totals into this one, if their types and years match.
         * @param target The ElectionData to merge in.
         */
        void mergeData(const ElectionData& target);
        /**
         * @brief Subtract another ElectionData's vote totals from this one, if their types and years match.
         * @param target The ElectionData to unmerge.
         */
        void unmergeData(const ElectionData& target);
        /**
         * @brief Identify this as a non-demographic (election) data set.
         * @return Always false.
         */
        bool isDemographic() const override { return false; };
        /**
         * @brief Get the Democratic vote total.
         * @return The Democratic vote total.
         */
        unsigned int getDem() const { return dem; }
        /**
         * @brief Get the Republican vote total.
         * @return The Republican vote total.
         */
        unsigned int getRep() const { return rep; }
        /**
         * @brief Get the other-party vote total.
         * @return The other-party vote total.
         */
        unsigned int getOther() const { return other; }

};

/**
 * @brief Determine the DataType of a dataset from its definition JSON, using its name as a fallback hint for demographic type suffixes.
 * @param name The dataset's internal name (used to detect the VAP_NH type and, for demographic sets, the type suffix such as "_VAP" or "_CVAP").
 * @param json The dataset schema JSON; if its "type" is "election", the "office" field selects the DataType.
 * @return The resolved DataType.
 * @throws std::runtime_error if the office or dataset type suffix is not recognized.
 */
DataType parseDataType(const std::string& name, const JsonValue& json);