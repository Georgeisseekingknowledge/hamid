#include <iostream>
#include <cstring>
#include <cstdlib> 
#include <ctime>   
#include <cmath>
#include <fstream> 
using namespace std;

// Enumerations pour la liste des couleurs et liste des formes
enum class Lstform { Square,Triangle, Circle, Diamond};
enum class Lstcoul { Red, Yellow, Green, Blue};

//intTocoul et intToform sont deux fonctions faites pour faciliter à l'utilisateur d'insérer un élément 
//au lieu qu'il entre une chaine il entre juste un entier et on retourne la chaine
Lstcoul intTocoul(int coul) {
    switch (coul) {
        case 1: return Lstcoul::Red;
        case 2: return Lstcoul::Yellow;
        case 3: return Lstcoul::Green;
        case 4: return Lstcoul::Blue;
        default: throw invalid_argument("Invalide couleur");
    }
}

Lstform intToform(int form) {
    switch (form) {
        case 1: return Lstform::Square;
        case 2: return Lstform::Triangle;
        case 3: return Lstform::Circle;
        case 4: return Lstform::Diamond;
        default: throw invalid_argument("Invalide forme");
    }
}
//une pièce doit être intégrée dans trois dimensions 
struct Piece {
    Lstcoul coul;
    Lstform form;
    Piece* next;  // la liste chainée simple ciculaire du plateau cela a travers le pointeur next quand on arrive a tail on fait head = tail->next pour maintenir le cercle de la chaine
    Piece* prevcoul; // la dimension suivante est la chaine doublemenet chainee circulaire cela a travers un pointeur qui pointe a la même pièce mais précédante si elle existe
    Piece* nextcoul; // un pointeur qui pointe a la même pièce mais  suivante si elle existe sinon elle reste initialisé à nullptr dans le constructeur
    Piece* prevform; //même chose pour la chaine doublement circulaire des formes
    Piece* nextform;
    std::string display; // pour représenter une pièce dans terminal
    // Constructeur
    Piece(Lstcoul c, Lstform f) : coul(c), form(f), next(nullptr), prevcoul(nullptr), nextcoul(nullptr), prevform(nullptr), nextform(nullptr), display("") {}
};

// Classe pour le plateau qui contient les pièces
class LstPieces {
private:
    Piece* head;  //la tête de la liste circulaire simple chainee
    int nbPieces;  //nombre de pièces dans la liste
    int score;   // le score du joueur 

public:
    // Constructeur
    LstPieces() : head(nullptr), score(0), nbPieces(0) {
        srand(time(nullptr)); 
    }

    // Destructeur 
    ~LstPieces() {
        while (head) {
            Piece* temp = head;
            head = head->next;
            delete temp;
        }
    }
    
    // pour une couleur et une forme donnée on peut afficher une pièce à l'aide de cette fonction
    std::string DisplayPiece(Lstcoul coul, Lstform form){
    string coulStr;
    string formStr;

    switch (coul) {
        case Lstcoul::Blue:
            coulStr = "\033[34m";
            break;
        case Lstcoul::Yellow:
            coulStr = "\033[33m";
            break;
        case Lstcoul::Red:
            coulStr = "\033[31m";
            break;
        case Lstcoul::Green:
            coulStr = "\033[32m";
    }
    

    switch (form) {
        case Lstform::Square:
            formStr = "Square";
            break;
        case Lstform::Diamond:
            formStr = "Diamond";
            break;
        case Lstform::Circle:
            formStr = "Circle";
            break;
        case Lstform::Triangle:
            formStr = "Triangle";
            break;
    }

    return coulStr  + formStr + "\033[0m";
    }  
    
    
    // fonction qui génére une pièce avec une couleur et forme aléatoire
    Piece* generate(){
        Lstcoul coul = static_cast<Lstcoul>(rand() % 4);
        Lstform form = static_cast<Lstform>(rand() % 4);

        Piece* newP = new Piece(coul, form);
        newP->display = DisplayPiece(coul, form);

        return newP;
    }
    
    // on initialise le plateau par une pièce où tous ces pointeurs pointes vers elle tant qu'il n'existe aucune autre pièce
    void Initialize() {
        Piece* newP = generate();
        newP->next = newP;
        newP->prevform = newP;
        newP->nextform = newP;
        newP->prevcoul = newP;
        newP->nextcoul = newP;

        head = newP;
        score = 0;                 
        nbPieces = 1;                //on ajouté une pièce donc il faut modifier nbPieces
    }
    
    //Retourne la dernière pièce du plateau
    Piece* getTail(){
        Piece* tail = head;
        while (tail->next != head) {
            tail = tail->next;
        }
        return tail;
    }       
    
