#include <algorithm>
#include <iostream>
#include <limits>
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



Medecin::Medecin(MediPass* mp,sqlite3* db,std::string firstname,
            std::string last_name,
            std::string password,
            std::string role,
            bool active,
            int telephone,
            std::string created_by,
            std::string created_at,
            const std::string& autorisation,
            const std::string& statut,
            const std::string& specialite)
     : Pro_sante(mp, db, firstname,last_name,password,role,active,telephone,created_by,created_at,autorisation,statut),
      specialite{specialite}
{
    if(!specialite_est_valide(specialite))
        throw Invalid{};
}

/*
Consultation::Consultation(chrono::system_clock::time_point date_et_heure,
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
                        const Medecin* medecin,
                        int id_patient,
                        const string& observations,
                        const string& motif,
                        const vector<Examen>& examens)
{

auto& dispo = obtenir_disponibilite();
    bool disponible = find(dispo.begin(), dispo.end(), date_et_heure) != dispo.end();

   if(!disponible)
        return false;

    Consultation c(mp, db, this, id_patient, motif, observations);
    // ajouter la consultation au dossier médicale
    // Ajouter le patient à la liste des patients pris en charge par le médécin. Ce qui lui permet d'accéder au dossier médical et de l'éditer
    return true;
  }


void Medecin::mettre_disponibilite(const vector<std::chrono::system_clock::time_point>& dates)
{
    for(const std::chrono::system_clock::time_point& d : dates)
        liste_disponibilite.push_back(d);
}

void Medecin::afficher_disponibilites() {
    cout << "\n--- Vos disponibilités ---\n";

    if (obtenir_disponibilite().empty()) {
        cout << "Aucune disponibilité enregistrée.\n";
        return;
    }

    for (const auto& d : obtenir_disponibilite()) {
        time_t t = chrono::system_clock::to_time_t(d);                      // ici je convertis le type chrono en time_t (un format classique lisible par c_time)
        cout << " - " << ctime(&t);                                         // ctime retourne une chaine de caractère correspondant à la date associé à l'objet de type time_t
    }
}


void Medecin::afficher_patients() {
    cout << "\n--- Liste des patients suivis ---\n";

    if (patients_en_charge.empty()) {
        cout << "Aucun patient actuellement.\n";
        return;
    }

    for (int i = 0; i < (int)patients_en_charge.size(); ++i) {
        cout << i + 1 << ". "
             << patients_en_charge[i].getNomComplet() << "\n";
    }
}


void Medecin::ajouter_consultation_interactive() {
    cout << "\n--- Ajouter une consultation ---\n";

    string motif, observations;

    // On vide le buffer avant les getline
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Motif : ";
    getline(cin, motif);

    cout << "Observations : ";
    getline(cin, observations);

    // --- Sélection du patient ---
    int id_patient;
    cout << "\nID du patient concerné : ";
    cin >> id_patient;

    // L'idéal ici serait de faire une Recherche du patient, une fois trouvé on continue, sinon on affiche une erreur
    // En attendant je vais juste créer un patient - Faith devra me montrer comment faire pour récupérer le patient depuis la base de donnée

    cout << "\n--- Informations du patient ---\n";

    int id;
    string nom, prenom, naissance;

    cout << "ID du patient : ";
    cin >> id;
    cin.ignore();

    cout << "Nom : ";
    getline(cin, nom);

    cout << "Prénom : ";
    getline(cin, prenom);

    cout << "Date de naissance (JJ/MM/AAAA) : ";
    getline(cin, naissance);

    Patient p(id, nom, prenom, naissance);

    // Vérifions si un/des examens ont été effectué

    vector<Examen> examens;

    char choix;
    cout << "\nAjouter un examen ? (o/n) : ";
    cin >> choix;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');            // Ici on vide le buffer afin de ne pas laisser des chaînes vides coincées dans l'objet cin dù à une utilisation antérieur

    while (choix == 'o' || choix == 'O') {
        string date, typeExamen, resultat;

        cout << "\n--- Nouvel examen ---\n";

        cout << "Date de l'examen (jj/mm/aaaa) : ";
        getline(cin, date);

        cout << "Type d'examen : ";
        getline(cin, typeExamen);

        cout << "Résultat : ";
        getline(cin, resultat);

        examens.emplace_back(date, typeExamen, resultat);          // ici emplace_back est une méthode très utile car elle construit l'objet directement à l'intérieur du vecteur en passant les arguments du constructeur. Il nous évite donc la copie ou le déplacement.

        cout << "Ajouter un autre examen ? (o/n) : ";
        cin >> choix;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Ici on crée la consultation
    //auto now = chrono::system_clock::now();

    Consultation nouvelleConsultation(
        mp,
        db,
        this,
        p.getId(),
        observations,
        motif,
        examens
    );

    // Une fois la consultation crée - elle est automatiquement ajouté à la liste des différentes consultations.

    cout << "\nConsultation ajoutée avec succès pour le patient "
         << p.getNomComplet() << ".\n";
}



void Medecin::afficher_infos_professionnelles() {
    cout << "\n--- Vos informations professionnelles ---\n";
    cout << "Nom d'utilisateur : " << Medecin::getLast_name() << "\n";
    cout << "Rôle : " << Medecin::getRole() << "\n";
    cout << "Autorisation : " << obtenir_autorisation() << "\n";
    cout << "Statut : " << obtenir_statut() << "\n";
    cout << "Actif : " << (isActive() ? "Oui" : "Non") << "\n";
}

void Medecin::menu() {
    int choix = -1;

    while (choix != 0) {
        cout << "\n==============================\n";
        cout << "     MENU DU MÉDECIN\n";
        cout << "==============================\n";
        cout << "Connecté en tant que : Dr " << Medecin::getLast_name() << " (" << Medecin::getRole() << ")\n";
        cout << "Statut : " << obtenir_statut()
             << " | Autorisation : " << obtenir_autorisation() << "\n";
        cout << "------------------------------\n";
        cout << "1. Voir la liste de mes patients\n";
        cout << "2. Voir mes disponibilités\n";
        cout << "3. Ajouter une nouvelle consultation\n";
        cout << "4. Voir mes informations professionnelles\n";
        cout << "0. Se déconnecter / Quitter\n";
        cout << "------------------------------\n";
        cout << "Votre choix : ";

        cin >> choix;

        // Ici on gère les entrées invalides
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrée invalide. Veuillez réessayer.\n";
            continue;
        }

        switch (choix) {
            case 1:
                afficher_patients();
                break;

            case 2:
                afficher_disponibilites();
                break;

            case 3:
                ajouter_consultation_interactive();
                break;

            case 4:
                afficher_infos_professionnelles();
                break;

            case 0:
                cout << "Déconnexion...\n";
                break;

            default:
                cout << "Choix invalide.\n";
                break;
        }
    }
}



/*int main() {

    try {
        cout << "\n===== TEST : Création d'un médecin =====\n";

        Medecin med(
            1,                      // id
            "drfaith",              // username
            "pass123",              // password
            "Directeur Médical",    // role
            true,                   // active
            229998877,              // telephone
            "admin",                // created_by
            "2024-12-01",           // created_at
            "A1",                   // autorisation
            "Actif",                // statut
            "Cardiologie"           // spécialité
        );

        cout << "Médecin créé avec succès.\n";
        cout << "Nom d'utilisateur : " << med.getUsername() << "\n";
        cout << "Spécialité : " << med.obtenir_specialite() << "\n\n";

        // ------------------------------
        cout << "===== TEST : Création de patients =====\n";

        Patient p1(1, "Aimane", "Diallo", "12/03/2004");
        Patient p2(2, "Madi", "Soglo", "21/08/2003");

        cout << "Patient 1 : " << p1.getNomComplet() << "\n";
        cout << "Patient 2 : " << p2.getNomComplet() << "\n\n";

        // ------------------------------
        cout << "===== TEST : Ajout de disponibilités =====\n";

        vector<chrono::system_clock::time_point> dispo;
        dispo.push_back(chrono::system_clock::now());

        med.mettre_disponibilite(dispo);

        cout << "Disponibilité ajoutée.\n\n";

        // ------------------------------
        cout << "===== TEST : Création d'examens =====\n";

        vector<Examen> examens{
            Examen("01/01/2025", "ECG", "Normal"),
            Examen("02/01/2025", "Scanner Thoracique", "RAS")
        };

        cout << "2 examens créés.\n\n";

        // ------------------------------
        cout << "===== TEST : Création consultation =====\n";

        auto now = chrono::system_clock::now();

        bool ok = med.creer_consultation(
            now,
            p1,
            "Douleur thoracique",
            "Suspicion angine de poitrine",
            examens
        );

        if(ok)
            cout << "Consultation créée avec succès.\n";
        else
            cout << "Échec : le médecin n'était pas disponible.\n";

        cout << "\n===== DISPONIBILITÉS =====\n";
        med.afficher_disponibilites();

        cout << "\n===== PATIENTS SUIVIS =====\n";
        med.afficher_patients();

        cout << "\n===== INFORMATIONS PROFESSIONNELLES =====\n";
        med.afficher_infos_professionnelles();

    }
    catch (const Pro_sante::Invalid&) {
        cout << "\nERREUR : Paramètres invalides pour Pro_sante.\n";
    }
    catch (const Medecin::Invalid&) {
        cout << "\nERREUR : Spécialité invalide.\n";
    }
    catch (...) {
        cout << "\nERREUR INCONNUE.\n";
    }

    return 0;
}*/








//<>
