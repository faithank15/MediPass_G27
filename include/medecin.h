#ifndef MEDECIN_H
#define MEDECIN_H

#include <string>
#include <chrono>
#include <vector>

#include "examen.h"
#include "profesionnel_de_sante.h"
#include "patient.h"


class Medecin : public Pro_sante
{
public:

    class Invalid {};

    virtual void menu();

    Medecin(MediPass* mp,sqlite3* db,std::string firstname,
            std::string last_name,
            std::string dateNaissance,
            std::string password,
            bool active,
            int telephone,
            std::string created_by,
            std::string created_at,
            std::string autorisation = "A2",
            std::string specialite = "");


    Medecin(const Medecin& original);


    // Actions
    void lire_dossier_medical(sqlite3* db, MediPass* mp, const string& firstname,const string& last_name) const;
    bool editer_dossier_medical(sqlite3* db, MediPass* mp, const string& firstname, const string& last_name);


    bool creer_consultation(
        sqlite3* db,
        MediPass* mp,
        const string& firstname,
        const string& last_name,                    // utiliser l'ID une fois qu'on sera en mesure de récupérer le patient dans la base de donnée sur la base de l'ID
        const string& observations,
        const string& motif);


    void mettre_disponibilite(const std::vector<std::chrono::system_clock::time_point>& dates);


    // Accesseur

    std::string obtenir_specialite() const { return specialite; };

    void afficher_patients();
    void afficher_disponibilites();
    void ajouter_consultation_interactive();
    void afficher_infos_professionnelles();
    bool lire_dossier_medical_interactive();

private:
    std::string specialite;
};

#endif