    // fonction qui insère une pièce dans le plateau selon la direction r pour droit, l pour gauche
    void insert(char direction, Piece* newP) {
        // insérer la nouvelle pièce dans la liste simple chaînée principale
        if (head == nullptr) {
            // si la liste est vide la nouvelle pièce est la seule pièce sur le plateau.
            head = newP;
            newP->next = newP;
        } else {
            // sinon ajouter la nouvelle pièce à la fin de la liste.
            Piece* tail = getTail();
            tail->next = newP;
            newP->next = head;
            nbPieces++;

            // si l'insertion est à gauche, il suffit de mettre à jour la tête c'est l'un des avantages des listes circulaires 
            if (direction == 'l') {
                head = newP;
            }
        }

            // insérer la nouvelle pièce dans la liste doublement chaînée circulaire des formes
            Piece* sameformPiece = newP->next;
            // on parcourt la liste jusqu'à ce qu'on trouve une autre pièce de la même forme.
            while (sameformPiece->form != newP->form) {
                sameformPiece = sameformPiece->next;
            }
            // Si aucune autre pièce de la même forme n'est trouvée la nouvelle pièce pointe vers elle-même.
            if (sameformPiece == newP) {
                newP->prevform = newP;
                newP->nextform = newP;
            } else {
                // Sinon insérer la nouvelle pièce entre prevformPiece et sameformPiece.
                Piece* prevformPiece = sameformPiece->prevform;
                newP->prevform = prevformPiece;
                newP->nextform = sameformPiece;
                prevformPiece->nextform = newP;
                sameformPiece->prevform = newP;
            }

            // de même : insérer la nouvelle pièce dans la liste doublement chaînée circulaire des couleurs.
            Piece* samecoulPiece = newP->next;
            // on parcourt la liste jusqu'à ce que vous trouviez une autre pièce de la même couleur.
            while (samecoulPiece->coul != newP->coul) {
                samecoulPiece = samecoulPiece->next;
            }
            // si aucune autre pièce de la même couleur n'est trouvée,la nouvelle pièce pointe vers elle même.
            if (samecoulPiece == newP) {
                newP->prevcoul = newP;
                newP->nextcoul = newP;
            } else {
                // sinon insérer la nouvelle pièce entre prevcoulPiece et samecoulPiece.
                Piece* prevcoulPiece = samecoulPiece->prevcoul;
                newP->prevcoul = prevcoulPiece;
                newP->nextcoul = samecoulPiece;
                prevcoulPiece->nextcoul = newP;
                samecoulPiece->prevcoul = newP;
            }
    }
    
    // Fonction pour restaurer les pointeurs de forme car après le décalage des couleurs les liens entre les pointeurs de forme se détruit et donc il faut les lier
    void restoreforms(){
         // Tableaux pour stocker les têtes et les queues des listes de formes(tria,gle, square, circle et diamond)
        Piece* heads[4] = {nullptr};
        Piece* tails[4] = {nullptr};
        // Parcourir la liste principale pour récupérer les pièces de même forme
        Piece* current = head;
        do {
            int i = static_cast<int>(current->form);
            // Si la tête de la liste de cette forme est vide, initialiser avec la pièce actuelle
            if (heads[i] == nullptr) {
                heads[i] = current;
                tails[i] = current;
            //sinon ajouter la pièce à la fin de la liste
            } else {
                tails[i]->nextform = current;
                current->prevform = tails[i];
                tails[i] = current;
            }
            current = current->next;
        } while (current != head);
         // et maintenant il faut garantir la circularitée de la liste des formes et donc 
         //pour chaque liste de forme relier la dernière pièce à la première 
  
        for (int i = 0; i < 4; i++) {
            if (heads[i] != nullptr) {
                tails[i]->nextform = heads[i];
                heads[i]->prevform = tails[i];
            }
        }
    }
    
    // Fonction pour déplacer les pièces d'une couleur spécifique vers la gauche
    void shiftcoul(Lstcoul coul){
        // Trouver la première pièce de la couleur spécifiée dans paramètre de shiftcoul
        Piece* firstcoul = head;
        while (firstcoul->next != head) {
            if (firstcoul->coul == coul) {
                break;
            }
            firstcoul = firstcoul->next;
        }
        //si on trouve pas la couleur spécifiée on quitte la boucle
        if (firstcoul->coul != coul) {
            return;
        }
        // Trouver la dernière pièce de la même couleur en exploitant la circularitée de la liste doublement chainée circulaire de couleur
        Piece* lastcoul = firstcoul->prevcoul;

        //si il y a plusieurs pièce que 1 on procède avec le décalage
        if (firstcoul != lastcoul) {
            Piece* current = firstcoul;
            while (current != lastcoul) {

                // à l'aide d'une variable auxilliare tmp on échange les couleurs des pièces consécutives
                Lstform tmp = current->form;
                current->form = current->nextcoul->form;
                current->nextcoul->form = tmp;

                //le même processus pour afficher ce décalage au terminal
                std::string tmp2 = current->display;
                current->display = current->nextcoul->display;
                current->nextcoul->display = tmp2;

                current = current->nextcoul;
            }
        }
        // Après le déplacement, restaurer les pointeurs de forme pour maintenir la structure de données cohérente
        restoreforms();
    }
    
