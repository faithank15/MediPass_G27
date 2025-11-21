#ifndef PATIENT_HPP
#define PATIENT_HPP

#include <string>

class Patient {
public:
    Patient(); // nécessaire pour load_patient()

    Patient(int id,
            const std::string& firstname,
            const std::string& lastname,
            const std::string& dateNaissance);

    int getId() const;
    std::string getNomComplet() const;
    std::string getDateNaissance() const;

private:
    int id;
    std::string firstname;
    std::string lastname;
    std::string dateNaissance;
};

#endif
