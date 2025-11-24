#ifndef CONSULTATION_H
#define CONSULTATION_H
#include <chrono>

#include <string>

class Consultation {
private:
    std::chrono::system_clock::time_point date_et_heure;
    int pro_id;
    int patient_id;
    std::string motif;
    std::string observations;

public:
    Consultation();
    Consultation(const std::chrono::system_clock::time_point date_et_heure,
                 const int& pro_id,
                 const int& patient_id,
                 const std::string& motif,
                 const std::string& observations);

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

    // Affichage
    void afficher() const;
};

#endif