   // Fonction pour restaurer les pointeurs de couleur dans la liste doublement chaînée circulaire
   //c'est la même explication que retoreforms 
    void restorecouls() {
        Piece* heads[4] = {nullptr};
        Piece* tails[4] = {nullptr};

        Piece* current = head;
        do {
            int i = static_cast<int>(current->coul);
            if (heads[i] == nullptr) {
                heads[i] = current;
                tails[i] = current;
            } else {
                tails[i]->nextcoul = current;
                current->prevcoul = tails[i];
                tails[i] = current;
            }
            current = current->next;
        } while (current != head);

        for (int i = 0; i < 4; i++) {
            if (heads[i] != nullptr) {
                tails[i]->nextcoul = heads[i];
                heads[i]->prevcoul = tails[i];
            }
        }
    }
    
    // Fonction pour déplacer les pièces d'une forme spécifique vers la gauche
    //c'est la même explication que retoreforms 
    void shiftform(Lstform form){
        Piece* firstform = head;
        while (firstform->next != head) {
            if (firstform->form == form) {
                break;
            }
            firstform = firstform->next;
        }
        if (firstform->form != form) {
            return;
        }
        Piece* lastform = firstform->prevform;

        if (firstform != lastform) {
            Piece* current = firstform;
            while (current != lastform) {
                Lstcoul tmp = current->coul;
                current->coul = current->nextform->coul;
                current->nextform->coul = tmp;

                std::string tmp2 = current->display;
                current->display = current->nextform->display;
                current->nextform->display = tmp2;

                current = current->nextform;
            }
        }
        restorecouls();
    }
    
    // Fonction pour incrémenter le score prend en paramètre le plateau actuel et la variable decal qui nous dit est ce que l'utilisateur a effectuer un décalage ou non
    void ScoreInc(LstPieces *board, int decal){

      if (decal)
      {
        //si l'utilisateur arrive à effectuer une suppression de 3 pièces à l'aide de décalage il a un bonus on l'ajoute 7 pts
        board->score = board->score + 7;
      }
      else{
        //sinon si il effectue une suppresion à l'aide de l'insertion il a ces 5 points pas de bonus
        board->score = board->score + 5;
      }
   }
    
    // Fonction qui supprime 3 pièces consécutives de même couleur ou forme dans un plateau
    void DeleteTriplets(LstPieces *board, int decal) {
        
    // la fonction ne travaille que si le nombre de pièces dans le plateau est supérieur ou égale à 3
    if (board->nbPieces >= 3) {
    // pour chaque pièce on comptera le nombre de pièce suivantes qui ont la même couleur ou forme et pour cela on utilisera cette fonctions
    int listcoul, listform;
    // pointeur qui va traverser le platau et son son précédent
    Piece *currentPiece = board->head;
    Piece *beforeCurrent = nullptr;
    //variable qui va nous dire quelle compteur est plus grand que l'autre compteur de forme ou de couleur
    int ListLarger=0;
    //après suppression des pieces du plateau on va stocker les pieces dans ce tableau pour les libérer
    Piece *supp[3];
    //dernière pièce de plateau
    Piece *tail = getTail();

    //traverser la liste simplement chainée circulaire du plateau
    while (currentPiece->next != board->head) {
        //initialisation des compteur par 1 en comptant la pièce actuelle
        listcoul = 1; 
        listform = 1;
        Piece* current = board->head->next;
       
        //cette boucle de la pièce actuel compte le nombre de pieces avec couleur ou forme similaires au 2 pièces suivante
        for (int i = 1; i < 3; i++) {
            if (current == board->head){
                break;}
            if (listcoul == i && current->coul == currentPiece->coul){
                 listcoul++;}
            if (listform == i && current->form == currentPiece->form){
                 listform++;}
            current = current->next;
        }
        //on prend le plus grand compteur des deux et en le stock dans ListLarger
        if (listcoul > listform){
            ListLarger = listcoul;
        }else{
            ListLarger = listform;
        }
        // si il existe juste 3 pièces dans le plateau et si ces pièces ont une couleur ou une forme similaire le joueur gagne et on quitte la fonction
        if (board->nbPieces == 3 && ListLarger == 3)
        {
            cout << "game won !";
            return;
        }
        //Si ListLarger == 3 on commence à supprimer les pièces
        if (ListLarger == 3) {
            for (int i = 0; i < 3; i++) {
                // on met jour les pointeurs des pièces adjacentes pour sauter la pièce à supprimer
                currentPiece->prevform->nextform = currentPiece->nextform;
                currentPiece->nextform->prevform = currentPiece->prevform;
                currentPiece->prevcoul->nextcoul = currentPiece->nextcoul;
                currentPiece->nextcoul->prevcoul = currentPiece->prevcoul;
                // Stockage des pièces dans le tableau pour les supprimer après
                supp[i] = currentPiece;
                //on passe à la pièce suivante
                currentPiece = currentPiece->next;
            }
            //libération des pièces
            for (int i = 0; i < 3; i++) {
                delete supp[i];  
            }
            // Mise à jour de la tête et de la queue de plateau
            if (beforeCurrent == nullptr) {
                board->head = currentPiece;
                tail->next = board->head;
            } else {
                beforeCurrent->next = currentPiece;
                if (currentPiece == board->head) {
                    tail = beforeCurrent;
            }
            }

            beforeCurrent = nullptr;
            board->ScoreInc(board, decal);
            board->nbPieces = board->nbPieces - 3;
        } else {
            beforeCurrent = currentPiece;
            currentPiece = currentPiece->next;
        }
    }

}}
    
