/**
 * INF-3173-40 - Principes des systèmes d'exploitation
 * Travail pratique #0: Verrouillage de fichiers
 * 
 * Gestionnaire de comptes bancaires avec différents niveaux de verrouillage
 * Version avec fichier texte
 * 
 * @author MASSIL ZADOUD
 * @date 29 mai 2025
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
             
             int c;
             while ((c = getchar()) != '\n' && c != EOF);
             printf("Choix invalide. Veuillez réessayer.\n");
             continue;
         }
         
         
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
 * Convertit une chaîne de caractères en type de compte énuméré.
 * 
 * @param str Chaîne de caractères représentant le type de compte
 *            (accepte "Cheque", "Chèque", "Epargne", "Épargne")
 * @return AccountType correspondant (CHEQUE, EPARGNE, ou UNKNOWN si non reconnu)
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
 * Convertit un type de compte énuméré en chaîne de caractères.
 * 
 * @param type Type de compte énuméré (CHEQUE, EPARGNE, ou autre)
 * @return Pointeur vers une chaîne constante représentant le type
 *         ("Chèque", "Épargne", ou "Inconnu" si type non reconnu)
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
 * Analyse une ligne de fichier texte et remplit une structure Account.
 * Format attendu : code,nom,prenom,type,solde
 * 
 * @param line Chaîne de caractères représentant une ligne du fichier texte
 * @param account Pointeur vers la structure Account à remplir
 * @return 0 en cas de succès, -1 si le format de la ligne est invalide
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
 * Affiche le menu principal du gestionnaire de comptes bancaires.
 * Présente les 6 options disponibles à l'utilisateur.
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
 * Ouvre le fichier de comptes bancaires en mode lecture/écriture.
 * Crée le fichier s'il n'existe pas avec les permissions 644.
 * 
 * @param filename Nom du fichier de comptes à ouvrir
 * @param fd Pointeur vers l'entier qui recevra le descripteur de fichier
 * @return 0 en cas de succès, -1 en cas d'erreur
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
 * Consulte et affiche les détails complets d'un compte bancaire.
 * Aucun verrouillage n'est appliqué pour cette opération de lecture seule,
 * mais vérifie si le fichier entier est verrouillé par une autre opération.
 * 
 * @param fd Descripteur du fichier de comptes ouvert
 */
void consult_account(int fd) {
    lseek(fd, 0, SEEK_SET);
    if (lockf(fd, F_TEST, 0) == -1) {
        printf("Le fichier est verrouillé veuillez essayer plus tard\n");
        return;
    }
    
    int code;
    printf("Entrez le code du compte: ");
    scanf("%d", &code);
    while (getchar() != '\n'); 
    
    Account account;
    if (find_account_by_code(fd, code, &account) == 0) {
        print_account(&account);
    } else {
        printf("Compte %d non trouvé.\n", code);
    }
}
 
 /**
 * Effectue un retrait d'un montant spécifié sur un compte bancaire.
 * Applique un verrouillage sur l'enregistrement complet (ligne entière) pendant
 * la durée de l'opération pour éviter les accès concurrents.
 * Vérifie la validité du montant et la disponibilité des fonds.
 * 
 * @param fd Descripteur du fichier de comptes ouvert
 */
