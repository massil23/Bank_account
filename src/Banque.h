/**
 * INF-3173-40 - Principes des systèmes d'exploitation
 * Travail pratique #0: Verrouillage de fichiers
 * 
 * Fichier d'en-tête pour le gestionnaire de comptes bancaires
 */

 #ifndef BANQUE_H
 #define BANQUE_H
 

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <errno.h>
 #include <stdbool.h>
 #include <strings.h> // Pour strcasecmp
 
 /* Si les constantes de verrouillage ne sont pas définies, les définir manuellement */
 #ifndef F_ULOCK
 #define F_ULOCK 0    /* Déverrouiller une section précédemment verrouillée */
 #endif
 
 #ifndef F_LOCK
 #define F_LOCK 1     /* Verrouiller une section (bloquant) */
 #endif
 
 #ifndef F_TLOCK
 #define F_TLOCK 2    /* Tester et verrouiller une section (non bloquant) */
 #endif
 
 #ifndef F_TEST
 #define F_TEST 3     /* Tester une section pour d'autres verrous */
 #endif
 
 // Définition des constantes
 #define MAX_NAME_LENGTH 50
 #define MAX_LINE_LENGTH 256
 #define ACCOUNT_TYPE_LENGTH 10
 
 // Types de comptes
 typedef enum {
     CHEQUE,
     EPARGNE,
     UNKNOWN
 } AccountType;
 
 // Structure d'un compte bancaire
 typedef struct {
     int code;
     char nom[MAX_NAME_LENGTH];
     char prenom[MAX_NAME_LENGTH];
     AccountType type;
     double solde;
     off_t position;      // Position dans le fichier
     size_t size;         // Taille de l'enregistrement (ligne)
     off_t solde_pos;     // Position du solde dans la ligne
     size_t solde_len;    // Longueur de la chaîne du solde
 } Account;
 
 // Prototypes des fonctions
 void display_menu();
 int open_account_file(const char* filename, int* fd);
 void consult_account(int fd);
 void withdraw_from_account(int fd);
 void deposit_to_account(int fd);
 void delete_account(int fd);
 void display_attribute(int fd);
 int find_account_by_code(int fd, int code, Account* account);
 void print_account(Account* account);
 int lock_record(int fd, off_t position, size_t size);
 int lock_attribute(int fd, off_t position, size_t attribute_size);
 int lock_file(int fd);
 int unlock(int fd, off_t position, size_t size);
 AccountType string_to_account_type(const char* str);
 const char* account_type_to_string(AccountType type);
 int parse_account_line(const char* line, Account* account);
 int write_account_to_file(int fd, Account* account);
 int update_account_solde(int fd, Account* account);
 
 #endif /* BANQUE_H */