#include "Antecedent.h"
#include <iostream>

Antecedent::Antecedent() {}

Antecedent::Antecedent(const std::string& description,
                       const std::string& date)
    : description(description), date(date) {}

std::string Antecedent::getDescription() const { return description; }
std::string Antecedent::getDate() const { return date; }

void Antecedent::setDescription(const std::string& d) { description = d; }
void Antecedent::setDate(const std::string& d) { date = d; }

void Antecedent::afficher() const {
    std::cout << "Antécédent : " << description
              << "\nDate : " << date
              << "\n-----------------------------\n";
}