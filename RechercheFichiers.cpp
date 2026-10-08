#include "RechercheFichiers.h"

#include <iostream>
#include <chrono>

namespace fs = std::filesystem;


// ============================================================
// CONSTRUCTEUR
// ============================================================

RechercheFichiers::RechercheFichiers()
{
    activerEcritureHierarchie = true;
    activerCout = true;
    activerChrono = true;

    activerComparaisonNomFichier = true;
    activerComparaisonExtension = true;
    activerExclusionDossiers = true;

    cheminDeDepart = "";
    dossierCopie = "copied";

    nombreFichiersParcourus = 0;
    nombreFichiersCopies = 0;
}


// ============================================================
// CONFIGURATION DES OPTIONS
// ============================================================

void RechercheFichiers::ConfigurerOptions(
    bool ecritureHierarchie,
    bool cout,
    bool chrono,
    bool comparaisonNomFichier,
    bool comparaisonExtension,
    bool exclusionDossiers
)
{
    activerEcritureHierarchie =
        ecritureHierarchie;

    activerCout =
        cout;

    activerChrono =
        chrono;

    activerComparaisonNomFichier =
        comparaisonNomFichier;

    activerComparaisonExtension =
        comparaisonExtension;

    activerExclusionDossiers =
        exclusionDossiers;
}


// ============================================================
// CONFIGURATION DES FICHIERS
// ============================================================

void RechercheFichiers::ConfigurerFichiers(
    const std::vector<std::string>& nouveauxNomsFichiers,
    const std::vector<std::string>& nouvellesExtensions,
    const std::vector<fs::path>& nouveauxDossiersExclus
)
{
    nomsFichiers = nouveauxNomsFichiers;
    extensions = nouvellesExtensions;
    dossiersExclus = nouveauxDossiersExclus;
}


// ============================================================
// CONFIGURATION DU DOSSIER DE DÉPART
// ============================================================

void RechercheFichiers::ConfigurerDossierDepart(
    const fs::path& chemin
)
{
    cheminDeDepart = chemin;
}

void RechercheFichiers::ConfigurerDossierCopie(
    const std::filesystem::path& chemin
)
{
    dossierCopie = chemin;
}
// ============================================================
// VÉRIFICATION D'UN DOSSIER EXCLU
// ============================================================

