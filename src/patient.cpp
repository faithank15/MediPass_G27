#include "patient.h"

Patient::Patient()
    : id(0), firstname(""), lastname(""), dateNaissance("") {}

Patient::Patient(int id,
                 const std::string& firstname,
                 const std::string& lastname,
                 const std::string& dateNaissance)
    : id(id), firstname(firstname), lastname(lastname), dateNaissance(dateNaissance) {}

int Patient::getId() const { return id; }

std::string Patient::getNomComplet() const {
    return firstname + " " + lastname;
}

std::string Patient::getDateNaissance() const {
    return dateNaissance;
}