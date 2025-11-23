#include "antecedant.h"
#include <iostream>

Antecedant::Antecedant() {}

Antecedant::Antecedant(const std::string& description,
                       const std::string& date)
    : description(description), date(date) {}

std::string Antecedant::getDescription() const { return description; }
std::string Antecedant::getDate() const { return date; }

void Antecedant::setDescription(const std::string& d) { description = d; }
void Antecedant::setDate(const std::string& d) { date = d; }

void Antecedant::afficher() const {
    std::cout << "Antecedant : " << description
              << "\nDate : " << date
              << "\n-----------------------------\n";
}
