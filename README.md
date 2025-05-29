# Gestionnaire de Comptes Bancaires
## INF-3173-40 - Principes des systèmes d'exploitation
## Travail pratique #0: Verrouillage de fichiers

**Étudiant :** MASSIL ZADOUD  
**Date de remise :** 29 mai 2025

Ce projet implémente un gestionnaire de comptes bancaires qui utilise différents types de verrouillages de fichiers pour garantir l'intégrité des données lors d'accès concurrents.

## Description du projet

Le gestionnaire de comptes bancaires permet de gérer des comptes stockés dans un fichier texte au format CSV. Il utilise l'appel système `lockf()` pour implémenter différents niveaux de verrouillage selon les opérations effectuées, garantissant ainsi la cohérence des données lors d'accès simultanés.

## Structure du projet

```
INF3173_TP0_MASSIL_ZADOUD/
├── .vscode/                # Configuration VSCode
│   ├── c_cpp_properties.json
│   └── settings.json
├── bin/                    # Répertoire des exécutables (créé par make)
│   ├── banque             # Le gestionnaire de comptes
│   └── compte             # L'initialiseur de comptes
├── src/                   # Code source
│   ├── Banque.c          # Implémentation des fonctions de gestion bancaire
│   ├── Banque.h          # Déclarations des structures et fonctions
│   └── compte.c          # Programme pour initialiser le fichier de comptes
├── Makefile              # Pour la compilation du projet
└── README.md             # Documentation du projet
```

## Format du fichier de comptes

Le fichier de comptes est un fichier texte où chaque ligne représente un compte au format CSV :

```
code,nom,prenom,type,solde
```

**Exemple :**
```
10,Hamad,Ammar,Chèque,1000.00
20,Dupont,Jean,Épargne,2500.00
```

## Compilation

Pour compiler le projet, exécutez la commande suivante dans le répertoire racine :

```bash
make
```

Cette commande générera deux exécutables dans le répertoire `bin/` :
- `banque` : Le gestionnaire de comptes bancaires
- `compte` : L'utilitaire pour initialiser le fichier de comptes

Pour nettoyer les fichiers compilés :
```bash
make clean
```

## Utilisation

### 1. Initialisation des comptes

Avant d'utiliser le gestionnaire, vous devez initialiser le fichier de comptes :

```bash
./bin/compte nom_fichier_comptes.txt
```

**Exemple :**
```bash
./bin/compte comptes.txt
```

Cette commande créera un fichier avec 5 comptes de test pré-configurés.

### 2. Lancement du gestionnaire

Exécutez le gestionnaire en spécifiant le fichier de comptes :

```bash
./bin/banque nom_fichier_comptes.txt
```

**Exemple :**
```bash
./bin/banque comptes.txt
```

## Fonctionnalités

Le gestionnaire offre 6 options principales :

1. **Consulter compte** : Affiche les détails complets d'un compte
   - *Verrouillage :* Aucun verrouillage nécessaire

2. **Retirer de compte** : Retire un montant spécifié d'un compte
   - *Verrouillage :* Verrouillage sur l'enregistrement complet (ligne entière)

3. **Déposer au compte** : Dépose un montant spécifié sur un compte
   - *Verrouillage :* Verrouillage uniquement sur l'attribut solde

4. **Détruire compte** : Supprime un compte du système
   - *Verrouillage :* Verrouillage sur le fichier entier

5. **Afficher un attribut** : Affiche un attribut spécifique (Nom, Prénom, ou Solde)
   - *Verrouillage :* Variable selon l'attribut demandé

6. **Fin** : Quitte le programme


## Auteur

**MASSIL ZADOUD**  
Étudiant en INF-3173-40 - Principes des systèmes d'exploitation  
Université du Québec à Montréal (UQAM)  
Session : Hiver 2025