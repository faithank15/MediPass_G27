#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <chrono>
#include <vector>

// Similation d'une classe Patient pour un test - À revoir

class Patient {
public:
    Patient(std::string nom, std::string prenom)
        : nom(nom), prenom(prenom) {}

    std::string obtenir_nom() const { return nom; }
    std::string obtenir_prenom() const { return prenom; }

    // Ici je surchage l'opérateur "==" afin de pouvoir utiliser la fonction "find"
    bool operator==(const Patient& other) const {
        return nom == other.nom && prenom == other.prenom;      // c'est assez simple - cela nous permettra de trouver facilement s'il existe une correspondance entre deux patients sans passer par une boucle range for et une vérification une à une - grâce à find()
    }

private:
    std::string nom;
    std::string prenom;
};

#endif