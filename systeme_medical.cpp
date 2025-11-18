#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// ----------------------------------------------------
// Classe Antecedent
// Représente un événement médical passé (allergie, chirurgie, pathologie, etc.).
// ----------------------------------------------------
class Antecedent {
private:
    std::string type;         // Ex: "Allergie", "Chirurgie", "Pathologie"
    std::string description;  // Détails spécifiques
    std::string dateEnregistrement; // Date de l'événement ou de l'enregistrement

public:
    // Constructeur
    Antecedent(const std::string& type, const std::string& desc, const std::string& date)
        : type(type), description(desc), dateEnregistrement(date) {}

    // Méthode d'affichage
    void afficher() const {
        std::cout << "\t- Type: " << type
                  << " | Date: " << dateEnregistrement
                  << " | Description: " << description << std::endl;
    }
    
    // Getters
    const std::string& getType() const { return type; }
    const std::string& getDescription() const { return description; }
};

// ----------------------------------------------------
// Classe Examen
// Représente un examen réalisé (ex: analyse de sang, radiographie) et son résultat.
// ----------------------------------------------------
class Examen {
private:
    std::string nom;          // Ex: "Radiographie pulmonaire", "Analyse de sang"
    std::string resultat;     // Interprétation des résultats
    std::string dateExamen;

public:
    // Constructeur
    Examen(const std::string& nom, const std::string& res, const std::string& date)
        : nom(nom), resultat(res), dateExamen(date) {}

    // Méthode d'affichage
    void afficher() const {
        std::cout << "\t\t-> Examen: " << nom
                  << " | Date: " << dateExamen
                  << " | Résultat: " << resultat << std::endl;
    }
    
    // Getters
    const std::string& getNom() const { return nom; }
    const std::string& getResultat() const { return resultat; }
};

// ----------------------------------------------------
// Classe Consultation
// Représente un acte médical documenté et agrège les examens associés.
// ----------------------------------------------------
class Consultation {
private:
    std::string dateConsultation;
    std::string idProfessionnelSante; 
    std::string motif;
    std::string observations;
    std::vector<Examen> examens; // Agrégation : Liste des examens liés

public:
    // Constructeur
    Consultation(const std::string& date, const std::string& idPro, const std::string& motif, const std::string& obs)
        : dateConsultation(date), idProfessionnelSante(idPro), motif(motif), observations(obs) {}

    // Méthode pour ajouter un examen à la consultation
    void ajouterExamen(const Examen& examen) {
        examens.push_back(examen);
    }

    // Méthode d'affichage complète
    void afficher() const {
        std::cout << "\n\t* Consultation du " << dateConsultation << std::endl;
        std::cout << "\t  Professionnel ID: " << idProfessionnelSante << std::endl;
        std::cout << "\t  Motif: " << motif << std::endl;
        std::cout << "\t  Observations: " << observations << std::endl;

        if (!examens.empty()) {
            std::cout << "\t  --- Examens Associés (" << examens.size() << ") ---" << std::endl;
            for (const auto& ex : examens) {
                ex.afficher();
            }
        }
    }
    
    // Getters
    const std::string& getDate() const { return dateConsultation; }
    const std::string& getMotif() const { return motif; }
};

// ----------------------------------------------------
// Classe DossierMedical (Simplifiée)
// Centralise l'historique d'un patient.
// ----------------------------------------------------
class DossierMedical {
private:
    std::string idPatient;
    // Les deux collections principales du dossier
    std::vector<Antecedent> antecedents;
    std::vector<Consultation> historiqueConsultations;

public:
    DossierMedical(const std::string& id) : idPatient(id) {}

    // Méthodes d'ajout
    void ajouterAntecedent(const Antecedent& a) {
        antecedents.push_back(a);
    }

    void ajouterConsultation(const Consultation& c) {
        historiqueConsultations.push_back(c);
    }
    
    // Affichage synthétique du dossier
    void afficherHistoriqueComplet() const {
        std::cout << "\n=================================================" << std::endl;
        std::cout << "  Dossier Médical pour Patient ID: " << idPatient << std::endl;
        std::cout << "=================================================" << std::endl;

        // Affichage des Antécédents
        std::cout << "\n--- Antécédents (" << antecedents.size() << ") ---" << std::endl;
        for (const auto& ant : antecedents) {
            ant.afficher();
        }

        // Affichage des Consultations
        std::cout << "\n--- Historique des Consultations (" << historiqueConsultations.size() << ") ---" << std::endl;
        for (const auto& cons : historiqueConsultations) {
            cons.afficher();
        }
        std::cout << "\n=================================================" << std::endl;
    }
    
    // Getters
    const std::string& getPatientID() const { return idPatient; }
    // Note : Pour un vrai système, on aurait des méthodes plus complexes de recherche/filtrage
};


// ----------------------------------------------------
// Fonction main de démonstration
// ----------------------------------------------------
int main() {
    // 1. Création du Dossier Médical
    DossierMedical dossier123("P-4589");

    // 2. Ajout des Antécédents
    Antecedent allergie("Allergie", "Pénicilline (réaction cutanée)", "2015-05-10");
    Antecedent chirurgie("Chirurgie", "Appendicectomie", "2020-01-15");
    dossier123.ajouterAntecedent(allergie);
    dossier123.ajouterAntecedent(chirurgie);

    // 3. Création d'une première Consultation
    Consultation cons1("2025-10-25", "M-101", "Forte fièvre et toux", "Diagnostic initial de grippe saisonnière.");
    
    // 4. Ajout d'Examens à cette Consultation
    Examen radio("Radio thorax", "Clair, pas d'infiltration visible.", "2025-10-25");
    Examen sang("Bilan sanguin", "CRP légèrement élevée (marqueur inflammatoire).", "2025-10-25");
    
    cons1.ajouterExamen(radio);
    cons1.ajouterExamen(sang);
    
    // 5. Ajout de la Consultation au Dossier
    dossier123.ajouterConsultation(cons1);
    
    // 6. Création et ajout d'une deuxième consultation (sans examen)
    Consultation cons2("2025-11-18", "M-101", "Visite de suivi", "Généralement bonne récupération, conseils nutritionnels.");
    dossier123.ajouterConsultation(cons2);

    // 7. Affichage du Dossier Complet pour validation
    std::cout << "--- DEMONSTRATION DU SYSTEME D'INFORMATION MEDICAL ---" << std::endl;
    dossier123.afficherHistoriqueComplet();

    return 0;
}