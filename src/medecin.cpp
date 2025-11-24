#include <algorithm>
#include <iostream>
#include "medecin.h"
#include "patient.h"
#include "consultation.h"


using namespace std;


static const vector<string> specialites_medicales = {
    "Médecine générale / Médecine de famille",
    "Cardiologie", "Dermatologie et vénéréologie",
    "Endocrinologie, diabétologie et métabolisme",
    "Gastro-entérologie et hépatologie",
    "Gériatrie", "Maladies infectieuses et tropicales",
    "Médecine interne", "Neurologie", "Oncologie médicale",
    "Pédiatrie", "Pneumologie", "Psychiatrie", "Rhumatologie",
    "Chirurgie générale / viscérale et digestive",
    "Chirurgie orthopédique et traumatologique",
    "Chirurgie plastique, reconstructrice et esthétique",
    "Gynécologie obstétrique", "Ophtalmologie",
    "Oto-rhino-laryngologie (ORL)", "Urologie",
    "Anesthésiologie et réanimation",
    "Anatomie et cytologie pathologiques",
    "Radiologie et imagerie médicale"
};


bool specialite_est_valide(const string& specialite)
{
    return find(specialites_medicales.begin(),
                specialites_medicales.end(),
                specialite) != specialites_medicales.end();
}



Medecin::Medecin(const string& firstname,
                 const string& last_name,
                 const string& numero_de_tel,
                 const string& autorisation,
                 const string& role,
                 const string& statut,
                 const string& specialite)
    : User(),
      Pro_sante(firstname,last_name, numero_de_tel, autorisation, role, statut),
      specialite{specialite}
{
    if(!specialite_est_valide(specialite))
        throw Invalid{};
}

Medecin::Medecin(const Medecin& original)
    : User{original.firstname,original.last_name,original.numero_de_tel,original.role},
        Pro_sante{original.firstname, original.last_name, original.numero_de_tel, original.autorisation,
            original.role, original.statut}
{

}

/*Consultation::Consultation(chrono::system_clock::time_point date_et_heure,
                           const Medecin* medecin,
                           const Patient& patient,
                           const string& observations,
                           const string& motif,
                           const vector<Examen>& examens)

    : date_et_heure{date_et_heure},
      medecin{medecin},
      patient{patient},
      observations{observations},
      motif{motif},
      examens{examens}
{

}
*/


void Medecin::lire_dossier_medical(const Patient& patient) const
{
    if(find(patients_en_charge.begin(), patients_en_charge.end(), patient)!= patients_en_charge.end()) {
        //Compléter les instructions pour afficher le dossier médical - reste production Assanat
    }else {
        // Interdire l'accès au dossier médical
    }
}

bool Medecin::editer_dossier_medical(const Patient& patient)
{
    bool peut_editer_dossier = find(patients_en_charge.begin(), patients_en_charge.end(), patient) != patients_en_charge.end();

    if(peut_editer_dossier){
        // Compléter ceci afin que le professionnel de santé puisse éditer le dossier médical
        return true;
    }else{
        // refuser la lecture du dossier médical
        return false;
    }

}

bool Medecin::creer_consultation(std::chrono::system_clock::time_point date_et_heure,
                        const Patient& patient,
                        const string& observations,
                        const string& motif,
                        const vector<Examen>& examens)
{

auto& dispo = obtenir_disponibilite();
    bool disponible = find(dispo.begin(), dispo.end(), date_et_heure) != dispo.end();

   if(!disponible)
        return false;

    Consultation c(date_et_heure, this, patient, observations, motif, examens);
    // ajouter la consultation au dossier médicale
    // Ajouter le patient à la liste des patients pris en charge par le médécin. Ce qui lui permet d'accéder au dossier médical et de l'éditer
    return true;
  }


void Medecin::mettre_disponibilite(const vector<std::chrono::system_clock::time_point>& dates)
{
    for(const std::chrono::system_clock::time_point& d : dates)
        liste_disponibilite.push_back(d);
}

int main() {

    try {
        // Création d'un médecin
        Medecin med{
            "Faith",
            "K.O",
            "+229 0143663080",         // numéro de téléphone
            "A1",                   // autorisation
            "Directeur Médical",    // rôle
            "Actif",                // statut
            "Cardiologie"           // spécialité
        };

        cout << "Médecin créé avec succès." << endl;
        cout << "Nom : " << med.obtenir_firstname()
             << " " << med.obtenir_lastname() << endl;
        cout << "Spécialité : " << med.obtenir_specialite() << endl;

        // Simuler des patients
        Patient p1{"Aimane", " "};
        Patient p2{"Madi", "Sdjn"};

        // Simuler des examens
        vector<Examen> examens{
            Examen("ECG"),
            Examen("Scanner thoracique")
        };

        // Disponibilité du médecin
        vector<chrono::system_clock::time_point> disp;
        disp.push_back(chrono::system_clock::now());

        med.mettre_disponibilite(disp);
        cout << "Disponibilité ajoutée." << endl;

        // Création d'une consultation
        bool ok = med.creer_consultation(
            chrono::system_clock::now(),
            p1,
            string("Douleur thoracique."),
            string("Angine de poitrine."),
            examens
        );

        if(ok)
            cout << "Consultation créée avec succès." << endl;
        else
            cout << "Le médecin n'était pas disponible." << endl;

    }
    catch (const Utilisateur::Invalid&) {
        cout << "Erreur : numéro de téléphone invalide !" << endl;
    }
    catch (const Pro_sante::Invalid&) {
        cout << "Erreur : autorisation ou rôle invalide !" << endl;
    }
    catch (const Medecin::Invalid&) {
        cout << "Erreur : spécialité invalide !" << endl;
    }
    catch (...) {
        cout << "Erreur inconnue !" << endl;
    }

    return 0;
}






//<>
