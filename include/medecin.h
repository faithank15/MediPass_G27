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

    // --- Structure pour gérer les disponibilités ---
    struct Disponibilite {
        int id;
        int day;
        std::string heure_start;
        std::string heure_end;
    };

    // --- Exceptions ---
    class Invalid {};

    // --- Constructeurs ---
    Medecin(MediPass* mp, sqlite3* db,
            std::string firstname,
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

    // --- Interface utilisateur ---
    virtual void menu();

    // --- Actions principales ---
    void lire_dossier_medical(sqlite3* db, MediPass* mp,
                              const std::string& firstname,
                              const std::string& last_name) const;

    bool editer_dossier_medical(sqlite3* db, MediPass* mp,
                                const std::string& firstname,
                                const std::string& last_name);

    bool creer_consultation(sqlite3* db, MediPass* mp,
                            const std::string& firstname,
                            const std::string& last_name,
                            const std::string& observations,
                            const std::string& motif);

    void programmer_consultation();
    void ajouter_consultation_interactive();

    // --- Gestion des disponibilités ---
    bool ajouter_disponibilite(sqlite3* db,
                               int id_medecin,
                               int day,
                               const std::string& heure_start,
                               const std::string& heure_end);

    std::vector<Disponibilite> charger_disponibilites(sqlite3* db, int id_medecin);
    void afficher_disponibilites();

    // --- Affichage des informations ---
    void afficher_patients();
    void afficher_infos_professionnelles();
    bool lire_dossier_medical_interactive();

    // --- Import / Export CSV des dossiers médicaux ---
    void exportDossiersCSV(const std::string& filename, const std::vector<int>& dossierIds = {});
    void importDossiersCSV(const std::string& filename);


    // --- Accesseurs ---
    std::string obtenir_specialite() const { return specialite; };

private:

};

#endif
