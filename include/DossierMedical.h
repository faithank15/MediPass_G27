#ifndef DOSSIERMEDICAL_H
#define DOSSIERMEDICAL_H
#include <sqlite3.h>
#include <vector>
#include <string>
#include <chrono> // Pour la gestion automatique du temps (heure de cr�ation)
#include "antecedant.h"
#include "examen.h"
#include "Soin.h"

class Consultation;

class DossierMedical {

private:

    int idDossier;
    int idPatient;
    std::string heureCreation; // Format cha�ne de caract�res pour la simplicit�
    std::vector<Antecedant> antecedants;
    std::vector<Consultation> consultations;
    std::vector<Examen> examens; // Ajout des examens
    std::vector<Soin> soins;

    // M�thode utilitaire interne pour obtenir l'heure courante format�e
    std::string getCurrentTimeAsString() const;

public:
    // Constructeur
    // Prend l'ID du dossier et l'ID du patient. L'heure de cr�ation est automatique.
    DossierMedical(sqlite3* db,int idP);

    DossierMedical(int idPatient, const std::string& heureCreation,std::vector<Antecedant> antecedants,std::vector<Consultation> consultations,std::vector<Examen> examens,std::vector<Soin> soins);

    // Accesseurs
    int getIdDossier() const;
    int getIdPatient() const;
    std::string getHeureCreation() const;

    // Getters pour les collections (retournent une r�f�rence constante)
    const std::vector<Antecedant>& getAntecedents() const;
    const std::vector<Consultation>& getConsultations() const;
    const std::vector<Examen>& getExamens() const;

    // Mutateurs

    void setIdDossier(int idD);
    void setIdPatient(int idP);
    // Pas de Setter pour l'heure de cr�ation, elle est automatique.

    // Mutateurs pour ajouter du contenu (m�thodes d'ajout)
    void ajouterAntecedent(const Antecedant& a);
    void ajouterConsultation(const Consultation& c);
    void ajouterExamen(const Examen& e);

    //destructeur
     ~DossierMedical();

     // afficher le dossier medical
     int afficher(std::string autorisation);

};

#endif // DOSSIERMEDICAL_H