void withdraw_from_account(int fd) {
    lseek(fd, 0, SEEK_SET);
    if (lockf(fd, F_TEST, 0) == -1) {
        printf("Le fichier est verrouillé veuillez essayer plus tard\n");
        return;
    }
    
    int code;
    double amount;
    
    printf("Entrez le code du compte: ");
    scanf("%d", &code);
    while (getchar() != '\n'); 
    
    Account account;
    if (find_account_by_code(fd, code, &account) != 0) {
        printf("Compte %d non trouvé.\n", code);
        return;
    }
    
    
    if (lock_record(fd, account.position, account.size) != 0) {
        printf("L'enregistrement que vous voulez accéder est verrouillé\n");
        return;
    }
    
    printf("Solde actuel: %.2f$\n", account.solde);
    printf("Entrez le montant à retirer: ");
    scanf("%lf", &amount);
    while (getchar() != '\n'); 
    
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
    
    
    if (update_account_solde(fd, &account) != 0) {
        printf("Erreur lors de la mise à jour du compte.\n");
        unlock(fd, account.position, account.size);
        return;
    }
    
    printf("\n# code nom prénom solde\n");
    printf("%d %s %s %.2f$\n", account.code, account.nom, account.prenom, account.solde);
    
    
    unlock(fd, account.position, account.size);
}
 
 /**
 * Effectue un dépôt d'un montant spécifié sur un compte bancaire.
 * Applique un verrouillage uniquement sur l'attribut solde, permettant
 * l'accès simultané aux autres attributs (nom, prénom) du compte.
 * Vérifie la validité du montant avant d'effectuer l'opération.
 * 
 * @param fd Descripteur du fichier de comptes ouvert
 */
void deposit_to_account(int fd) {
    lseek(fd, 0, SEEK_SET);
    if (lockf(fd, F_TEST, 0) == -1) {
        printf("Le fichier est verrouillé veuillez essayer plus tard\n");
        return;
    }
    
    int code;
    double amount;
    
    printf("Entrez le code du compte: ");
    scanf("%d", &code);
    while (getchar() != '\n');
    
    Account account;
    if (find_account_by_code(fd, code, &account) != 0) {
        printf("Compte %d non trouvé.\n", code);
        return;
    }
    
    
    if (account.solde_pos == -1) {
        printf("Erreur: position du solde non trouvée.\n");
        return;
    }
    
    off_t solde_offset = account.position + account.solde_pos;
    
    
    if (lock_attribute(fd, solde_offset, account.solde_len) != 0) {
        printf("Le solde que vous voulez accéder est verrouillé\n");
        return;
    }
    
    printf("Solde actuel: %.2f$\n", account.solde);
    printf("Entrez le montant à déposer: ");
    scanf("%lf", &amount);
    while (getchar() != '\n');
    
    if (amount <= 0) {
        printf("Le montant doit être positif.\n");
        unlock(fd, solde_offset, account.solde_len);
        return;
    }
    
    account.solde += amount;
    
    
    if (update_account_solde(fd, &account) != 0) {
        printf("Erreur lors de la mise à jour du compte.\n");
        unlock(fd, solde_offset, account.solde_len);
        return;
    }
    
    printf("\n# code nom prénom solde\n");
    printf("%d %s %s %.2f$\n", account.code, account.nom, account.prenom, account.solde);
    
    
    unlock(fd, solde_offset, account.solde_len);
}
 
 /**
 * Met à jour le solde d'un compte dans le fichier en réécrivant la ligne complète.
 * Reconstruit la ligne au format CSV avec les nouvelles données du compte
 * et l'écrit à la position correspondante dans le fichier.
 * 
 * @param fd Descripteur du fichier de comptes ouvert
 * @param account Pointeur vers la structure Account contenant les données mises à jour
 * @return 0 en cas de succès, -1 en cas d'erreur d'écriture
 */
 int update_account_solde(int fd, Account* account) {
     char buffer[MAX_LINE_LENGTH];
     
     
     snprintf(buffer, sizeof(buffer), "%d,%s,%s,%s,%.2f\n", 
              account->code, 
              account->nom, 
              account->prenom, 
              account_type_to_string(account->type), 
              account->solde);
     
     
     lseek(fd, account->position, SEEK_SET);
     
     
     if (write(fd, buffer, strlen(buffer)) != (ssize_t)strlen(buffer)) {
         return -1;
     }
     
     return 0;
 }
 
 /**
 * Écrit un nouveau compte dans le fichier à la position courante du curseur.
 * Formate les données du compte au format CSV et les ajoute au fichier.
 * Utilisée pour créer de nouveaux comptes ou initialiser le fichier.
 * 
 * @param fd Descripteur du fichier de comptes ouvert
 * @param account Pointeur vers la structure Account à écrire
 * @return 0 en cas de succès, -1 en cas d'erreur d'écriture
 */
 int write_account_to_file(int fd, Account* account) {
     char buffer[MAX_LINE_LENGTH];
     
     
     snprintf(buffer, sizeof(buffer), "%d,%s,%s,%s,%.2f\n", 
              account->code, 
              account->nom, 
              account->prenom, 
              account_type_to_string(account->type), 
              account->solde);
     
     
     if (write(fd, buffer, strlen(buffer)) != (ssize_t)strlen(buffer)) {
         return -1;
     }
     
     return 0;
 }
 
 /**
 * Supprime un compte bancaire en appliquant un verrouillage sur le fichier entier.
 * Marque le compte comme supprimé en remplaçant sa ligne par un marqueur "#DELETED".
 * Cette opération critique nécessite un accès exclusif à l'ensemble du fichier
 * pour garantir l'intégrité des données pendant la suppression.
 * 
 * @param fd Descripteur du fichier de comptes ouvert
 */
 void delete_account(int fd) {
     int code;
     
     
     if (lock_file(fd) != 0) {
         printf("Le fichier est verrouillé veuillez essayer plus tard\n");
         return;
     }
     
     printf("Entrez le code du compte à détruire: ");
     scanf("%d", &code);
     while (getchar() != '\n'); 
     
     
     
     printf("Le compte %d est détruit.\n", code);
     
    
     
     Account account;
     if (find_account_by_code(fd, code, &account) != 0) {
         printf("Compte %d non trouvé.\n", code);
         unlock(fd, 0, 0); 
         return;
     }
     
     
     lseek(fd, account.position, SEEK_SET);
     char deleted_marker[MAX_LINE_LENGTH];
     snprintf(deleted_marker, sizeof(deleted_marker), "#DELETED,%d\n", code);
     write(fd, deleted_marker, strlen(deleted_marker));
     
     
     unlock(fd, 0, 0);
 }
 
 /**
 * Affiche un attribut spécifique d'un compte bancaire (nom, prénom ou solde).
 * Vérifie les verrouillages appropriés selon l'attribut demandé :
 * - Nom et prénom : accès libre (sauf si fichier entier verrouillé)
 * - Solde : vérifie si l'attribut solde est spécifiquement verrouillé
 * 
 * @param fd Descripteur du fichier de comptes ouvert
 */
