#include "consultation.h"
#include <iostream>
#include <sqlite3.h>
#include "MediPass.h"
#include "medecin.h"

// Constructeurs
Consultation::Consultation() {}

Consultation::Consultation(MediPass* mp,sqlite3* db,
                          const Medecin* medecin,
                          int id_patient,
                          const std::string& motif,
                          const std::string& observations,
                          const std::vector<Examen> examens)
    :  patient_id(id_patient),
      motif(motif), observations(observations), examens(examens) {
      
      this->date_et_heure = mp->getTimeDate();
      
      professionnel_info.push_back(medecin->getFirstname());
      professionnel_info.push_back(medecin->getLast_name());

      sqlite3_exec(db,
        sqlite3_mprintf("SELECT first_name, last_name FROM users WHERE id = %d;", id_patient),
        MediPass::callback_names,
        &patient_info,
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
if (examens.empty()) {
    std::cout << "Consultation du " << date_et_heure
              << "\n-----Professionnel-----\n" << "Prénoms: " << professionnel_info[0] << "\nNom de famille: " << professionnel_info[1]
              << "\n-----Patient-----\n" << "Prénoms: " << patient_info[0] << "\nNom de famille: " << patient_info[1]
              << "\nMotif : " << motif
              << "\nObservations : " << observations
              << "\n-------------------------------\n";
} else {
    std::cout << "Consultation du " << date_et_heure
              << "\n-----Professionnel-----\n" << "Prénoms: " << professionnel_info[0] << "\nNom de famille: " << professionnel_info[1]
              << "\n-----Patient-----\n" << "Prénoms: " << patient_info[0] << "\nNom de famille: " << patient_info[1]
              << "\nMotif : " << motif
              << "\nObservations : " << observations
              << "\n-----Examens-----\n";
    for (const auto& examen : examens) {
      std::cout << "- Date: " << examen.getDate()
                << ", Type: " << examen.getTypeExamen()
                << ", Résultat: " << examen.getResultat() << "\n";
    }
    std::cout << "-------------------------------\n";
}
}
