#include "DossierMedical.h"
#include <iostream>
#include <ctime>
#include <chrono>

// Méthode Utilitaires (Interne)


std::string DossierMedical::getCurrentTimeAsString() const {
    // Obtenir le temps actuel en secondes depuis l'époque
    std::time_t now = std::time(nullptr);

    // Convertir l'heure en structure locale (pour le formatage)
    std::tm* local_time = std::localtime(&now);

    char buf[100];

    // Formater l'heure dans la chaîne de caractères (buffer)
    if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local_time)) {
        return buf;
    }
    return "Erreur lors de l'obtention de l'heure.";
}

// Constructeur


DossierMedical::DossierMedical(int idD, int idP)
    : idDossier(idD), idPatient(idP) {

    // Initialisation automatique de l'heure de création lors de l'instanciation du dossier
    heureCreation = getCurrentTimeAsString();
}

// Accesseurs

int DossierMedical::getIdDossier() const {
    return idDossier;
}

int DossierMedical::getIdPatient() const {
    return idPatient;
}

std::string DossierMedical::getHeureCreation() const {
    return heureCreation;
}

// Getters pour les collections (retournent une référence constante)
const std::vector<Antecedant>& DossierMedical::getAntecedents() const {
    return antecedants;
}

const std::vector<Consultation>& DossierMedical::getConsultations() const {
    return consultations;
}

const std::vector<Examen>& DossierMedical::getExamens() const {
    return examens;
}

// Mutateurs

void DossierMedical::setIdDossier(int idD) {
    idDossier = idD;
}

void DossierMedical::setIdPatient(int idP) {
    idPatient = idP;
}

// Mutateurs d'ajout pour les collections
void DossierMedical::ajouterAntecedent(const Antecedant& a) {
    antecedants.push_back(a);
}

void DossierMedical::ajouterConsultation(const Consultation& c) {
    consultations.push_back(c);
}

void DossierMedical::ajouterExamen(const Examen& e) {
    examens.push_back(e);
}

DossierMedical::~DossierMedical()
{
    //dtor
}
