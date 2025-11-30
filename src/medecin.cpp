#include <algorithm>
#include <iostream>
#include <limits>
#include "medecin.h"
#include "patient.h"
#include "consultation.h"
#include "DossierMedical.h"

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
            std::string dateNaissance,
            std::string password,
            bool active,
            int telephone,
            std::string created_by,
            std::string created_at,
            std::string autorisation,
            std::string specialite)
     : Pro_sante(mp, db, firstname,last_name,dateNaissance,password,active,telephone,created_by,created_at,"A2","medecin",specialite)
{
    if(!specialite_est_valide(specialite))
        throw Invalid{};
}

/*
void Medecin::lire_dossier_medical(const Patient& patient) const
{
    auto it = find(patients_id.begin(), patients_id.end(), patient.getId());

    if(it != patients_id.end()) {
        std::cout << "Accès autorisé. Affichage du dossier médical...\n";

        const DossierMedical& dossier = patient.getDossierMedical();
        dossier.afficher(autorisation);

    } else {
        std::cerr << "Accès refusé : ce patient n'est pas dans votre liste.\n";
    }
}


bool Medecin::editer_dossier_medical(Patient& patient)
{
    auto it = find(patients_id.begin(), patients_id.end(), patient.getId());

    if(it == patients_id.end()){
        std::cout << "Accès refusé : ce patient n'est pas sous votre responsabilité.\n";
        return false;
    }

    DossierMedical* dossier = patient.getDossierMedical(); // accès modifiable

    int choix = -1;

    do {
        std::cout << "\n=== Edition du dossier médical ===\n";
        std::cout << "1. Ajouter un antécédent\n";
        std::cout << "2. Ajouter une consultation\n";
        std::cout << "0. Quitter\n";
        std::cout << "Votre choix : ";
        std::cin >> choix;

        switch (choix)
        {
            case 1: {
                std::string desc, date;
                std::cout << "Description de l'antécédent : ";
                std::cin.ignore();
                std::getline(std::cin, desc);

                std::cout << "Date : ";
                std::getline(std::cin, date);

                dossier->ajouterAntecedent(Antecedant(desc, date));
                break;
            }

            case 2: {
                std::string motif, obs;
                std::cout << "Motif de consultation : ";
                std::cin.ignore();
                std::getline(std::cin, motif);

                std::cout << "Observations : ";
                std::getline(std::cin, obs);

                // Une vraie consultation devrait inclure médecin + examens + BD
                Consultation c;
                c.setMotif(motif);
                c.setObservations(obs);
                c.setPatientId(patient.getId());
                c.setProId(this->getId());


                // --- Ajouter des examens à la consultation ---
                char choixExamen = 'n';
                std::cout << "Ajouter un examen ? (o/n) : ";
                std::cin >> choixExamen;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                while (choixExamen == 'o' || choixExamen == 'O') {
                    std::string dateExamen, typeExamen, resultat;

                    std::cout << "Date de l'examen (JJ/MM/AAAA) : ";
                    std::getline(std::cin, dateExamen);

                    std::cout << "Type d'examen : ";
                    std::getline(std::cin, typeExamen);

                    std::cout << "Résultat : ";
                    std::getline(std::cin, resultat);
                    Examen examen(dateExamen, typeExamen, resultat);

                    c.ajouter_examen(examen);                           // à nécessité une méthode ajouterExamen dans Consultation

                    std::cout << "Ajouter un autre examen ? (o/n) : ";
                    std::cin >> choixExamen;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }

                dossier->ajouterConsultation(c);
                break;
            }
            case 0:
                std::cout << "Fin de l'édition du dossier.\n";
                break;

            default:
                std::cout << "Choix invalide.\n";
        }

    } while (choix != 0);

    return true;
}
*/
// Cette méthode est susceptible de modification
bool Medecin::creer_consultation(
                        Patient& patient,                           // utiliser l'ID une fois qu'on sera en mesure de récupérer le patient dans la base de donnée sur la base de l'ID
                        const string& observations,
                        const string& motif,
                        const vector<Examen>& examens)
{
    // 1. Vérifier si son ID est dans la liste ids du médecin
    auto it = find(patients_id.begin(), patients_id.end(), patient.getId());

    if (it == patients_id.end()) {
        cout << "Erreur : ce patient n'est pas dans la liste des patients du médecin.\n";
        return false;
    }

    // 2. Créer la consultation
    Consultation c(mp, db, this,
                   patient.getId(),
                   motif,
                   observations,
                   examens);

    // 3. Ajouter la consultation au dossier médical du patient
    patient.getDossierMedical()->ajouterConsultation(c);

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

    if (patients_id.empty()) {
        cout << "Aucun patient actuellement.\n";
        return;
    }

    for (int i = 0; i < (int)patients_id.size(); ++i) {
        cout << i + 1 << ". ";
            //  << [i].getNomComplet() << "\n";                             // En attente de Faith   -   récupération du nombre de patients en relation avec le médécin actuel
    }
}


void Medecin::ajouter_consultation_interactive() {
    /*cout << "\n--- Ajouter une consultation ---\n";

    string motif, observations;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Motif : ";
    getline(cin, motif);

    cout << "Observations : ";
    getline(cin, observations);

    // --- Sélection du patient ---
    int id_patient;
    cout << "\nID du patient concerné : ";
    cin >> id_patient;

    // Vérifier si ce patient est suivi par le médecin
    auto it = find(patients_id.begin(), patients_id.end(), id_patient);
    if (it == patients_id.end()) {
        cout << "Erreur : ce patient n'est pas pris en charge par ce médecin.\n";
        return;
    }

    // Récupération du patient depuis la base de donnée
    //J'ai créé une méthode loadPatient qui retourne un pointeur sur l'objet contenant les infos que tu cherche mais il a besoin
    //du pointeur de la db et et du résultat de la méthode getCreds qui est aussi dans MediPass
    Patient patient;//
    try {
        // patient = mp->getPatientById(db, id_patient);                        // En attente de Faith
    }
    catch (const exception& e) {
        cout << "Erreur : impossible de charger le patient.\n";
        return;
    }

    cout << "\n--- Patient sélectionné : "
         << patient.getNomComplet() << " ---\n";

    // --- Ajout éventuel d'examens ---
    vector<Examen> examens;

    char choix;
    cout << "\nAjouter un examen ? (o/n) : ";
    cin >> choix;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (choix == 'o' || choix == 'O') {
        string date, typeExamen, resultat;

        cout << "\n--- Nouvel examen ---\n";

        cout << "Date (jj/mm/aaaa) : ";
        getline(cin, date);

        cout << "Type d'examen : ";
        getline(cin, typeExamen);

        cout << "Résultat : ";
        getline(cin, resultat);

        examens.emplace_back(date, typeExamen, resultat);

        cout << "Ajouter un autre examen ? (o/n) : ";
        cin >> choix;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // --- Création de la consultation ---
    Consultation nouvelleConsultation(
        mp,
        db,
        this,
        patient.getId(),
        observations,
        motif,
        examens
    );

    cout << "\nConsultation ajoutée avec succès pour le patient "

         << patient.getNomComplet() << ".\n";
         */
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
