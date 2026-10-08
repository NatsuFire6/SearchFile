#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

class RechercheFichiers
{
public:

    // ========================================================
    // CONSTRUCTEUR
    // ========================================================

    RechercheFichiers();


    // ========================================================
    // CONFIGURATION DES OPTIONS
    // ========================================================

    void ConfigurerOptions(
        bool ecritureHierarchie,
        bool cout,
        bool chrono,
        bool comparaisonNomFichier,
        bool comparaisonExtension,
        bool exclusionDossiers
    );


    // ========================================================
    // CONFIGURATION DES FICHIERS
    // ========================================================

    void ConfigurerFichiers(
        const std::vector<std::string>& nomsFichiers,
        const std::vector<std::string>& extensions,
        const std::vector<std::filesystem::path>& dossiersExclus
    );


    // ========================================================
    // CONFIGURATION DU DOSSIER DE DÉPART
    // ========================================================

    void ConfigurerDossierDepart(
        const std::filesystem::path& chemin
    );

    void ConfigurerDossierCopie(const std::filesystem::path& chemin);
    // ========================================================
    // LANCEMENT DE LA RECHERCHE
    // ========================================================

    void LancerRecherche();


private:

    // ========================================================
    // OPTIONS
    // ========================================================

    bool activerEcritureHierarchie;
    bool activerCout;
    bool activerChrono;

    bool activerComparaisonNomFichier;
    bool activerComparaisonExtension;
    bool activerExclusionDossiers;


    // ========================================================
    // CHEMINS
    // ========================================================

    std::filesystem::path cheminDeDepart;
    std::filesystem::path dossierCopie;


    // ========================================================
    // CRITÈRES DE RECHERCHE
    // ========================================================

    std::vector<std::string> nomsFichiers;
    std::vector<std::string> extensions;
    std::vector<std::filesystem::path> dossiersExclus;


    // ========================================================
    // STATISTIQUES
    // ========================================================

    int nombreFichiersParcourus;
    int nombreFichiersCopies;


    // ========================================================
    // FONCTIONS INTERNES
    // ========================================================

    bool EstDossierExclu(
        const std::filesystem::path& dossier
    );

    bool DoitEtreCopie(
        const std::filesystem::path& fichier
    );

    void CopierFichier(
        const std::filesystem::path& fichier
    );

    void ExplorerDossier(
        const std::filesystem::path& dossier,
        std::ofstream* fichierSortie,
        const std::string& indentation
    );
};