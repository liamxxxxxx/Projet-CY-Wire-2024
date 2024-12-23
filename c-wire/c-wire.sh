#!/bin/bash


TMP_DIR="tmp"
GRAPH_DIR="graphiques"
FILTERED_FILE="$TMP_DIR/filtered_csv.dat"

#Fonction pour afficher l'aide
display_help() {
    echo "utilisation: $0 <chemin du fichier de données> <type d'usine> <type de consommateur> [ID d'usine] [-h]"
    echo "Options:"
    echo "  <chemin du fichier de données> Chemin d'accès au fichier CSV d'entrée (obligatoire)"
    echo "  <type de centrale> Type de centrale à traiter (obligatoire)"
    echo "                       Valeurs possibles : hvb, hva, lv"
    echo "  <type de consommateur> : Type de consommateur à traiter (obligatoire)"
    echo "                       Valeurs possibles: comp, indiv, all"
    echo "                       Note: 'hvb indiv', 'hvb all', 'hva indiv', 'hva all' sont invalide"
    echo "  [ID de la centrale] ID optionnel de la centrale pour filtrer les résultats (de 1 à 5)"
    echo "  -h                   Afficher cette aide et quitter (prioritaire)"
}

#Vérifie si la combinaison de type de centrale et type de consommateur est invalide

validate_params() {
    local plant_type=$1
    local consumer_type=$2

    if [[ "$plant_type" =~ ^(hvb|hva)$ && "$consumer_type" =~ ^(indiv|all)$ ]]; then
        echo "Erreur : Combinaison invalide entre le type de centrale et le type de consommateur."
        display_help
        exit 1
    fi
}

# Fonction pour trier le fichier de sortie par colonne Capacité

sort_output_file() {
    local output_file="$1"
    local temp_sorted_file="$output_file.sorted"
	# Trie les lignes du fichier de sortie en fonction de la deuxième colonne
    (head -n 1 "$output_file" && tail -n +2 "$output_file" | sort -t':' -k2,2n) > "$temp_sorted_file"
	
    mv "$temp_sorted_file" "$output_file"
}
# Affiche l'aide si l'option -h est présente
if [[ " $@ " =~ " -h " ]]; then
    display_help
    exit 0
fi
# Vérifie le nombre d'arguments
if [[ $# -lt 3 ]]; then
    echo "Erreur : Arguments insuffisants"
    display_help
    exit 1
fi

# Variables pour les paramètres d'entrée
DATA_FILE=$1
PLANT_TYPE=$2
CONSUMER_TYPE=$3
PLANT_ID=$4

# Vérifie si le fichier d'entrée existe
if [[ ! -f "$DATA_FILE" ]]; then
    echo "Erreur : Fichier de données '$DATA_FILE' non trouvé"
    display_help
    exit 1
fi

# Valide les paramètres d'entrée
validate_params "$PLANT_TYPE" "$CONSUMER_TYPE"

# Création des répertoires nécessaires
if [[ ! -d "$TMP_DIR" ]]; then
    mkdir "$TMP_DIR"
else
    rm -rf "$TMP_DIR/*"
fi

if [[ ! -d "$GRAPH_DIR" ]]; then
    mkdir "$GRAPH_DIR"
fi

# Vérifie et compile le programme C si nécessaire
C_PROGRAM="codeC/process_data"
if [[ ! -f "$C_PROGRAM" ]]; then
    echo "Programme C non trouvé, tentative de compilation... Programme C non trouvé, tentative de compilation..."
    make -C codeC/
    if [[ $? -ne 0 ]]; then
        echo "Erreur : Échec de la compilation"
        exit 1
    fi
fi

# Enregistre le temps de début
START_TIME=$(date +%s)

# Filtrage des données si un ID de centrale est fourni
if [[ -n "$PLANT_ID" ]]; then
	
	if [[ -f "$FILTERED_FILE" ]]; then
		
		rm -f "$FILTERED_FILE"
	
	fi
    # Filtre les données en fonction de l'ID de centrale
    awk -F';' -v id="$PLANT_ID" '
        NR == 1 || $1 == id {
            print
        }
    ' "$DATA_FILE" > "$FILTERED_FILE"

    if [[ ! -s "$FILTERED_FILE" ]]; then
        echo "Erreur : Aucune ligne trouvée pour l'ID de la centrale $PLANT_ID"
        exit 1
    fi

    DATA_FILE="$FILTERED_FILE"
fi

# Exécution du programme C avec les paramètres appropriés
if [[ -z "$PLANT_ID" ]]; then
    ./"$C_PROGRAM" "$DATA_FILE" "$PLANT_TYPE" "$CONSUMER_TYPE"
else
    ./"$C_PROGRAM" "$DATA_FILE" "$PLANT_TYPE" "$CONSUMER_TYPE" "$PLANT_ID"
fi
EXIT_CODE=$?

# Vérifie si le traitement a échoué
if [[ $EXIT_CODE -ne 0 ]]; then
    echo "Erreur : Échec du traitement des données"
    echo "Temps de traitement : 0,0 sec"
    exit 1
fi

# Prépare le fichier de sortie
OUTPUT_FILE="tests/${PLANT_TYPE}_${CONSUMER_TYPE}"

if [[ -n "$PLANT_ID" ]]; then
    OUTPUT_FILE="${OUTPUT_FILE}_${PLANT_ID}"
fi
OUTPUT_FILE="${OUTPUT_FILE}.csv"

# Trie le fichier de sortie si disponible

if [[ -f "$OUTPUT_FILE" ]]; then
    echo "Tri du fichier de sortie par la colonne Capacité..."
    sort_output_file "$OUTPUT_FILE"
    echo "Tri terminé : $OUTPUT_FILE"
else
    echo "Erreur : Fichier de sortie $OUTPUT_FILE non trouvé"
    exit 1
fi

# Calcule et affiche le temps de traitement
END_TIME=$(date +%s)
PROCESS_TIME=$(echo "$END_TIME - $START_TIME" | bc)

echo "Temps de traitement : ${PROCESS_TIME}sec"

exit 0
