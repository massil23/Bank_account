/**
 * INF-3173-40 - Principes des systèmes d'exploitation
 * Travail pratique #0: Verrouillage de fichiers
 * 
 * Gestionnaire de comptes bancaires avec différents niveaux de verrouillage
 * Version avec fichier texte
 */

#include "Banque.h"
#include <fcntl.h>
#include <unistd.h>

 // Main
 int main(int argc, char* argv[]) {
     if (argc != 2) {
         fprintf(stderr, "Usage: %s <fichier_comptes>\n", argv[0]);
         exit(EXIT_FAILURE);
     }
 
     int fd;
     if (open_account_file(argv[1], &fd) != 0) {
         exit(EXIT_FAILURE);
     }
 
     int choice = 0;
     do {
         display_menu();
         printf("Votre choix: ");
         if (scanf("%d", &choice) != 1) {
             // Nettoyer l'entrée
             int c;
             while ((c = getchar()) != '\n' && c != EOF);
             printf("Choix invalide. Veuillez réessayer.\n");
             continue;
         }
         
         // Vider le buffer
         while (getchar() != '\n');
 
         switch (choice) {
             case 1:
                 consult_account(fd);
                 break;
             case 2:
                 withdraw_from_account(fd);
                 break;
             case 3:
                 deposit_to_account(fd);
                 break;
             case 4:
                 delete_account(fd);
                 break;
             case 5:
                 display_attribute(fd);
                 break;
             case 6:
                 printf("Merci d'avoir utilisé notre service bancaire. Au revoir!\n");
                 break;
             default:
                 printf("Option invalide. Veuillez réessayer.\n");
                 break;
         }
     } while (choice != 6);
 
     close(fd);
     return 0;
 }
 
 /**
  * Convertit une chaîne en type de compte
  */
 AccountType string_to_account_type(const char* str) {
     if (strcasecmp(str, "Cheque") == 0 || strcasecmp(str, "Chèque") == 0) {
         return CHEQUE;
     } else if (strcasecmp(str, "Epargne") == 0 || strcasecmp(str, "Épargne") == 0) {
         return EPARGNE;
     } else {
         return UNKNOWN;
     }
 }
 
 /**
  * Convertit un type de compte en chaîne
  */
 const char* account_type_to_string(AccountType type) {
     switch (type) {
         case CHEQUE:
             return "Chèque";
         case EPARGNE:
             return "Épargne";
         default:
             return "Inconnu";
     }
 }
 
 /**
  * Analyse une ligne du fichier et remplit la structure Account
  * Format attendu: code,nom,prenom,type,solde
  * Par exemple: 10,Hamad,Ammar,Chèque,1000
  */
 int parse_account_line(const char* line, Account* account) {
     char type_str[ACCOUNT_TYPE_LENGTH];
     int items_read = sscanf(line, "%d,%[^,],%[^,],%[^,],%lf", 
                            &account->code, 
                            account->nom,
                            account->prenom,
                            type_str,
                            &account->solde);
     
     if (items_read != 5) {
         return -1;
     }
     
     account->type = string_to_account_type(type_str);
 
     // Trouver la position du solde dans la ligne
     char* solde_ptr = strrchr(line, ',');
     if (solde_ptr) {
         account->solde_pos = solde_ptr - line + 1;
         account->solde_len = strlen(solde_ptr + 1);
     } else {
         account->solde_pos = -1;
         account->solde_len = 0;
     }
     
     return 0;
 }
 
 /**
  * Affiche le menu principal
  */
 void display_menu() {
     printf("\n===== Gestionnaire de Comptes Bancaires =====\n");
     printf("1. Consulter compte\n");
     printf("2. Retirer de compte\n");
     printf("3. Déposer au compte\n");
     printf("4. Détruire compte\n");
     printf("5. Afficher un attribut en particulier\n");
     printf("6. Fin\n");
 }
 
 /**
  * Ouvre le fichier des comptes
  */
 int open_account_file(const char* filename, int* fd) {
     *fd = open(filename, O_RDWR | O_CREAT, 0644);
     if (*fd == -1) {
         perror("Erreur lors de l'ouverture du fichier");
         return -1;
     }
     return 0;
 }
 
 /**
 * Consulte les détails d'un compte (aucun verrouillage nécessaire)
 * vérifie si le fichier entier est verrouillé
 */