bool RechercheFichiers::EstDossierExclu(
    const fs::path& dossier
)
{
    // --------------------------------------------------------
    // Exclusion désactivée
    // --------------------------------------------------------

    if (!activerExclusionDossiers)
    {
        return false;
    }


    // --------------------------------------------------------
    // Le dossier de copie doit TOUJOURS être exclu
    // --------------------------------------------------------

    try
    {
        /*
         * On compare les chemins absolus/canoniques
         * pour éviter les problèmes de chemins relatifs.
         */
        if (fs::equivalent(
                dossier,
                dossierCopie))
        {
            return true;
        }
    }
    catch (const fs::filesystem_error&)
    {
        // Le dossier n'existe peut-être pas encore.
    }


    // --------------------------------------------------------
    // Comparaison avec les dossiers exclus
    // --------------------------------------------------------

    fs::path cheminRelatif;

    try
    {
        cheminRelatif =
            fs::relative(
                dossier,
                cheminDeDepart
            );
    }
    catch (const fs::filesystem_error&)
    {
        return false;
    }


    for (const fs::path& cheminExclu :
         dossiersExclus)
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

bool RechercheFichiers::DoitEtreCopie(
    const fs::path& fichier
)
{
    // --------------------------------------------------------
    // COMPARAISON DU NOM
    // --------------------------------------------------------

    if (activerComparaisonNomFichier)
    {
        const std::string nom =
            fichier.filename().string();


        for (const std::string& nomRecherche :
             nomsFichiers)
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

void RechercheFichiers::CopierFichier(
    const fs::path& fichier
)
{
    try
    {
        // ----------------------------------------------------
        // Calcul du chemin relatif
        // ----------------------------------------------------

        fs::path cheminRelatif =
            fs::relative(
                fichier,
                cheminDeDepart
            );


        // ----------------------------------------------------
        // Destination
        // ----------------------------------------------------

        fs::path destination =
            dossierCopie / cheminRelatif;


        // ----------------------------------------------------
        // Création des dossiers
        // ----------------------------------------------------

        fs::create_directories(
            destination.parent_path()
        );


        // ----------------------------------------------------
        // Copie
        // ----------------------------------------------------

        fs::copy_file(
            fichier,
            destination,
            fs::copy_options::overwrite_existing
        );


        nombreFichiersCopies++;


        // ----------------------------------------------------
        // Console
        // ----------------------------------------------------

        if (activerCout)
        {
            std::cout
                << "[COPIE] "
                << fichier
                << " -> "
                << destination
                << '\n';
        }
    }
    catch (const fs::filesystem_error& e)
    {
        if (activerCout)
        {
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

void RechercheFichiers::ExplorerDossier(
    const fs::path& dossier,
    std::ofstream* fichierSortie,
    const std::string& indentation
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
                // DOSSIER EXCLU
                // ------------------------------------------------

                if (EstDossierExclu(
                        cheminDossier))
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

                    continue;
                }


                // ------------------------------------------------
                // Écriture de l'arborescence
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
                // Récursion
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
                // Écriture dans hierarchie.txt
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

                if (DoitEtreCopie(
                        element.path()))
                {
                    CopierFichier(
                        element.path()
                    );
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


        if (activerCout)
        {
            std::cerr
                << "[ERREUR PARCOURS] "
                << dossier
                << " : "
                << e.what()
                << '\n';
        }
    }
}


// ============================================================
// LANCEMENT DE LA RECHERCHE
// ============================================================

void RechercheFichiers::LancerRecherche()
{
    // ========================================================
    // RESET DES STATISTIQUES
    // ========================================================

    nombreFichiersParcourus = 0;
    nombreFichiersCopies = 0;


    // ========================================================
    // CHRONOMÈTRE
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
        if (activerCout)
        {
            std::cerr
                << "Le dossier de depart n'existe pas : "
                << cheminDeDepart
                << '\n';
        }

        return;
    }


    if (!fs::is_directory(cheminDeDepart))
    {
        if (activerCout)
        {
            std::cerr
                << "Le chemin de depart n'est pas un dossier : "
                << cheminDeDepart
                << '\n';
        }

        return;
    }


    // ========================================================
    // CRÉATION DU DOSSIER COPIED
    // ========================================================

    try
    {
        fs::create_directories(
            dossierCopie
        );
    }
    catch (const fs::filesystem_error& e)
    {
        if (activerCout)
        {
            std::cerr
                << "Impossible de creer copied : "
                << e.what()
                << '\n';
        }

        return;
    }


    // ========================================================
    // FICHIER HIÉRARCHIE
    // ========================================================

    std::ofstream fichierSortie;

    if (activerEcritureHierarchie)
    {
        fichierSortie.open(
            "hierarchie.txt"
        );


        if (!fichierSortie.is_open())
        {
            if (activerCout)
            {
                std::cerr
                    << "Impossible d'ouvrir "
                       "hierarchie.txt\n";
            }

            return;
        }


        fichierSortie
            << "[D] "
            << cheminDeDepart.filename().string()
            << '\n';
    }


    // ========================================================
    // RECHERCHE
    // ========================================================

    ExplorerDossier(
        cheminDeDepart,

        activerEcritureHierarchie
            ? &fichierSortie
            : nullptr,

        "    "
    );


    // ========================================================
    // FERMETURE DU FICHIER
    // ========================================================

    if (activerEcritureHierarchie)
    {
        fichierSortie.close();
    }


    // ========================================================
    // CHRONOMÈTRE
    // ========================================================

    if (activerChrono)
    {
        const auto fin =
            std::chrono::high_resolution_clock::now();


        const std::chrono::duration<double> duree =
            fin - debut;


        if (activerCout)
        {
            std::cout
                << "Temps total : "
                << duree.count()
                << " secondes\n";
        }
    }


    // ========================================================
    // RÉSULTATS
    // ========================================================

    if (activerCout)
    {
        std::cout
            << "\n====================================\n";

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

        std::cout
            << "====================================\n";
    }
}