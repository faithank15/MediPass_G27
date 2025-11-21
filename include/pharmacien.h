#ifndef PHARMACIEN_H
#define PHARMACIEN_H

#include <string>
#include <chrono>
#include <vector>

struct Recommander{

    std::chrono::system_clock::time_point date_et_heure;
    std::string info_recommandation();

};

class Pharmacien{

public:

    void recommander();
    void lire_dossier_medical();
    
};

#endif