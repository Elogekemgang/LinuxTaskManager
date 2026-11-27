#ifndef LTM_TACHES_H
#define LTM_TACHES_H

/*
 * Contrat commun des modules du groupe — application terminal en C17.
 *
 * Chaque membre inclut ce fichier. Les fonctions sont declarees ici et
 * seront definies une seule fois dans les fichiers .c de leur responsable.
 * Les structures ci-dessous sont le modele francais du groupe ; elles ne
 * sont pas des alias de Task / TaskList du moteur existant (task.h).
 */

#include <stddef.h>
#include <stdint.h>

/* Capacites en octets UTF-8, sans compter le zero final des chaines. */
#define TACHE_TITRE_MAX 120U
#define TACHE_DESCRIPTION_MAX 1000U
#define TACHE_DATE_TAILLE 11U /* AAAA-MM-JJ et zero final. */
#define TACHES_MAX 10000U
#define FICHIER_TACHES_PAR_DEFAUT "data/taches.txt"

typedef enum {
    PRIORITE_FAIBLE = 1,
    PRIORITE_MOYENNE = 2,
    PRIORITE_ELEVEE = 3
} Priorite;

typedef enum {
    STATUT_EN_ATTENTE = 0,
    STATUT_EN_COURS = 1,
    STATUT_TERMINEE = 2
} StatutTache;

typedef struct {
    uint64_t id;                /* Unique, positif, non reutilise. */
    int64_t dateCreation;       /* Secondes Unix ; fixee a la creation. */
    int64_t dateModification;   /* Secondes Unix ; derniere modification. */
    Priorite priorite;
    StatutTache statut;
    char dateEcheance[TACHE_DATE_TAILLE]; /* Vide si aucune echeance. */
    char titre[TACHE_TITRE_MAX + 1U];
    char description[TACHE_DESCRIPTION_MAX + 1U];
} Tache;

typedef struct {
    Tache *taches;         /* Tableau dynamique possede par cette liste. */
    size_t nombre;        /* Nombre de cases utilisees. */
    size_t capacite;      /* Nombre de cases allouees. */
    uint64_t prochainId;  /* Compteur a sauvegarder, meme apres suppression. */
} ListeTaches;

typedef enum {
    TACHES_SUCCES = 0,
    TACHES_SANS_CHANGEMENT,
    TACHES_ANNULE,
    TACHES_FIN_SAISIE,
    TACHES_DONNEES_INVALIDES,
    TACHES_INTROUVABLE,
    TACHES_CAPACITE_ATTEINTE,
    TACHES_ERREUR_MEMOIRE,
    TACHES_ERREUR_FICHIER,
    TACHES_FICHIER_INVALIDE,
    TACHES_SAUVEGARDE_INCERTAINE
} ResultatTaches;

/*
 * Regles communes :
 * - Les pointeurs obligatoires sont non NULL et les chaines terminees par 0.
 * - Une liste est initialisee avant tout usage ; nombre <= capacite <= TACHES_MAX.
 * - taches == NULL si et seulement si capacite == 0 ; prochainId commence a 1.
 * - Chaque id est positif et strictement inferieur a prochainId.
 * - Les chaines sont des textes UTF-8 valides, sans controle ni saut de ligne.
 * - Le titre contient du texte visible ; la description peut etre vide.
 * - Une echeance non vide est une vraie date AAAA-MM-JJ (annees 1970 a 9999).
 * - Un identifiant n'est pas un indice : les tris peuvent deplacer les taches.
 * - Les erreurs ou annulations des mutations conservent la liste d'entree.
 * - Les noms de resultats abreges dans les commentaires portent le prefixe
 *   TACHES_ dans le code (exemple : SUCCES signifie TACHES_SUCCES).
 * - Les pointeurs vers ses elements deviennent invalides apres reallocation,
 *   remplacement ou liberation ; apres un tri, rechercher a nouveau par id.
 */

/* Membre 6 : cycle de vie de la liste, a coordonner avec les membres 2 et 5. */

/* Initialise une liste neuve, sans allocation. Ne libere pas un ancien tableau. */
void initialiserListeTaches(ListeTaches *liste);

/* Libere le tableau puis remet la liste dans son etat initial. */
void libererListeTaches(ListeTaches *liste);

/* Membre 2 — ajouter.c et afficher.c. */

