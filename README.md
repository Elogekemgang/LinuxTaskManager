# LinuxTaskManager — Gestionnaire de tâches sous Linux

Projet de **Programmation Système**, encadré par **Mme NOUHO**, réalisé en groupe de six membres. Présentation prévue le **jeudi 17 décembre 2026**. Livrable demandé : **rapport numérique au format PDF**.

L’application permet de gérer les tâches quotidiennes depuis un **terminal Linux** : créer une tâche, consulter la liste, modifier ses informations, la supprimer et organiser son travail. Les données sont conservées dans des fichiers entre deux lancements.

## Périmètre du projet

Le projet collectif utilise une interface en ligne de commande, avec un menu interactif en français. Une tâche représente une activité personnelle, comme préparer un rapport ; elle ne représente pas un processus du système Linux.

| Fonctionnalité | Comportement attendu |
|---|---|
| Ajouter | Saisir un titre, une description facultative, une priorité et une échéance facultative. |
| Consulter | Afficher les tâches et signaler une liste vide. |
| Modifier | Retrouver une tâche par identifiant et mettre à jour ses informations. |
| Supprimer | Demander confirmation puis retirer la tâche sélectionnée. |
| Rechercher | Retrouver les tâches par titre ou identifiant. |
| Organiser | Trier par date ou priorité et filtrer par statut. |
| Sauvegarder | Enregistrer les changements et retrouver les données au prochain lancement. |
| Gérer les erreurs | Signaler les saisies invalides et les problèmes de fichiers. |

La version terminal existante propose également des statistiques. Leur prise en charge dans la répartition du groupe reste à convenir.

Les fichiers GTK et leur tutoriel présents dans ce dépôt correspondent à un **exercice personnel distinct du livrable collectif**. Ils ne sont pas nécessaires pour compiler ou utiliser la version terminal.

## Répartition du travail

Les noms et prénoms sont à renseigner par le groupe.

| Membre | Responsabilité | Fichiers prévus |
|---|---|---|
| 1 — Architecture et coordination | Définir les structures, les prototypes et les conventions communes ; vérifier la compatibilité et suivre l’avancement. | `taches.h`, `README.md` |
| 2 — Ajout et affichage | Saisie des informations, ajout et affichage des tâches, message de liste vide. | `ajouter.c`, `afficher.c` |
| 3 — Modification et suppression | Sélection par identifiant, modification, suppression confirmée et recherche d’indice. | `modifier.c`, `supprimer.c` |
| 4 — Recherche et organisation | Recherche par titre ou identifiant, tri par date ou priorité et filtre par statut. | `rechercher.c`, `organiser.c` |
| 5 — Fichiers et sauvegarde | Chargement, enregistrement, persistance et gestion des erreurs d’entrées-sorties. | `fichiers.c`, fichier de données |
| 6 — Menu, intégration et tests | Menu principal, assemblage des modules, compilation et vérification de l’ensemble. | `main.c`, `tests.c` |

Le membre 1 fixe les contrats communs avec les responsables des fonctions. Le membre 6 réalise l’intégration technique en s’appuyant sur ces contrats. Chaque membre teste sa contribution avant de la transmettre.

## Organisation des fichiers_Yoya_

### Organisation cible du groupe

Cette arborescence décrit le découpage prévu par la répartition ; les modules français ne sont pas encore créés dans le dépôt actuel.

```text
LinuxTaskManager/
├── include/
│   └── taches.h          # Types, constantes et prototypes communs
├── src/
│   ├── main.c            # Menu et exécution des choix
│   ├── ajouter.c
│   ├── afficher.c
│   ├── modifier.c
│   ├── supprimer.c
│   ├── rechercher.c
│   ├── organiser.c
│   └── fichiers.c
├── tests/
│   └── tests.c           # Vérifications d’intégration
├── data/
│   └── taches.txt        # Nom prévu dans la répartition
├── docs/                 # Conception, rapport et préparation de l’oral
├── Makefile
├── .gitignore
└── README.md
```

### Sources actuellement disponibles

