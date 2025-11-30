#ifndef CONSULTATION_H
#define CONSULTATION_H
#include <chrono>

#include <string>
#include "MediPass.h"
#include "medecin.h"

class Examen;

class Consultation {
private:
    std::string date_et_heure;
    int pro_id;
    int patient_id;
    std::string motif;
    std::string observations;

public:
    Consultation();
    Consultation(MediPass* mp,sqlite3* db,
                 const Medecin* medecin,
                 int id_patient,
                 const std::string& motif,
                 const std::string& observations,
                 const std::vector<Examen> examens = {}
                );

    // Getters
    std::string getDateHeure() const;
    int getProId() const;
    int getPatientId() const;
    std::string getMotif() const;
    std::string getObservations() const;

    // Setters
    void setDateHeure(const std::string& d);
    void setProId(const int& p);
    void setPatientId(const int& p);
    void setMotif(const std::string& m);
    void setObservations(const std::string& o);
    void ajouter_examen(Examen& examen);

    // Affichage
    void afficher() const;
    //Menu
    void menu();
private:
    std::vector<std::string> professionnel_info;
    std::vector<std::string> patient_info;
    std::vector<Examen> examens;
};

#endif