void display_attribute(int fd) {
    
    lseek(fd, 0, SEEK_SET);
    if (lockf(fd, F_TEST, 0) == -1) {
        printf("Le fichier est verrouillé veuillez essayer plus tard\n");
        return;
    }
    
    int code;
    int attribute_choice;
    
    printf("Entrez le code du compte: ");
    scanf("%d", &code);
    while (getchar() != '\n');
    
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
    while (getchar() != '\n');
    
    switch (attribute_choice) {
        case 1:
            printf("Nom: %s\n", account.nom);
            break;
        case 2:
            printf("Prénom: %s\n", account.prenom);
            break;
        case 3:
            
            if (account.solde_pos == -1) {
                printf("Erreur: position du solde non trouvée.\n");
                return;
            }
            
            off_t solde_offset = account.position + account.solde_pos;
            
            
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
 * Recherche un compte bancaire par son code unique dans le fichier.
 * Lit le fichier ligne par ligne, ignore les lignes de commentaires (commençant par #)
 * et remplit la structure Account avec les données du compte trouvé ainsi que
 * sa position et sa taille dans le fichier pour les opérations de verrouillage.
 * 
 * @param fd Descripteur du fichier de comptes ouvert
 * @param code Code unique du compte à rechercher
 * @param account Pointeur vers la structure Account à remplir si trouvé
 * @return 0 si le compte est trouvé, -1 si non trouvé
 */
 int find_account_by_code(int fd, int code, Account* account) {
     char line[MAX_LINE_LENGTH];
     off_t position = 0;
     
     
     lseek(fd, 0, SEEK_SET);
     
     
     int bytes_read;
     char buffer[1];
     int index = 0;
     
     while ((bytes_read = read(fd, buffer, 1)) > 0) {
         if (buffer[0] == '\n' || index >= MAX_LINE_LENGTH - 1) {
             
             line[index] = '\0';
             
             
             if (line[0] != '#' && parse_account_line(line, account) == 0) {
                 if (account->code == code) {
                     account->position = position;
                     account->size = index + 1; 
                     return 0; 
                 }
             }
             
             position += index + 1; 
             index = 0;
         } else {
             line[index++] = buffer[0];
         }
     }
     
     return -1;
 }
 
 /**
 * Affiche de manière formatée toutes les informations d'un compte bancaire.
 * Présente les données selon le format spécifié dans l'énoncé avec
 * un en-tête descriptif et le type de compte sur une ligne séparée.
 * 
 * @param account Pointeur vers la structure Account à afficher
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
 * Verrouille un enregistrement complet (ligne entière) dans le fichier.
 * Utilise un verrouillage non-bloquant (F_TLOCK) pour éviter d'attendre
 * indéfiniment si la section est déjà verrouillée par un autre processus.
 * Utilisé pour les opérations de retrait qui nécessitent un accès exclusif à tout l'enregistrement.
 * 
 * @param fd Descripteur du fichier ouvert
 * @param position Position de début de l'enregistrement dans le fichier
 * @param size Taille de l'enregistrement à verrouiller
 * @return 0 en cas de succès, -1 si le verrouillage échoue
 */
 int lock_record(int fd, off_t position, size_t size) {
     lseek(fd, position, SEEK_SET);
     if (lockf(fd, F_TLOCK, size) == -1) {
         return -1; // Échec
     }
     return 0; // Succès
 }
 
 /**
 * Verrouille un attribut spécifique d'un enregistrement dans le fichier.
 * Utilise un verrouillage non-bloquant (F_TLOCK) sur une portion précise du fichier
 * correspondant à un seul attribut.
 * Permet l'accès concurrent aux autres attributs du même enregistrement.
 * Utilisé pour les opérations de dépôt qui modifient uniquement le solde.
 * 
 * @param fd Descripteur du fichier ouvert
 * @param position Position de début de l'attribut dans le fichier
 * @param attribute_size Taille de l'attribut à verrouiller
 * @return 0 en cas de succès, -1 si le verrouillage échoue
 */
 int lock_attribute(int fd, off_t position, size_t attribute_size) {
     lseek(fd, position, SEEK_SET);
     if (lockf(fd, F_TLOCK, attribute_size) == -1) {
         return -1; // Échec
     }
     return 0; // Succès
 }
 
 /**
 * Verrouille l'intégralité du fichier de comptes bancaires.
 * Utilise un verrouillage non-bloquant (F_TLOCK) avec une taille de 0
 * qui signifie "verrouiller jusqu'à la fin du fichier".
 * Empêche tout accès concurrent au fichier pendant les opérations critiques.
 * Utilisé pour les opérations de suppression de comptes qui nécessitent un accès exclusif complet.
 * 
 * @param fd Descripteur du fichier ouvert
 * @return 0 en cas de succès, -1 si le verrouillage échoue
 */
 int lock_file(int fd) {
     lseek(fd, 0, SEEK_SET);
     if (lockf(fd, F_TLOCK, 0) == -1) {
         return -1; // Échec
     }
     return 0; // Succès
 }
 
 /**
 * Déverrouille une section spécifique du fichier précédemment verrouillée.
 * Utilise F_ULOCK pour libérer le verrou sur la zone définie par la position et la taille.
 * Doit être appelée après chaque opération de verrouillage pour permettre
 * l'accès concurrent aux autres processus. Une taille de 0 déverrouille jusqu'à la fin du fichier.
 * 
 * @param fd Descripteur du fichier ouvert
 * @param position Position de début de la section à déverrouiller
 * @param size Taille de la section à déverrouiller (0 = jusqu'à la fin du fichier)
 * @return 0 en cas de succès, -1 si le déverrouillage échoue
 */
 int unlock(int fd, off_t position, size_t size) {
     lseek(fd, position, SEEK_SET);
     if (lockf(fd, F_ULOCK, size) == -1) {
         return -1; // Échec
     }
     return 0; // Succès
 }