    // Fonction qui affiche au terminal la pièce suivante, plateau, score, nombre de pièces
    void DisplayCli(Piece* nextP) {
        cout << "\nnextpiece :\t\t"; cout << "Score: " << score << "\tPieces: " << nbPieces << "\n";

        cout << nextP->display << "\n\n";
        cout << "board :\n";

        Piece* current = head;
        do {
            cout << current->display << " ";
            current = current->next;
        } while (current != head);
        cout << "\n\n";
    }
    
    // Fonction tant que l'utilisateur n'a pas arrivé à 15 pièces on envoie TRUE si il arrive on envoie FALSE pour finir le jeu
    bool GameOver() {
        return !(nbPieces == 15);
    }
    
    // libérer une pièce
    static void freePiece(Piece* piece) {
        delete piece;
    }
    
    // libèrer un plateau
    static void freeboard(LstPieces* board){
    Piece* current = board->head;
    for (int i = 0; i < board->nbPieces; i++) {
        Piece* next = current->next;
        freePiece(current);
        current = next;
    }
    delete board;
}};

int main() {


    LstPieces board;
    board.Initialize(); // Initialiser le plateau
    //boolean qui détermine si le jeu conitnue ou non il utilise la fonction GameOver
    bool go = true;
    //si un utilisateur fait un décalage on retourne TRUE
    bool decal = false; 
    while (go) { // fonction principale termine quand go == false 
        //on affiche l'itération actuelle seule dans l'écran et la précédante se supprime
        system("cls");  
        Piece* nextPiece = board.generate(); // Genere la prochaine piece
        board.DisplayCli(nextPiece); // affiche le plateau

        cout << "Enter an action :\n";
        cout << "1: insert\n";
        cout << "2: shift\n";
        cout << "3: quit \n-> ";
        int action;
        cin >> action;

        if (action == 3) {
           break; 
        } else if (action == 1) {
            decal = false; 

            cout << "Insert at :\n";
            cout << "l: left\n";
            cout << "r: right\n-> ";
            char direction;
            cin >> direction;
            //r pour droit et l pour gauche pour insérer soit à droite ou à gauche de plateau
            if (direction == 'l' || direction == 'r') {
                board.insert(direction, nextPiece); 
            } else {
                cout << "Undefined direction.\n";
            }
        } else 
            if (action == 2) {
              decal = true;
             cout << "Shift the :\n";
             cout << "1: coul\n";
             cout << "2: form\n-> ";
            int type;
            cin >> type;
            if (type == 1) {
                cout << "Input the coul to shift :\n1: Blue\n2: Yellow\n3: Red\n4: Green\n->";
                int coul;
                cin >> coul;
                board.shiftcoul(intTocoul(coul));
            } else if (type == 2) {
                cout << "Input the form to shift :\n1: Square\n2: Diamond\n3: Circle\n4: Triangle\n->";
                int form;
                cin >> form;
                board.shiftform(intToform(form));
            }
        }

        //si il existe trois pièces consecutives de même couleur ou forme on les supprimes
        board.DeleteTriplets(&board, decal);
        //vérifier si l'utilisateur a arrivé à 15 pièces
        go = board.GameOver();
    }
    
    return 0;
}