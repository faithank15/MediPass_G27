#ifndef PROFESIONNEL_DE_SANTE_H
#define PROFESIONNEL_DE_SANTE_H

#include <chrono>
#include <vector>
#include <string>
#include "utilisateur.h"

// Forward declaration pour éviter les include circulaires
class Patient;

class Pro_sante: virtual public User{

public:
    class Invalid{};
    Pro_sante(MediPass* mp, sqlite3* db, const std::string& firstname,
              const std::string& last_name,
              const std::string& numero_de_tel,
              const std::string& autorisation,
              const std::string& role,
              const std::string& statut);

    // Acesseurs
    std::string obtenir_autorisation() const{ return autorisation; };
    std::string obtenir_role() const { return role; };
    std::string obtenir_statut() const { return statut; };

    const std::vector<std::chrono::system_clock::time_point>& obtenir_disponibilite() const{return liste_disponibilite; };
    void ajouter_date_disponible(const std::chrono::system_clock::time_point& date);

    // Mutateurs




protected:

    std::string autorisation;
    std::string role;
    std::string statut;

    std::vector<std::chrono::system_clock::time_point> liste_disponibilite;
    std::vector<Patient> patients_en_charge;


private:


};


#endif //PROFESIONNEL_DE_SANTE_H