La version terminal déjà réalisée utilise les fichiers suivants :

| Emplacement | Rôle actuel |
|---|---|
| [`include/task.h`](include/task.h) | Structures `Task`, `TaskList`, priorités, statuts et fonctions métier. |
| [`include/storage.h`](include/storage.h) | Contrats du stockage et codes de résultat. |
| `src/task.c` | Validation, recherche, tri et statistiques. |
| `src/storage.c` | Lecture, écriture, verrouillage et publication des fichiers. |
| `src/menu.c`, `src/main.c` | Menu interactif et commandes du terminal. |
| `src/utils.c` | Fonctions utilitaires. |
| `tests/` | Tests automatisés du moteur et des commandes. |

Le groupe doit harmoniser les noms et le découpage avant de remplacer ces modules. Le Makefile devra être adapté lors de cette réorganisation. Les commandes ci-dessous s’appliquent aux sources actuelles.

## Compiler et lancer

Prérequis : **Linux, GCC et GNU Make**. **Python 3** est nécessaire pour les tests d’intégration et la démonstration automatisée.

Depuis la racine du dépôt :

```sh
make
./taskmanager
```

La deuxième commande ouvre le menu interactif. Pour consulter l’aide ou utiliser un dossier de données distinct :

```sh
./taskmanager --help
./taskmanager --data-dir /tmp/ltm-exercice
```

Le dossier par défaut est `./data`, relatif au répertoire courant. Utiliser le même chemin absolu avec `--data-dir` pour retrouver la même base depuis plusieurs terminaux. Aucun lancement avec `sudo` n’est nécessaire.

### Exemples de commandes

Ces exemples utilisent un dossier d’exercice commun et supposent une base initialement vide :

```sh
./taskmanager --data-dir /tmp/ltm-exercice add \
    --title "Préparer le rapport" --priority elevee --due 2026-12-17
./taskmanager --data-dir /tmp/ltm-exercice list
./taskmanager --data-dir /tmp/ltm-exercice edit 1 --status en-cours
./taskmanager --data-dir /tmp/ltm-exercice done 1
./taskmanager --data-dir /tmp/ltm-exercice stats
./taskmanager --data-dir /tmp/ltm-exercice delete 1 --yes
```

L’identifiant `1` correspond à la première création dans une base neuve. Pour des données déjà présentes, utiliser l’identifiant affiché par la liste.

## Règles communes et fichier taches.h

Le membre 1 prépare le fichier partagé `include/taches.h` avec le groupe. Celui-ci doit définir la structure `Tache`, les types associés, les constantes et les prototypes publics. Les modules qui suivent ce découpage incluront ce même en-tête :

```c
#include "taches.h"
```

Dans le code existant, ces définitions sont réparties entre les en-têtes de `include/`. La structure commune s’appelle actuellement `Task`. La création de `taches.h` doit réutiliser un modèle cohérent avec les autres modules.

### Données et conventions à respecter

- **Titre** : obligatoire, au plus 120 octets UTF-8.
- **Description** : facultative, au plus 1 000 octets UTF-8 ; une seule ligne.
- **Priorité** : faible = `1`, moyenne = `2`, élevée = `3`.
- **Statut** : en attente, affiché « À faire » dans la CLI actuelle = `0` ; en cours = `1` ; terminée = `2`.
- **Dates** : distinguer la création de l’échéance ; l’échéance utilise `AAAA-MM-JJ` et peut rester vide.
- **Identifiant** : unique, persistant et non réutilisé après suppression ; il ne correspond pas nécessairement à l’indice dans le tableau.
- **Suppression** : confirmation obligatoire ; les identifiants restants sont conservés.
- **Fonctions publiques** : convenir des paramètres, du type de retour, des données modifiées et des erreurs signalées avant l’implémentation.

### Fonctions prévues par la répartition

Les noms ci-dessous constituent la convention cible. Les signatures complètes restent à définir dans `taches.h` avec les responsables concernés.