void consult_account(int fd) {
    // Vérifier si le fichier entier est verrouillé (par une suppression en cours)
    lseek(fd, 0, SEEK_SET);
    if (lockf(fd, F_TEST, 0) == -1) {
        printf("Le fichier est verrouillé veuillez essayer plus tard\n");
        return;
    }
    
    int code;
    printf("Entrez le code du compte: ");
    scanf("%d", &code);
    while (getchar() != '\n'); // Vider le buffer
    
    Account account;
    if (find_account_by_code(fd, code, &account) == 0) {
        print_account(&account);
    } else {
        printf("Compte %d non trouvé.\n", code);
    }
}
 
 /**
 * Retire un montant d'un compte (verrouillage sur l'enregistrement)
 * vérifie d'abord si le fichier entier est verrouillé
 */
void withdraw_from_account(int fd) {
    // Vérifier si le fichier entier est verrouillé (par une suppression en cours)
    lseek(fd, 0, SEEK_SET);
    if (lockf(fd, F_TEST, 0) == -1) {
        printf("Le fichier est verrouillé veuillez essayer plus tard\n");
        return;
    }
    
    int code;
    double amount;
    
    printf("Entrez le code du compte: ");
    scanf("%d", &code);
    while (getchar() != '\n'); // Vider le buffer
    
    Account account;
    if (find_account_by_code(fd, code, &account) != 0) {
        printf("Compte %d non trouvé.\n", code);
        return;
    }
    
    // Verrouiller l'enregistrement
    if (lock_record(fd, account.position, account.size) != 0) {
        printf("L'enregistrement que vous voulez accéder est verrouillé\n");
        return;
    }
    
    printf("Solde actuel: %.2f$\n", account.solde);
    printf("Entrez le montant à retirer: ");
    scanf("%lf", &amount);
    while (getchar() != '\n'); // Vider le buffer
    
    if (amount <= 0) {
        printf("Le montant doit être positif.\n");
        unlock(fd, account.position, account.size);
        return;
    }
    
    if (amount > account.solde) {
        printf("Fonds insuffisants.\n");
        unlock(fd, account.position, account.size);
        return;
    }
    
    account.solde -= amount;
    
    // Mettre à jour le compte dans le fichier
    if (update_account_solde(fd, &account) != 0) {
        printf("Erreur lors de la mise à jour du compte.\n");
        unlock(fd, account.position, account.size);
        return;
    }
    
    printf("\n# code nom prénom solde\n");
    printf("%d %s %s %.2f$\n", account.code, account.nom, account.prenom, account.solde);
    
    // Déverrouiller l'enregistrement
    unlock(fd, account.position, account.size);
}
 
 /**
 * Dépose un montant sur un compte (verrouillage sur l'attribut solde uniquement)
 * MAIS vérifie d'abord si le fichier entier est verrouillé
 */
