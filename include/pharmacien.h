#ifndef PHARMACIEN_H
#define PHARMACIEN_H

#include <string>
#include <chrono>
#include <vector>
#include "profesionnel_de_sante.h"

struct Recommander{

    std::chrono::system_clock::time_point date_et_heure;
    std::string info_recommandation();

};

class Pharmacien: public Pro_sante

{

public:

    Pharmacien(MediPass* mp, sqlite3* db, const std::string& firstname,
              const std::string& last_name,
              std::string password,
              bool active,
              int telephone,
              std::string created_by,
              std::string created_at,
              std::string autorisation="A2");

    void recommander();
    void lire_dossier_medical(int patient_id);
    void menu();

};

#endif