| Responsable | Fonctions |
|---|---|
| Membre 2 | `ajouterTache`, `afficherTaches` |
| Membre 3 | `modifierTache`, `supprimerTache`, `trouverIndice` |
| Membre 4 | `rechercherTache`, `trierParDate`, `trierParPriorite`, `filtrerParStatut` |
| Membre 5 | `sauvegarderTaches`, `chargerTaches` |
| Membre 6 | `main`, `afficherMenu`, `executerChoix` |

Le groupe doit notamment préciser quelle fonction attribue les identifiants et à quel moment une mutation déclenche sa sauvegarde.

## Stockage et programmation système

La répartition prévoit un fichier nommé `taches.txt`. **La version actuelle utilise un format binaire explicite dans `data/tasks.dat`, accompagné de `data/tasks.lock`.** Le membre 5 et le coordinateur doivent fixer le format commun et adapter ensemble lecture, écriture et documentation. Changer uniquement l’extension ne convertit pas les données.

Le travail système couvre notamment `open()` / `openat()`, `read()`, `write()`, `close()`, `lseek()`, `stat()` / `fstat()`, `mkdir()`, `fcntl()` et le nettoyage par `unlinkat()`. Les erreurs sont vérifiées et remontées à l’interface avec leur contexte, en utilisant notamment `errno`, `strerror()` et `perror()`.

Dans le moteur existant, les écritures utilisent un verrou, une relecture, un temporaire, `fsync()` et un remplacement par `renameat()`. Les fichiers invalides sont signalés sans être réinitialisés automatiquement. Le fichier `tasks.lock` reste en place pendant l’utilisation.

Les données personnelles, fichiers temporaires et exécutables ne doivent pas être ajoutés au dépôt Git. Une base de données personnelle ne remplace pas une sauvegarde de secours.

## Tests et validation

Pour vérifier les sources actuelles :

```sh
make test
make analyze
```

`make test` exécute les tests C et les scénarios d’intégration Python. `make analyze` utilise l’analyseur statique GCC avec les avertissements traités en erreurs. Ces commandes ne nécessitent pas le SDK GTK.

Avant de transmettre sa contribution, chaque membre doit vérifier les cas qui le concernent :

- ajout normal, titre vide et saisie trop longue ;
- affichage d’une liste vide ou contenant plusieurs tâches ;
- modification et suppression avec identifiant existant ou absent ;
- annulation d’une suppression ;
- recherche présente ou absente, tris et filtres ;
- fermeture puis relancement avec les mêmes données ;
- erreurs de lecture ou d’écriture, sans perte silencieuse des données.

Pour une démonstration automatique sur une base temporaire :

```sh
make demo
```

## Collaboration avec Git et GitHub

1. Le coordinateur prépare le dépôt commun et les définitions partagées.
2. Chaque membre crée une branche pour sa contribution, par exemple `membre-2-ajout-affichage`.
3. Il compile et teste son travail avant de le proposer.
4. Il ouvre une *pull request* décrivant les changements et les vérifications effectuées.
5. Le coordinateur vérifie les contrats ; le membre 6 contrôle l’intégration avec les autres modules.
6. Les changements vérifiés sont fusionnés dans la branche principale.

Exemple pour commencer une contribution :

```sh
git switch -c membre-2-ajout-affichage
```

Utiliser quatre espaces d’indentation, des noms explicites et les noms de fonctions convenus. Écrire des messages de commit courts, comme `Ajouter la recherche par titre`. Toute modification de `taches.h` doit être communiquée aux membres concernés.

À chaque point d’avancement, partager ce qui est terminé, ce qui reste à faire et les blocages. Le groupe doit produire un seul logiciel fonctionnel dont chaque membre peut expliquer sa contribution.

## Rapport et présentation

Le rapport PDF doit présenter le besoin, l’architecture retenue, les structures de données, les fonctions, les appels système, les tests et les limites connues. Les documents de travail sont regroupés dans `docs/` ; adapter les supports existants au périmètre terminal du groupe avant la remise.

Chaque membre prépare une explication de sa partie et une démonstration de son fonctionnement. Les identités, la classe et l’établissement doivent être complétés par le groupe.