void deposit_to_account(int fd) {
    // Vérifier si le fichier entier est verrouillé (par une suppression en cours)
    lseek(fd, 0, SEEK_SET);
    if (lockf(fd, F_TEST, 0) == -1) {
        printf("Le fichier est verrouillé veuillez essayer plus tard\n");
        return;
    }
    
    int code;
    double amount;
    
    printf("Entrez le code du compte: ");
    scanf("%d", &code);
    while (getchar() != '\n'); // Vider le buffer
    
    Account account;
    if (find_account_by_code(fd, code, &account) != 0) {
        printf("Compte %d non trouvé.\n", code);
        return;
    }
    
    // Calculer la position et la taille de l'attribut solde
    if (account.solde_pos == -1) {
        printf("Erreur: position du solde non trouvée.\n");
        return;
    }
    
    off_t solde_offset = account.position + account.solde_pos;
    
    // Verrouiller uniquement l'attribut solde
    if (lock_attribute(fd, solde_offset, account.solde_len) != 0) {
        printf("Le solde que vous voulez accéder est verrouillé\n");
        return;
    }
    
    printf("Solde actuel: %.2f$\n", account.solde);
    printf("Entrez le montant à déposer: ");
    scanf("%lf", &amount);
    while (getchar() != '\n'); // Vider le buffer
    
    if (amount <= 0) {
        printf("Le montant doit être positif.\n");
        unlock(fd, solde_offset, account.solde_len);
        return;
    }
    
    account.solde += amount;
    
    // Mettre à jour le solde dans le fichier
    if (update_account_solde(fd, &account) != 0) {
        printf("Erreur lors de la mise à jour du compte.\n");
        unlock(fd, solde_offset, account.solde_len);
        return;
    }
    
    printf("\n# code nom prénom solde\n");
    printf("%d %s %s %.2f$\n", account.code, account.nom, account.prenom, account.solde);
    
    // Déverrouiller le solde
    unlock(fd, solde_offset, account.solde_len);
}
 
 /**
  * Met à jour le solde d'un compte dans le fichier
  */
 int update_account_solde(int fd, Account* account) {
     char buffer[MAX_LINE_LENGTH];
     
     // Recréer la ligne complète
     snprintf(buffer, sizeof(buffer), "%d,%s,%s,%s,%.2f\n", 
              account->code, 
              account->nom, 
              account->prenom, 
              account_type_to_string(account->type), 
              account->solde);
     
     // Positionner le curseur au début de la ligne
     lseek(fd, account->position, SEEK_SET);
     
     // Écrire la nouvelle ligne
     if (write(fd, buffer, strlen(buffer)) != (ssize_t)strlen(buffer)) {
         return -1;
     }
     
     return 0;
 }
 
 /**
  * Écrit un compte dans le fichier
  */
 int write_account_to_file(int fd, Account* account) {
     char buffer[MAX_LINE_LENGTH];
     
     // Créer la ligne
     snprintf(buffer, sizeof(buffer), "%d,%s,%s,%s,%.2f\n", 
              account->code, 
              account->nom, 
              account->prenom, 
              account_type_to_string(account->type), 
              account->solde);
     
     // Écrire la ligne
     if (write(fd, buffer, strlen(buffer)) != (ssize_t)strlen(buffer)) {
         return -1;
     }
     
     return 0;
 }
 
 /**
  * Détruit un compte (verrouillage sur le fichier entier)
  */
 void delete_account(int fd) {
     int code;
     
     // Verrouiller le fichier entier
     if (lock_file(fd) != 0) {
         printf("Le fichier est verrouillé veuillez essayer plus tard\n");
         return;
     }
     
     printf("Entrez le code du compte à détruire: ");
     scanf("%d", &code);
     while (getchar() != '\n'); // Vider le buffer
     
     
     
     printf("Le compte %d est détruit.\n", code);
     
    
     
     Account account;
     if (find_account_by_code(fd, code, &account) != 0) {
         printf("Compte %d non trouvé.\n", code);
         unlock(fd, 0, 0); // Déverrouiller le fichier entier
         return;
     }
     
     
     lseek(fd, account.position, SEEK_SET);
     char deleted_marker[MAX_LINE_LENGTH];
     snprintf(deleted_marker, sizeof(deleted_marker), "#DELETED,%d\n", code);
     write(fd, deleted_marker, strlen(deleted_marker));
     
     // Déverrouiller le fichier
     unlock(fd, 0, 0);
 }
 
 /**
 * Affiche un attribut spécifique d'un compte
 * MAIS vérifie d'abord si le fichier entier est verrouillé
 */
