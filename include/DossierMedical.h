#ifndef DOSSIERMEDICAL_H
#define DOSSIERMEDICAL_H

#include <vector>
#include <string>
#include <chrono> // Pour la gestion automatique du temps (heure de cr�ation)
#include "antecedant.h"
#include "consultation.h"
#include "examen.h"
#include "Soin.h"

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
    DossierMedical(int idD, int idP);

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
     void afficher(std::string autorisation) const;

};

#endif // DOSSIERMEDICAL_H

