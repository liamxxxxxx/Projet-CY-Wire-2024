#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "avl.h"

#define MAX_LINE_LENGTH 256

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "utilisation: %s <fichier_d'entrée> <type_de_centrale> <type_de_consommateur> [id_de_centrale]\n", argv[0]);
        return 1;
    }

    char *inputFilename = argv[1];
    char *stationType = argv[2];
    char *consumerType = argv[3];
	int plantId = 0;
	
	if (argc == 5) {
		plantId = atoi(argv[4]);
	}

    Node *root = NULL;
    FILE *infile = fopen(inputFilename, "r");
    if (!infile) {
        perror("Erreur lors de l'ouverture du fichier d'entrée");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    char *token;

    // Lire et traiter le fichier d'entrée
	//Sauter la ligne d'en-tête
    fgets(line, MAX_LINE_LENGTH, infile);

    // Lire et traiter le fichier d'entrée
    while (fgets(line, MAX_LINE_LENGTH, infile) != NULL) {
        char powerPlantStr[20], hvbStr[20], hvaStr[20], lvStr[20], companyStr[20], individualStr[20], capacityStr[50], loadStr[50];
        int powerPlant = -1, hvb = -1, hva = -1, lv = -1, company = -1, individual = -1;
        long long capacity = 0, load = 0;

        // Utilisez strtok pour analyser la ligne CSV
        char *token = strtok(line, ";");
        if (token == NULL) continue; // Gérer les lignes vides potentielles
        strcpy(powerPlantStr, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strcpy(hvbStr, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strcpy(hvaStr, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strcpy(lvStr, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strcpy(companyStr, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strcpy(individualStr, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strcpy(capacityStr, token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strcpy(loadStr, token);

        // Convertir les chaînes en entiers ou en longs, en gérant "-"
        if (isdigit(powerPlantStr[0])) powerPlant = atoi(powerPlantStr);
        if (isdigit(hvbStr[0])) hvb = atoi(hvbStr);
        if (isdigit(hvaStr[0])) hva = atoi(hvaStr);
        if (isdigit(lvStr[0])) lv = atoi(lvStr);
        if (isdigit(companyStr[0])) company = atoi(companyStr);
        if (isdigit(individualStr[0])) individual = atoi(individualStr);
        if (isdigit(capacityStr[0])) capacity = atoll(capacityStr);
        
		if (isdigit(loadStr[0])) load = atoll(loadStr);

        int stationId = -1;
        if (strcmp(stationType, "hvb") == 0 && hvb != -1) stationId = hvb;
        else if (strcmp(stationType, "hva") == 0 && hva != -1) stationId = hva;
        else if (strcmp(stationType, "lv") == 0 && lv != -1) stationId = lv;

        if (stationId != -1) {
            Node* existingNode = search(root, stationId);
            if(capacity > 0) {
                if(existingNode == NULL){
                    root = insert(root, stationId, capacity);
                    existingNode = search(root, stationId);
                }
            }
            if(existingNode != NULL && load > 0) {
                 existingNode->station.consumption += load;
            }
        }

    }
    fclose(infile);

    // Création du nom du fichier de sortie
    char outputFilename[50];
	char formattedFile [50];
	strcpy(outputFilename, "tests/");
	if (plantId == 0) {
		snprintf(formattedFile, 50, "%s_%s.csv", stationType, consumerType);
	} else {
		snprintf(formattedFile, 50, "%s_%s_%d.csv", stationType, consumerType, plantId);
	}
	strcat(outputFilename, formattedFile);
    FILE *outfile = fopen(outputFilename, "w");
    if (!outfile) {
        perror("Erreur lors de l'ouverture du fichier de sortie");
        freeAVLTree(root);
        return 1;
    }

    fprintf(outfile, "%s:Capacité:%s\n", stationType, consumerType);
    inorder(root, outfile, stationType, consumerType);
    fclose(outfile);

    freeAVLTree(root);
	return 0;
}
