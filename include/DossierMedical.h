#ifndef DOSSIERMEDICAL_H
#define DOSSIERMEDICAL_H

#include <vector>
#include <string>
#include <chrono> // Pour la gestion automatique du temps (heure de création)
#include "Antecedent.h"
#include "Consultation.h"
#include "Examen.h"

class DossierMedical {

private:

    int idDossier;
    int idPatient;
    std::string heureCreation; // Format chaîne de caractères pour la simplicité
    std::vector<Antecedent> antecedents;
    std::vector<Consultation> consultations;
    std::vector<Examen> examens; // Ajout des examens

    // Méthode utilitaire interne pour obtenir l'heure courante formatée
    std::string getCurrentTimeAsString() const;

public:
    // Constructeur
    // Prend l'ID du dossier et l'ID du patient. L'heure de création est automatique.
    DossierMedical(int idD, int idP);

    // Accesseurs
    int getIdDossier() const;
    int getIdPatient() const;
    std::string getHeureCreation() const;

    // Getters pour les collections (retournent une référence constante)
    const std::vector<Antecedent>& getAntecedents() const;
    const std::vector<Consultation>& getConsultations() const;
    const std::vector<Examen>& getExamens() const;

    // Mutateurs

    void setIdDossier(int idD);
    void setIdPatient(int idP);
    // Pas de Setter pour l'heure de création, elle est automatique.

    // Mutateurs pour ajouter du contenu (méthodes d'ajout)
    void ajouterAntecedent(const Antecedent& a);
    void ajouterConsultation(const Consultation& c);
    void ajouterExamen(const Examen& e);

    //destructeur
     ~DossierMedical();
};

#endif // DOSSIERMEDICAL_H

