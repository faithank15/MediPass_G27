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

    // Surchage d'opérateur - // Je vous en prie - ne supprimer plus les surchages d'opérateurs, cela me permet d'utiliser "find" pour parcourir des vecteurs de ces objets et de les comparer - c'est souvent plus pratique q'une boucle for
    bool operator==(const Patient& other) const {
        return this->getNomComplet() == other.getNomComplet(); 
    }

private:
    int id;
    std::string firstname;
    std::string lastname;
    std::string dateNaissance;
};

#endif