/*
 * Saisit titre, description, priorite et echeance dans le terminal.
 * Valide avant insertion, agrandit le tableau si necessaire, attribue
 * prochainId puis l'incremente. Une nouvelle tache est EN_ATTENTE.
 * Retourne CAPACITE_ATTEINTE aussi si prochainId == UINT64_MAX.
 * Ne sauvegarde pas : executerChoix coordonne la transaction complete.
 */
ResultatTaches ajouterTache(ListeTaches *liste);

/* Affiche toutes les taches, ou un message explicite si nombre == 0. */
void afficherTaches(const ListeTaches *liste);

/* Membre 3 — modifier.c et supprimer.c. */

/*
 * Recherche id puis saisit les nouvelles valeurs. Preserve id et dateCreation.
 * Une fiche identique retourne SANS_CHANGEMENT. Ne sauvegarde pas.
 */
ResultatTaches modifierTache(ListeTaches *liste, uint64_t id);

/*
 * Recherche id et demande une confirmation explicite avant suppression.
 * Annulation : ANNULE ; absent : INTROUVABLE. Ne renumerote pas les ids,
 * ne diminue pas prochainId et ne sauvegarde pas.
 */
ResultatTaches supprimerTache(ListeTaches *liste, uint64_t id);

/* Renvoie un indice dans [0, nombre[, ou -1 si l'identifiant est absent. */
ptrdiff_t trouverIndice(const ListeTaches *liste, uint64_t id);

/* Membre 4 — rechercher.c et organiser.c. */

/*
 * Propose une recherche par id ou sous-chaine du titre, puis affiche les
 * correspondances. Casse ASCII ignoree pour le titre, accents conserves.
 * Retourne INTROUVABLE si aucune correspondance, FIN_SAISIE en fin d'entree.
 */
ResultatTaches rechercherTache(const ListeTaches *liste);

/* Echeances croissantes, dates absentes en dernier ; egalites par id croissant. */
void trierParDate(ListeTaches *liste);

/* Priorites elevee, moyenne, faible ; egalites par id croissant. */
void trierParPriorite(ListeTaches *liste);

/* Affiche le statut demande sans retirer les autres donnees de la liste. */
void filtrerParStatut(const ListeTaches *liste, StatutTache statut);

/* Membre 5 — fichiers.c. */

/*
 * Sauvegarde toutes les taches ET prochainId dans le fichier indique.
 * SUCCES signifie que la sauvegarde est confirmee ; ERREUR_FICHIER indique
 * un echec avant publication. Si la publication a eu lieu sans confirmation
 * finale, retourne SAUVEGARDE_INCERTAINE : recharger avant toute repetition.
 * Ne modifie pas la liste. Le format est a definir avec le coordinateur :
 * l'extension .txt ne constitue pas a elle seule un contrat de stockage.
 */
ResultatTaches sauvegarderTaches(const ListeTaches *liste, const char *chemin);

/*
 * Charge et valide les donnees et leur compteur, dans une liste initialisee.
 * Remplace son ancien tableau seulement si le chargement reussit.
 * Un fichier absent donne une liste vide ; un fichier existant invalide
 * retourne FICHIER_INVALIDE et conserve le fichier et la liste d'entree.
 * Gere la fermeture des descripteurs et les allocations qu'elle effectue.
 */
ResultatTaches chargerTaches(ListeTaches *liste, const char *chemin);

/* Membre 6 — main.c et tests.c. */

void afficherMenu(void);

/*
 * Relie le choix du menu aux fonctions ci-dessus ; 0 signifie quitter.
 * Pour ajouter/modifier/supprimer : travailler sur une copie independante,
 * appeler la mutation, puis sauvegarder seulement apres une mutation SUCCES.
 * Remplacer la liste active seulement apres sauvegarde confirmee.
 * Une erreur conserve l'ancienne liste ; une sauvegarde incertaine impose
 * une relecture avant toute nouvelle mutation. Les consultations, recherches,
 * filtres et tris ne demandent aucune reecriture du fichier.
 * Le menu prend aussi en charge le choix et la validation d'un identifiant
 * pour modifier/supprimer, et du statut pour filtrer.
 */
ResultatTaches executerChoix(int choix, ListeTaches *liste, const char *chemin);

/* main() est defini dans main.c ; ce n'est pas une fonction partagee a appeler. */

#endif /* LTM_TACHES_H */
