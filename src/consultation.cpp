#include "consultation.h"
#include <iostream>
#include <sqlite3.h>
#include "MediPass.h"

// Constructeurs
Consultation::Consultation() {}

Consultation::Consultation(const std::chrono::system_clock::time_point date_et_heure,
                           const int& pro_id,
                           const int& patient_id,
                           const std::string& motif,
                           const std::string& observations)
    : date_et_heure(date_et_heure), pro_id(pro_id), patient_id(patient_id),
      motif(motif), observations(observations) {
      
      std::vector<std::string> professionnel_info;
      professionnel_info.push_back(std::to_string(pro_id));
      sqlite3_exec(db,
        "SELECT first_name, last_name FROM users WHERE id = " + std::to_string(pro_id) + ";",
        MediPass::callback_names,
        &professionnel_info,
        nullptr
      );
      }

// Getters
std::string Consultation::getDateHeure() const { return date_et_heure; }
int Consultation::getProId() const { return pro_id; }
int Consultation::getPatientId() const { return patient_id; }
std::string Consultation::getMotif() const { return motif; }
std::string Consultation::getObservations() const { return observations; }

// Setters
void Consultation::setDateHeure(const std::string& d) { date_et_heure = d; }
void Consultation::setProId(const int& p) { pro_id = p; }
void Consultation::setPatientId(const int& p) { patient_id = p; }
void Consultation::setMotif(const std::string& m) { motif = m; }
void Consultation::setObservations(const std::string& o) { observations = o; }

// Affichage
void Consultation::afficher() const {
    std::cout << "Consultation du " << date_et_heure
              << "\nProfessionnel : " << professionnel
              << "\nMotif : " << motif
              << "\nObservations : " << observations
              << "\n-------------------------------\n";
}
