#include <iostream>
#include <cstring>
#include <cstdlib> 
#include <ctime>   
#include <cmath>
#include <fstream> 
using namespace std;

enum class Lstform { Square,Triangle, Circle, Diamond};
enum class Lstcoul { Red, Yellow, Green, Blue};


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
struct Piece {
    Lstcoul coul;
    Lstform form;
    Piece* next; 
    Piece* prevcoul;
    Piece* nextcoul; 
    Piece* prevform; 
    Piece* nextform;
    std::string display; 
    Piece(Lstcoul c, Lstform f) : coul(c), form(f), next(nullptr), prevcoul(nullptr), nextcoul(nullptr), prevform(nullptr), nextform(nullptr), display("") {}
};

class LstPieces {
private:
    Piece* head;
    int nbPieces;  
    int score;   

public:
    
    LstPieces() : head(nullptr), score(0), nbPieces(0) {
        srand(time(nullptr)); 
    }

    ~LstPieces() {
        while (head) {
            Piece* temp = head;
            head = head->next;
            delete temp;
        }
    }
    
    std::string DisplayPiece(Lstcoul coul, Lstform form){
    string coulStr;
    string formStr;

    switch (coul) {
        case Lstcoul::Blue:
            coulStr = "Blue";
            break;
        case Lstcoul::Yellow:
            coulStr = "Yellow";
            break;
        case Lstcoul::Red:
            coulStr = "Red";
            break;
        case Lstcoul::Green:
            coulStr = "Green";
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

    return coulStr + " " + formStr;
    }  
    
    
    Piece* generate(){
        Lstcoul coul = static_cast<Lstcoul>(rand() % 4);
        Lstform form = static_cast<Lstform>(rand() % 4);

        Piece* newP = new Piece(coul, form);
        newP->display = DisplayPiece(coul, form);

        return newP;
    }
    
    void Initialize() {
        Piece* newP = generate();
        newP->next = newP;
        newP->prevform = newP;
        newP->nextform = newP;
        newP->prevcoul = newP;
        newP->nextcoul = newP;

        head = newP;
        score = 0;                 
        nbPieces = 1;                
    }
    
    Piece* getTail(){
        Piece* tail = head;
        while (tail->next != head) {
            tail = tail->next;
        }
        return tail;
    }       
    
    void insert(char direction, Piece* newP) {
        if (head == nullptr) {
            head = newP;
            newP->next = newP;
        } else {
            Piece* tail = getTail();
            tail->next = newP;
            newP->next = head;
            nbPieces++;

            if (direction == 'l') {
                head = newP;
            }
        }

            Piece* sameformPiece = newP->next;
            while (sameformPiece->form != newP->form) {
                sameformPiece = sameformPiece->next;
            }
            if (sameformPiece == newP) {
                newP->prevform = newP;
                newP->nextform = newP;
            } else {
                Piece* prevformPiece = sameformPiece->prevform;
                newP->prevform = prevformPiece;
                newP->nextform = sameformPiece;
                prevformPiece->nextform = newP;
                sameformPiece->prevform = newP;
            }

            Piece* samecoulPiece = newP->next;
            while (samecoulPiece->coul != newP->coul) {
                samecoulPiece = samecoulPiece->next;
            }
            if (samecoulPiece == newP) {
                newP->prevcoul = newP;
                newP->nextcoul = newP;
            } else {
                Piece* prevcoulPiece = samecoulPiece->prevcoul;
                newP->prevcoul = prevcoulPiece;
                newP->nextcoul = samecoulPiece;
                prevcoulPiece->nextcoul = newP;
                samecoulPiece->prevcoul = newP;
            }
    }
    
    void restoreforms(){
        Piece* heads[4] = {nullptr};
        Piece* tails[4] = {nullptr};
        Piece* current = head;
        do {
            int i = static_cast<int>(current->form);
            if (heads[i] == nullptr) {
                heads[i] = current;
                tails[i] = current;
            } else {
                tails[i]->nextform = current;
                current->prevform = tails[i];
                tails[i] = current;
            }
            current = current->next;
        } while (current != head);
  
        for (int i = 0; i < 4; i++) {
            if (heads[i] != nullptr) {
                tails[i]->nextform = heads[i];
                heads[i]->prevform = tails[i];
            }
        }
    }
    
    void shiftcoul(Lstcoul coul){
    Piece* firstCoul = nullptr;
    Piece* lastCoul = nullptr;
    Piece* current = head;
    do {
        if (current->coul == coul) {
            if (firstCoul == nullptr) {
                firstCoul = current;
            }
            lastCoul = current;
        }
        current = current->next;
    } while (current != head);

    if (firstCoul != nullptr && lastCoul != nullptr && firstCoul != lastCoul) {
        Lstcoul tmpCoul = lastCoul->coul;
        std::string tmpDisplay = lastCoul->display;

        current = lastCoul;
        while (current != firstCoul) {
            Piece* prev = current->prevcoul;
            while (prev->coul != current->coul) {
                prev = prev->prevcoul;
            }

            current->coul = prev->coul;
            current->display = prev->display;
            current = prev;
        }

        firstCoul->coul = tmpCoul;
        firstCoul->display = tmpDisplay;
    }

    restorecouls();
}

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
    
    void shiftform(Lstform form){
    Piece* firstForm = nullptr;
    Piece* lastForm = nullptr;
    Piece* current = head;
    do {
        if (current->form == form) {
            if (firstForm == nullptr) {
                firstForm = current;
            }
            lastForm = current;
        }
        current = current->next;
    } while (current != head);

    if (firstForm != nullptr && lastForm != nullptr && firstForm != lastForm) {
        Lstform tmpForm = lastForm->form;
        std::string tmpDisplay = lastForm->display;

        current = lastForm;
        while (current != firstForm) {
            Piece* prev = current->prevform;
            while (prev->form != current->form) {
                prev = prev->prevform;
            }

            current->form = prev->form;
            current->display = prev->display;
            current = prev;
        }

        firstForm->form = tmpForm;
        firstForm->display = tmpDisplay;
    }

    restoreforms();
}

    void ScoreInc(LstPieces *board, int decal){

      if (decal)
      {
        board->score = board->score + 7;
      }
      else{
        board->score = board->score + 5;
      }
   }
    
    void DeleteTriplets(LstPieces *board, int decal) {
        
    if (board->nbPieces >= 3) {
    int listcoul, listform;
    Piece *currentPiece = board->head;
    Piece *beforeCurrent = nullptr;
    int ListLarger=0;
    Piece *supp[3];
    Piece *tail = getTail();

    while (currentPiece->next != board->head) {
        listcoul = 1; 
        listform = 1;
        Piece* current = board->head->next;
       
        for (int i = 1; i < 3; i++) {
            if (current == board->head){
                break;}
            if (listcoul == i && current->coul == currentPiece->coul){
                 listcoul++;}
            if (listform == i && current->form == currentPiece->form){
                 listform++;}
            current = current->next;
        }
        if (listcoul > listform){
            ListLarger = listcoul;
        }else{
            ListLarger = listform;
        }
        if (board->nbPieces == 3 && ListLarger == 3)
        {
            cout << "game won !";
            return;
        }
        if (ListLarger == 3) {
            for (int i = 0; i < 3; i++) {
                currentPiece->prevform->nextform = currentPiece->nextform;
                currentPiece->nextform->prevform = currentPiece->prevform;
                currentPiece->prevcoul->nextcoul = currentPiece->nextcoul;
                currentPiece->nextcoul->prevcoul = currentPiece->prevcoul;
                supp[i] = currentPiece;
                currentPiece = currentPiece->next;
            }
            for (int i = 0; i < 3; i++) {
                delete supp[i];  
            }
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
    
    bool GameOver() {
        return !(nbPieces == 15);
    }
    
    static void freePiece(Piece* piece) {
        delete piece;
    }
    
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
    board.Initialize();
    bool go = true;
    bool decal = false; 
    while (go) {  
        system("cls");  
        Piece* nextPiece = board.generate(); 
        board.DisplayCli(nextPiece);  

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

        board.DeleteTriplets(&board, decal);
        go = board.GameOver();
    }
    
    return 0;
}