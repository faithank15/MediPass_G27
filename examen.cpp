#include "Examen.h"
#include <iostream>

Examen::Examen() {}

Examen::Examen(const std::string& date,
               const std::string& typeExamen,
               const std::string& resultat)
    : date(date), typeExamen(typeExamen), resultat(resultat) {}

std::string Examen::getDate() const { return date; }
std::string Examen::getTypeExamen() const { return typeExamen; }
std::string Examen::getResultat() const { return resultat; }

void Examen::setDate(const std::string& d) { date = d; }
void Examen::setTypeExamen(const std::string& t) { typeExamen = t; }
void Examen::setResultat(const std::string& r) { resultat = r; }

void Examen::afficher() const {
    std::cout << "Examen du " << date
              << "\nType : " << typeExamen
              << "\nRésultat : " << resultat
              << "\n-----------------------------\n";
}