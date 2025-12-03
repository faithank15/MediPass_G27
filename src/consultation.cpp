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
                          const std::string& prescription,
                          const std::vector<Examen> examens)
    :  patient_id(id_patient),
      motif(motif), observations(observations), prescription(prescription), examens(examens) {

      this->date_et_heure = mp->getTimeDate();

      dossier_id = mp->getDossierId(db, id_patient);

      professionnel_info.push_back(medecin->getFirstname());
      professionnel_info.push_back(medecin->getLast_name());

      sqlite3_exec(db,
        sqlite3_mprintf("SELECT first_name, last_name FROM users WHERE id = %d;", id_patient),
        MediPass::callback_names,
        &patient_info,
        nullptr
      );

      //Enregistrer la consultation dans la base de données
      const char* sql =
          "INSERT INTO CONSULTATIONS (date_et_heure,dossier_id, pro_id, patient_id, motif, observations) "
          "VALUES ('%q', %d, %d, '%q', '%q');";
      char* errMsg = nullptr;
      std::string formatted_sql = sqlite3_mprintf(
          sql,
          date_et_heure.c_str(),
          medecin->getId(),
          id_patient,
          motif.c_str(),
          observations.c_str()
      );
      if (sqlite3_exec(db, formatted_sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
          std::cerr << "Erreur lors de l'insertion de la consultation : " << errMsg << std::endl;
          sqlite3_free(errMsg);
      } else {
          id = sqlite3_last_insert_rowid(db); 
      }
      sqlite3_free((void*)formatted_sql.c_str());
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

void Consultation::ajouter_examen(const Examen& examen) {return examens.push_back(examen); };

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
