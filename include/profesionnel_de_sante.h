#ifndef PROFESIONNEL_DE_SANTE_H
#define PROFESIONNEL_DE_SANTE_H

#include <chrono>
#include <vector>
#include <string>
#include <sqlite3.h>
#include "utilisateur.h"
#include "MediPass.h"

// Forward declaration pour éviter les include circulaires
class Patient;

class Pro_sante: public User{

public:
    class Invalid{};

    virtual void menu()=0;
    Pro_sante(MediPass* mp, sqlite3* db, const std::string& firstname,
              const std::string& last_name,
              std::string password,
              bool active,
              int telephone,
              std::string created_by,
              std::string created_at,
              std::string autorisation,
              std::string statut);

    // Acesseurs
    std::string obtenir_autorisation() const{ return autorisation; };
    std::string obtenir_statut() const { return statut; };

    const std::vector<std::chrono::system_clock::time_point>& obtenir_disponibilite() const{return liste_disponibilite; };
    void ajouter_date_disponible(const std::chrono::system_clock::time_point& date);

    // Mutateurs




protected:
    MediPass* mp;
    sqlite3* db;

    std::string autorisation;
    std::string role;
    std::string statut;

    std::vector<std::chrono::system_clock::time_point> liste_disponibilite;
    std::vector<int> patients_id;


private:


};


#endif //PROFESIONNEL_DE_SANTE_H
