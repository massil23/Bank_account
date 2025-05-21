# INF-3173-40 - Principes des systèmes d'exploitation
# Travail pratique #0: Verrouillage de fichiers
# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
LDFLAGS = 

# Répertoires
SRC_DIR = src
BIN_DIR = bin

# Fichiers source
SRC_BANQUE = $(SRC_DIR)/Banque.c
SRC_COMPTE = $(SRC_DIR)/compte.c

# Exécutables
EXEC_BANQUE = $(BIN_DIR)/banque
EXEC_COMPTE = $(BIN_DIR)/compte

all: $(BIN_DIR) $(EXEC_BANQUE) $(EXEC_COMPTE)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(EXEC_BANQUE): $(SRC_BANQUE)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(EXEC_COMPTE): $(SRC_COMPTE)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean