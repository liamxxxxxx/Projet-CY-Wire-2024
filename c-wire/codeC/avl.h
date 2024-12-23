// Structure d'une station
typedef struct Station {
    int id;
    long long capacity;
    long long consumption;
    int height;
} Station;

// Structure d'un nœud AVL
typedef struct Node {
    Station station;
    struct Node *left;
    struct Node *right;
} Node;

// Fonction pour obtenir la hauteur d'un nœud
int height(Node *N) {
    if (N == NULL)
        return 0;
    return N->station.height;
}

// Fonction pour obtenir un maximum de deux entiers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Fonction pour créer un nouveau nœud AVL
Node *newNode(int id, long long capacity) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {
        perror("Échec de l'allocation de mémoire");
        exit(1);
    }
    node->station.id = id;
    node->station.capacity = capacity;
    node->station.consumption = 0;
    node->station.height = 1; // Le nouveau nœud est initialement à la hauteur 1
    node->left = NULL;
    node->right = NULL;
    return (node);
}

// Rotation à droite du sous-arbre enraciné avec y
Node *rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;

    // Effectuer une rotation
    x->right = y;
    y->left = T2;

    // Mettre à jour les hauteurs
    y->station.height = max(height(y->left), height(y->right)) + 1;
    x->station.height = max(height(x->left), height(x->right)) + 1;

    // Renvoie une nouvelle racine
    return x;
}

// Rotation à gauche du sous-arbre enraciné avec x
Node *leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;

    // Effectuer une rotation
    y->left = x;
    x->right = T2;

    // Mettre à jour les hauteurs
    x->station.height = max(height(x->left), height(x->right)) + 1;
    y->station.height = max(height(y->left), height(y->right)) + 1;

    // Renvoie une nouvelle racine
    return y;
}

// Obtenir le facteur d'équilibre du nœud N
int getBalance(Node *N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}


// Fonction d'insertion AVL
Node *insert(Node *node, int id, long long capacity) {
    if (node == NULL)
        return (newNode(id, capacity));

    if (id < node->station.id)
        node->left = insert(node->left, id, capacity);
    else if (id > node->station.id)
        node->right = insert(node->right, id, capacity);
    else
        return node;

    node->station.height = 1 + max(height(node->left), height(node->right));
	
    int balance = getBalance(node);

    // Cas Gauche-Gauche
    if (balance > 1 && id < node->left->station.id)
        return rightRotate(node);

    // Cas Droite-Droite
    if (balance < -1 && id > node->right->station.id)
        return leftRotate(node);

    // Cas Gauche-Droite
    if (balance > 1 && id > node->left->station.id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Cas Droite-Gauche
    if (balance < -1 && id < node->right->station.id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    /* renvoie le pointeur de nœud (inchangé) */
    return node;
}

Node* search(Node* root, int id) {
    if (root == NULL || root->station.id == id)
        return root;

    if (id < root->station.id)
        return search(root->left, id);

    return search(root->right, id);
}

void inorder(Node *root, FILE *outfile, const char stationType, const char* consumerType) {
    if (root != NULL) {
        inorder(root->left, outfile, stationType, consumerType);
        fprintf(outfile, "%d:%lld:%lld\n", root->station.id, root->station.capacity, root->station.consumption);
        inorder(root->right, outfile, stationType, consumerType);
    }
}

void freeAVLTree(Node* node) {
    if (node == NULL) return;
    freeAVLTree(node->left);
    freeAVLTree(node->right);
    free(node);
}
