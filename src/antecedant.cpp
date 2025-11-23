#include "antecedant.h"


Antecedant::Antecedant(const std::string& description, const std::string& date_diagnosis)
    : description(description), date_diagnosis(date_diagnosis) {}

std::string Antecedant::getDescription() const {
    return description;
}

std::string Antecedant::getDateDiagnosis() const {
    return date_diagnosis;
}

