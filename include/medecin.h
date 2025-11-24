#ifndef MEDECIN_H
#define MEDECIN_H

#include <string>
#include <chrono>
#include <vector>

#include "examen.h"
#include "profesionnel_de_sante.h"
#include "patient.h"
class Medecin;


struct Consultation
{
    Consultation(std::chrono::system_clock::time_point date_et_heure,
                 const Medecin* medecin,
                 const Patient& patient,
                 const std::string& observations,
                 const std::string& motif,
                 const std::vector<Examen>& examens);

    std::chrono::system_clock::time_point date_et_heure;
    const Medecin* medecin; // Référence légère au médecin associé
    Patient patient;
    std::string observations;
    std::string motif;
    std::vector<Examen> examens;
};



class Medecin : public Pro_sante
{
public:

    class Invalid {};

    virtual void menu();

    Medecin(int id,
            std::string username,
            std::string password,
            std::string role, 
            bool active,
            int telephone,
            std::string created_by,
            std::string created_at,
            const std::string& autorisation,
            const std::string& statut,
            const std::string& specialite);


    Medecin(const Medecin& original);


    // Actions
    void lire_dossier_medical(const Patient& patient) const;
    bool editer_dossier_medical(const Patient& patient);


    bool creer_consultation(
        std::chrono::system_clock::time_point date_et_heure,
        const Patient& patient,
        const std::string& observations,
        const std::string& motif,
        const std::vector<Examen>& examens);


    void mettre_disponibilite(const std::vector<std::chrono::system_clock::time_point>& dates);


    // Accesseur
    std::string obtenir_specialite() const { return specialite; };

    void afficher_patients();
    void afficher_disponibilites();
    void ajouter_consultation_interactive();
    void afficher_infos_professionnelles();



private:
    std::string specialite;
};

#endif