void display_attribute(int fd) {
    // Vérifier si le fichier entier est verrouillé (par une suppression en cours)
    lseek(fd, 0, SEEK_SET);
    if (lockf(fd, F_TEST, 0) == -1) {
        printf("Le fichier est verrouillé veuillez essayer plus tard\n");
        return;
    }
    
    int code;
    int attribute_choice;
    
    printf("Entrez le code du compte: ");
    scanf("%d", &code);
    while (getchar() != '\n'); // Vider le buffer
    
    Account account;
    if (find_account_by_code(fd, code, &account) != 0) {
        printf("Compte %d non trouvé.\n", code);
        return;
    }
    
    printf("Quel attribut voulez-vous afficher?\n");
    printf("1. Nom\n");
    printf("2. Prénom\n");
    printf("3. Solde\n");
    printf("Votre choix: ");
    scanf("%d", &attribute_choice);
    while (getchar() != '\n'); // Vider le buffer
    
    switch (attribute_choice) {
        case 1:
            printf("Nom: %s\n", account.nom);
            break;
        case 2:
            printf("Prénom: %s\n", account.prenom);
            break;
        case 3:
            // Pour le solde, on vérifie d'abord s'il est verrouillé
            if (account.solde_pos == -1) {
                printf("Erreur: position du solde non trouvée.\n");
                return;
            }
            
            off_t solde_offset = account.position + account.solde_pos;
            
            // Essayer un test de verrouillage (F_TEST)
            lseek(fd, solde_offset, SEEK_SET);
            if (lockf(fd, F_TEST, account.solde_len) == -1) {
                printf("Le solde que vous voulez accéder est verrouillé\n");
            } else {
                printf("Solde: %.2f$\n", account.solde);
            }
            break;
        default:
            printf("Choix invalide.\n");
            break;
    }
}
 
 /**
  * Cherche un compte par son code
  * Retourne 0 si trouvé, -1 sinon
  */
 int find_account_by_code(int fd, int code, Account* account) {
     char line[MAX_LINE_LENGTH];
     off_t position = 0;
     
     // Revenir au début du fichier
     lseek(fd, 0, SEEK_SET);
     
     // Lire le fichier ligne par ligne
     int bytes_read;
     char buffer[1];
     int index = 0;
     
     while ((bytes_read = read(fd, buffer, 1)) > 0) {
         if (buffer[0] == '\n' || index >= MAX_LINE_LENGTH - 1) {
             // Fin de la ligne
             line[index] = '\0';
             
             // Ignorer les lignes de commentaire qui commencent par #
             if (line[0] != '#' && parse_account_line(line, account) == 0) {
                 if (account->code == code) {
                     account->position = position;
                     account->size = index + 1; // +1 pour inclure le \n
                     return 0; // Trouvé
                 }
             }
             
             position += index + 1; // +1 pour inclure le \n
             index = 0;
         } else {
             line[index++] = buffer[0];
         }
     }
     
     return -1; // Non trouvé
 }
 
 /**
  * Affiche les détails d'un compte
  */
 void print_account(Account* account) {
     printf("\n# code nom prénom solde\n");
     printf("%d %s %s %.2f$\n", 
            account->code, 
            account->nom, 
            account->prenom, 
            account->solde);
     
     printf("Type de compte: %s\n", 
            account_type_to_string(account->type));
 }
 
 /**
  * Verrouille un enregistrement complet
  */
 int lock_record(int fd, off_t position, size_t size) {
     lseek(fd, position, SEEK_SET);
     if (lockf(fd, F_TLOCK, size) == -1) {
         return -1; // Échec
     }
     return 0; // Succès
 }
 
 /**
  * Verrouille un attribut spécifique
  */
 int lock_attribute(int fd, off_t position, size_t attribute_size) {
     lseek(fd, position, SEEK_SET);
     if (lockf(fd, F_TLOCK, attribute_size) == -1) {
         return -1; // Échec
     }
     return 0; // Succès
 }
 
 /**
  * Verrouille le fichier entier
  */
 int lock_file(int fd) {
     lseek(fd, 0, SEEK_SET);
     if (lockf(fd, F_TLOCK, 0) == -1) {
         return -1; // Échec
     }
     return 0; // Succès
 }
 
 /**
  * Déverrouille une section de fichier
  */
 int unlock(int fd, off_t position, size_t size) {
     lseek(fd, position, SEEK_SET);
     if (lockf(fd, F_ULOCK, size) == -1) {
         return -1; // Échec
     }
     return 0; // Succès
 }