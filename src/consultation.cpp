#include "consultation.h"
#include <iostream>

// Constructeurs
Consultation::Consultation() {}

Consultation::Consultation(const std::string& date,
                           const std::string& professionnel,
                           const std::string& motif,
                           const std::string& observations)
    : date(date), professionnel(professionnel),
      motif(motif), observations(observations) {}

// Getters
std::string Consultation::getDate() const { return date; }
std::string Consultation::getProfessionnel() const { return professionnel; }
std::string Consultation::getMotif() const { return motif; }
std::string Consultation::getObservations() const { return observations; }

// Setters
void Consultation::setDate(const std::string& d) { date = d; }
void Consultation::setProfessionnel(const std::string& p) { professionnel = p; }
void Consultation::setMotif(const std::string& m) { motif = m; }
void Consultation::setObservations(const std::string& o) { observations = o; }

// Affichage
void Consultation::afficher() const {
    std::cout << "Consultation du " << date
              << "\nProfessionnel : " << professionnel
              << "\nMotif : " << motif
              << "\nObservations : " << observations
              << "\n-------------------------------\n";
}
