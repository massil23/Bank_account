/**
 * INF-3173-40 - Principes des systèmes d'exploitation
 * Travail pratique #0: Verrouillage de fichiers
 * 
 * Utilitaire pour initialiser le fichier texte de comptes avec des données de test
 *  
 * @author MASSIL ZADOUD
 * @date 29 mai 2025
 */

 #include "Banque.h"

 int main(int argc, char* argv[]) {
     if (argc != 2) {
         fprintf(stderr, "Usage: %s <fichier_comptes>\n", argv[0]);
         exit(EXIT_FAILURE);
     }
 
     // Ouvrir le fichier en mode écriture (remplacer s'il existe déjà)
     int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
     if (fd == -1) {
         perror("Erreur lors de l'ouverture du fichier");
         exit(EXIT_FAILURE);
     }
 
     // Définir les comptes de test
     struct {
         int code;
         char nom[MAX_NAME_LENGTH];
         char prenom[MAX_NAME_LENGTH];
         char type[ACCOUNT_TYPE_LENGTH];
         double solde;
     } test_accounts[] = {
         {10, "Hamad", "Ammar", "Chèque", 1000.0},
         {20, "Zadoud", "Massil", "Épargne", 2500.0},
         {30, "Achek", "Momo", "Chèque", 750.0},
         {40, "Feham", "Ismail", "Épargne", 5000.0},
         {50, "Anes", "Abdel", "Chèque", 1200.0}
     };
 
     int num_accounts = sizeof(test_accounts) / sizeof(test_accounts[0]);
     char buffer[MAX_LINE_LENGTH];
 
     // Écrire les comptes dans le fichier
     for (int i = 0; i < num_accounts; i++) {
         // Formater chaque ligne au format CSV
         snprintf(buffer, sizeof(buffer), "%d,%s,%s,%s,%.2f\n", 
                  test_accounts[i].code,
                  test_accounts[i].nom,
                  test_accounts[i].prenom,
                  test_accounts[i].type,
                  test_accounts[i].solde);
         
         // Écrire la ligne dans le fichier
         ssize_t bytes_written = write(fd, buffer, strlen(buffer));
         if (bytes_written == -1) {
             perror("Erreur lors de l'écriture dans le fichier");
             close(fd);
             exit(EXIT_FAILURE);
         }
     }
 
     close(fd);
     printf("Fichier de comptes initialisé avec %d comptes.\n", num_accounts);
     printf("Format du fichier: code,nom,prenom,type,solde\n");
     
     return 0;
 }