/*
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <chrono>

namespace fs = std::filesystem;


// ============================================================
// CONFIGURATION
// ============================================================


// ------------------------------------------------------------
// OPTIONS GÉNÉRALES
// ------------------------------------------------------------

// Écrire l'arborescence dans "hierarchie.txt"
const bool activerEcritureHierarchie = false;

// Mesurer et afficher le temps d'exécution
const bool activerChrono = true;

// Afficher les résultats dans la console
const bool activerCout = true;

// ------------------------------------------------------------
// OPTIONS DE COMPARAISON
// ------------------------------------------------------------

// Comparer les noms des fichiers
const bool activerComparaisonNomFichier = true;

// Comparer les extensions des fichiers
const bool activerComparaisonExtension = true;

// Exclure certains dossiers
const bool activerExclusionDossiers = true;


// ------------------------------------------------------------
// DOSSIER DE DÉPART
// ------------------------------------------------------------

const fs::path cheminDeDepart =
    R"(C:\Users\valen)";


// ------------------------------------------------------------
// DOSSIER DE COPIE
// ------------------------------------------------------------

const fs::path dossierCopie =
    "copied";


// ------------------------------------------------------------
// NOMS DE FICHIERS À RECHERCHER
// ------------------------------------------------------------

const std::vector<std::string> nomsFichiers =
{
    "menu selection_arme.png"
};


// ------------------------------------------------------------
// EXTENSIONS À RECHERCHER
// ------------------------------------------------------------

const std::vector<std::string> extensions =
{
};


// ------------------------------------------------------------
// DOSSIERS À EXCLURE
//
// Les chemins sont relatifs à cheminDeDepart.
//
// Exemple :
//
// "Projet/Build"
//
// correspond à :
//
// C:\Users\valen\Documents\Projet\Build
// ------------------------------------------------------------

const std::vector<fs::path> dossiersExclus =
{
    "Public",
    "Default"
};


// ============================================================
// VARIABLES DE STATISTIQUES
// ============================================================

int nombreFichiersParcourus = 0;
int nombreFichiersCopies = 0;


// ============================================================
// VÉRIFICATION D'UN DOSSIER EXCLU
// ============================================================

bool EstDossierExclu(const fs::path& dossier)
{
    // Si la fonctionnalité est désactivée,
    // aucun dossier n'est considéré comme exclu.

    if (!activerExclusionDossiers)
    {
        return false;
    }


    fs::path cheminRelatif;

    try
    {
        cheminRelatif =
            fs::relative(dossier, cheminDeDepart);
    }
    catch (const fs::filesystem_error&)
    {
        return false;
    }


    // Comparaison avec les chemins exclus
    for (const fs::path& cheminExclu : dossiersExclus)
    {
        if (cheminRelatif == cheminExclu)
        {
            return true;
        }
    }


    return false;
}


// ============================================================
// VÉRIFICATION DU FICHIER
// ============================================================

bool DoitEtreCopie(const fs::path& fichier)
{
    // --------------------------------------------------------
    // COMPARAISON DU NOM
    // --------------------------------------------------------

    if (activerComparaisonNomFichier)
    {
        const std::string nom =
            fichier.filename().string();


        for (const std::string& nomRecherche : nomsFichiers)
        {
            if (nom == nomRecherche)
            {
                return true;
            }
        }
    }


    // --------------------------------------------------------
    // COMPARAISON DE L'EXTENSION
    // --------------------------------------------------------

    if (activerComparaisonExtension)
    {
        const std::string extension =
            fichier.extension().string();


        for (const std::string& extensionRecherchee :
             extensions)
        {
            if (extension == extensionRecherchee)
            {
                return true;
            }
        }
    }


    return false;
}


// ============================================================
// COPIE DU FICHIER
// ============================================================

void CopierFichier(const fs::path& fichier)
{
    try
    {
        // Chemin relatif par rapport au dossier de départ
        fs::path cheminRelatif =
            fs::relative(fichier, cheminDeDepart);


        // Chemin final dans "copied"
        fs::path destination =
            dossierCopie / cheminRelatif;


        // Création des dossiers nécessaires
        fs::create_directories(
            destination.parent_path()
        );


        // Copie du fichier
        fs::copy_file(
            fichier,
            destination,
            fs::copy_options::overwrite_existing
        );

        if (activerCout) {
            std::cout
               << "[COPIE] "
               << fichier
               << " -> "
               << destination
               << '\n';
        }


        nombreFichiersCopies++;
    }
    catch (const fs::filesystem_error& e)
    {
        if (activerCout) {
            std::cerr
               << "[ERREUR COPIE] "
               << fichier
               << " : "
               << e.what()
               << '\n';
        }
    }
}


// ============================================================
// EXPLORATION RÉCURSIVE
// ============================================================

void ExplorerDossier(
    const fs::path& dossier,
    std::ofstream* fichierSortie,
    const std::string& indentation = ""
)
{
    try
    {
        for (const auto& element :
             fs::directory_iterator(dossier))
        {
            // =================================================
            // DOSSIER
            // =================================================

            if (element.is_directory())
            {
                const fs::path cheminDossier =
                    element.path();


                // ------------------------------------------------
                // Vérification de l'exclusion
                // ------------------------------------------------

                if (EstDossierExclu(cheminDossier))
                {
                    if (activerEcritureHierarchie &&
                        fichierSortie != nullptr)
                    {
                        *fichierSortie
                            << indentation
                            << "[D] "
                            << cheminDossier.filename().string()
                            << " [EXCLU]"
                            << '\n';
                    }


                    // Ne pas parcourir ce dossier
                    continue;
                }


                // ------------------------------------------------
                // Écriture du dossier
                // ------------------------------------------------

                if (activerEcritureHierarchie &&
                    fichierSortie != nullptr)
                {
                    *fichierSortie
                        << indentation
                        << "[D] "
                        << cheminDossier.filename().string()
                        << '\n';
                }


                // ------------------------------------------------
                // Exploration récursive
                // ------------------------------------------------

                ExplorerDossier(
                    cheminDossier,
                    fichierSortie,
                    indentation + "    "
                );
            }


            // =================================================
            // FICHIER
            // =================================================

            else if (element.is_regular_file())
            {
                nombreFichiersParcourus++;


                // ------------------------------------------------
                // Écriture du fichier dans hierarchie.txt
                // ------------------------------------------------

                if (activerEcritureHierarchie &&
                    fichierSortie != nullptr)
                {
                    *fichierSortie
                        << indentation
                        << "[F] "
                        << element.path().filename().string()
                        << '\n';
                }


                // ------------------------------------------------
                // Comparaison
                // ------------------------------------------------

                if (DoitEtreCopie(element.path()))
                {
                    CopierFichier(element.path());
                }
            }
        }
    }
    catch (const fs::filesystem_error& e)
    {
        if (activerEcritureHierarchie &&
            fichierSortie != nullptr)
        {
            *fichierSortie
                << indentation
                << "[ERREUR] "
                << e.what()
                << '\n';
        }
    }
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    // ========================================================
    // DÉBUT DU CHRONOMÈTRE
    // ========================================================

    std::chrono::high_resolution_clock::time_point debut;

    if (activerChrono)
    {
        debut =
            std::chrono::high_resolution_clock::now();
    }


    // ========================================================
    // VÉRIFICATION DU DOSSIER DE DÉPART
    // ========================================================

    if (!fs::exists(cheminDeDepart))
    {
        if (!activerCout)
            return 1;
        std::cerr
            << "Le dossier de depart n'existe pas : "
            << cheminDeDepart
            << '\n';

        return 1;
    }


    if (!fs::is_directory(cheminDeDepart))
    {
        if (!activerCout)
            return 1;
        std::cerr
            << "Le chemin de depart n'est pas un dossier : "
            << cheminDeDepart
            << '\n';

        return 1;
    }


    // ========================================================
    // CRÉATION DU DOSSIER DE COPIE
    // ========================================================

    try
    {
        fs::create_directories(dossierCopie);
    }
    catch (const fs::filesystem_error& e)
    {
        if (!activerCout)
            return 1;
        std::cerr
            << "Impossible de creer le dossier copied : "
            << e.what()
            << '\n';

        return 1;
    }


    // ========================================================
    // OUVERTURE DE HIERARCHIE.TXT
    // ========================================================

    std::ofstream fichierSortie;

    if (activerEcritureHierarchie)
    {
        fichierSortie.open("hierarchie.txt");


        if (!fichierSortie.is_open())
        {
            if (!activerCout)
                return 1;
            std::cerr
                << "Impossible d'ouvrir hierarchie.txt\n";

            return 1;
        }


        // Écriture de la racine
        fichierSortie
            << "[D] "
            << cheminDeDepart.filename().string()
            << '\n';
    }


    // ========================================================
    // PARCOURS
    // ========================================================

    ExplorerDossier(
        cheminDeDepart,

        // Si l'écriture est désactivée,
        // on donne nullptr.
        activerEcritureHierarchie
            ? &fichierSortie
            : nullptr,

        "    "
    );


    // ========================================================
    // FERMETURE DE HIERARCHIE.TXT
    // ========================================================

    if (activerEcritureHierarchie)
    {
        fichierSortie.close();
    }


    // ========================================================
    // FIN DU CHRONOMÈTRE
    // ========================================================

    if (activerChrono)
    {
        const auto fin =
            std::chrono::high_resolution_clock::now();


        const std::chrono::duration<double> duree =
            fin - debut;

        if (!activerCout)
            return 0;
        std::cout
            << "Temps total : "
            << duree.count()
            << " secondes\n";
    }

    if (!activerCout)
        return 0;
    // ========================================================
    // RÉSULTATS
    // ========================================================

    std::cout << '\n';

    std::cout
        << "====================================\n";

    std::cout
        << "              TERMINE\n";

    std::cout
        << "====================================\n";

    std::cout
        << "Fichiers parcourus : "
        << nombreFichiersParcourus
        << '\n';

    std::cout
        << "Fichiers copies    : "
        << nombreFichiersCopies
        << '\n';


    if (activerEcritureHierarchie)
    {
        std::cout
            << "Hierarchie.txt     : OUI\n";
    }
    else
    {
        std::cout
            << "Hierarchie.txt     : NON\n";
    }


    if (activerChrono)
    {
        std::cout
            << "Chronometre        : OUI\n";
    }
    else
    {
        std::cout
            << "Chronometre        : NON\n";
    }


    std::cout
        << "====================================\n";


    return 0;
}
*/
#include "RechercheFichiers.h"

int main()
{
    RechercheFichiers recherche;
    // ========================================================
    // CONFIGURATION DES BOOLÉENS
    // ========================================================
    recherche.ConfigurerOptions(
        true,   // Écriture de hierarchie.txt
        false,  // Ecriture dans la console
        true,   // Chronomètre
        true,   // Comparaison nom
        true,   // Comparaison extension
        true    // Exclusion dossiers
    );
    // ========================================================
    // CONFIGURATION DES FICHIERS
    // ========================================================
    recherche.ConfigurerFichiers(
        {
        },

        {
            ".txt"
        },

        {
        }
    );
    // ========================================================
    // DOSSIER DE DÉPART
    // ========================================================
    recherche.ConfigurerDossierDepart(
        R"(C:\Users\valen)"
    );
    recherche.ConfigurerDossierCopie(
        R"(FichiersCopies)"
    );
    // ========================================================
    // LANCEMENT
    // ========================================================
    recherche.LancerRecherche();
    return 0;
}