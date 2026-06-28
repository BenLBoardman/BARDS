#include "dataset.hpp"

bool isDemographic(const JsonValue& json) {
    return json["type"].asString() == "demographic";
}

DemographicData::DemographicData(DemographicData& schema, const JsonValue& json) : DataSet(schema.year, schema.type, schema.title) {
    votingAge = schema.votingAge;
    total    = json["Total"].isNull()    ? 0 : json["Total"].asInt();
    white    = json["White"].isNull()    ? 0 : json["White"].asInt();
    hispanic = json["Hispanic"].isNull() ? 0 : json["Hispanic"].asInt();

    if (schema.type == VAP_NH) {
        black   = json["BlackAlone"].isNull()   ? 0 : json["BlackAlone"].asInt();
        asian   = json["AsianAlone"].isNull()   ? 0 : json["AsianAlone"].asInt();
        native  = json["NativeAlone"].isNull()  ? 0 : json["NativeAlone"].asInt();
        pacific = json["PacificAlone"].isNull() ? 0 : json["PacificAlone"].asInt();
        other   = json["OtherAlone"].isNull()   ? 0 : json["OtherAlone"].asInt();
        mixed   = json["TwoOrMore"].isNull()    ? 0 : json["TwoOrMore"].asInt();
    } else {
        black   = json["Black"].isNull()   ? 0 : json["Black"].asInt();
        asian   = json["Asian"].isNull()   ? 0 : json["Asian"].asInt();
        native  = json["Native"].isNull()  ? 0 : json["Native"].asInt();
        pacific = json["Pacific"].isNull() ? 0 : json["Pacific"].asInt();
        other   = json["Other"].isNull()   ? 0 : json["Other"].asInt();
        mixed   = json["RemTwoOrMore"].isNull() ? 0 : json["RemTwoOrMore"].asInt();
    }

    schema.mergeData(*this);
}


ElectionData::ElectionData(ElectionData& schema, const JsonValue& json) : DataSet(schema.year, schema.type, schema.title) {
    composite = schema.composite;
    total = json["Total"].isNull() ? 0 : json["Total"].asInt();
    dem   = json["Dem"].isNull()   ? 0 : json["Dem"].asInt();
    rep   = json["Rep"].isNull()   ? 0 : json["Rep"].asInt();

    schema.mergeData(*this);
}

DemographicData::DemographicData(const std::string& name, const JsonValue& json) : DataSet(json["year"].asInt(), parseDataType(name, json), json["title"].asString()) {
    votingAge = !json["votingAge"].isNull() && json["votingAge"].asBool();
    total = white = hispanic = black = asian = pacific = native = other = mixed = 0;
}

ElectionData::ElectionData(const std::string& name, const JsonValue& json) : DataSet(json["year"].asInt(), parseDataType(name, json), json["title"].asString()) {
    composite = (type == COMP);
    dem = rep = total = 0;
}

//Merge a DemographicData's numbers into this one
void DemographicData::mergeData(DemographicData target) {
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

void ElectionData::mergeData(ElectionData target) {
    if(type != target.type || year != target.year) {
        //can only merge data of the same type & year
        return;
    }
    total += target.total;
    dem += target.dem;
    rep += target.rep;
}


DataType parseDataType(const std::string& name, const JsonValue& json) {
    if (json["type"].asString() == "election") {
        std::string office = json["office"].asString();
        if (office == "pres")   return PRES;
        if (office == "gov")    return GOV;
        if (office == "sen")    return SEN;
        if (office == "comp")   return COMP;
        if (office == "lt_gov") return LT_GOV;
        if (office == "ag")     return AG;
        if (office == "con")   return CONG;
        throw std::runtime_error("Unknown election office: " + office);
    }
    // check VAP_NH before generic suffix parsing since it has two underscored parts
    if (name.find("VAP_NH") != std::string::npos) return VAP_NH;
    
    std::string suffix = name.substr(name.rfind('_') + 1);
    if (suffix == "CVAP") return CVAP;
    if (suffix == "VAP")  return VAP;
    if (suffix == "ACS")  return ACS;
    if (suffix == "CENS") return CENS;
    throw std::runtime_error("Unknown dataset type: " + name);
}