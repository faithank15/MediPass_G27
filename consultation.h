#ifndef CONSULTATION_H
#define CONSULTATION_H

#include <string>

class Consultation {
private:
    std::string date;
    std::string professionnel;
    std::string motif;
    std::string observations;

public:
    Consultation();
    Consultation(const std::string& date,
                 const std::string& professionnel,
                 const std::string& motif,
                 const std::string& observations);

    // Getters
    std::string getDate() const;
    std::string getProfessionnel() const;
    std::string getMotif() const;
    std::string getObservations() const;

    // Setters
    void setDate(const std::string& d);
    void setProfessionnel(const std::string& p);
    void setMotif(const std::string& m);
    void setObservations(const std::string& o);

    // Affichage
    void afficher() const;
};

#endif