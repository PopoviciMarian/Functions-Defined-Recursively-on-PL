#include <iostream>
#include <graphics.h>
#include <winbgim.h>
#include <cstring>
#include <cmath>
#include <stdlib.h>
#define MAX_SIZE 200

using namespace std;

int windowWidth = 1000;
int windowHeight = 700;
int size_circle = 25;
char listSubformulae[MAX_SIZE][MAX_SIZE];
int lengthListSubformulae = 0;
const char *propositionalVariables[16] = { "p", "p1", "p2", "p3", "p'", "q", "q1", "q2", "q3", "q'", "r", "r1", "r2", "r3", "r'"};
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

struct Nod{
    char type[10]; /// n, ^, v, p, q, r, p1...
    char child1[MAX_SIZE];
    char child2[MAX_SIZE];
    struct Nod *nod1;
    struct Nod *nod2;
};

struct char20{
    char s[20];
    int index;
};

bool isPropositionalVariable(char s[2]){
    for(int i = 0; i < 15; ++i){
        if(strcmp(propositionalVariables[i], s) == 0){
            return true;
        }
    }
    return false;
}

bool isNegationLogicalConnector(char s){
    if(s == 'n' || s == 'N')
        return true;
    return false;
}

bool isConjunctionsOrDisjunctions(char s){
    if(s == '^' || s == 'v' || s == 'V')
        return true;
    return false;
}

struct char20 findCase(char str[]){
    struct char20 result;
    strcpy(result.s, "");
    if(isNegationLogicalConnector(str[0])){
       result.index = 0;
       strcpy(result.s, "n");
        return result;
    }
    else if(str[0] == '('){
        char var[4];
        var[0] = str[1];
        var[1] = str[2];
        var[2] = '\0';
        if(isPropositionalVariable(var)){ /// cazul p1, p2, p3, q1......
            result.s[0] = str[3];
            result.s[1] = '\0';
            result.index = 3;
            return result;
        }
        var[1] = '\0';
        if(isPropositionalVariable(var)){ /// cazul p, q, r
            result.index = 2;
            result.s[0] = str[2];
            result.s[1] = '\0';
            return result;
        }
        int nrParenthesis = 0;
        if(isNegationLogicalConnector(str[1])){
            if(str[2] != '('){
                for(int i = 3; i < strlen(str); i++){
                    if(str[i] == '('){
                        nrParenthesis++;
                    }
                    else if(str[i] == ')'){
                        nrParenthesis--;
                    }
                    if(isConjunctionsOrDisjunctions(str[i]) && nrParenthesis == 0){///  case nnnnp
                        result.index = i;
                        result.s[0] = str[i];
                        result.s[1] = '\0';
                        return result;
                    }
                }
            }
            else{
                for(int i = 2; i < strlen(str); ++i){
                    if(str[i] == '('){
                        nrParenthesis++;
                    }
                    else if(str[i] == ')'){
                        nrParenthesis--;
                    }
                    if(nrParenthesis == 0){
                        for(int j = i+1; i < strlen(str); ++i){
                            if(isConjunctionsOrDisjunctions(str[j])){
                                result.index = j;
                                var[0] = str[j];
                                var[1] = '\0';
                                strcpy(result.s, var);
                                return result;
                            }
                        }
                    }
                }
            }
        }
        else if(str[1] == '('){
            for(int i = 1; i < strlen(str); ++i){
                if(str[i] == '('){
                    nrParenthesis++;
                }
                else if(str[i] == ')'){
                    nrParenthesis--;
                }
                if(nrParenthesis == 0){
                    result.index = i+1;
                    result.s[0] = str[i+1];
                    result.s[1] = '\0';
                    return result;
                }
            }
        }
    }
    strcpy(result.s, str);
    result.index = 0;
    return result;
}

struct Nod *createNod(char s[MAX_SIZE]){
    struct Nod* newNod = new Nod();
    if(isPropositionalVariable(s)){
        strcpy(newNod->type, s);
        strcpy(newNod->child1, "NULL");
        strcpy(newNod->child2, "NULL");
    }
    if(isNegationLogicalConnector(s[0])){
        strcpy(newNod->type, "n");
        strcpy(newNod->child1, s+1);
        strcpy(newNod->child2, "NULL");
    }
    if(s[0] == '('){
        struct char20 result = findCase(s);
        strcpy(newNod->type, result.s);
        strncpy(newNod->child1, s+1, result.index - 1);
        newNod->child1[result.index-1] = '\0';
        strcpy(newNod->child2, s+result.index + 1);
        newNod->child2[strlen(s)-result.index - 2] = '\0';
    }
    return newNod;
}

void buildTree(struct Nod *root){
    if(strcmp(root->child1, "NULL")){
        root->nod1 = createNod(root->child1);
        buildTree(root->nod1);
    }else{
        root->nod1 = NULL;
    }

    if(strcmp(root->child2, "NULL")){
        root->nod2 = createNod(root->child2);
        buildTree(root->nod2);
    }else{
        root->nod2 = NULL;
    }
}

int findHeight(struct Nod *root){
    if(root->nod1 == NULL && root->nod2 == NULL){
        return 1;
    }
    else if(isNegationLogicalConnector(root->type[0]) && root->type[1] == '\0'){
        return 1 + findHeight(root->nod1);
    }
    else
        return 1 + max(findHeight(root->nod1), findHeight(root->nod2));
}

int findSize(struct Nod *root){
     if(root->nod1 == NULL && root->nod2 == NULL){
        return 1;
    }
     else if(isNegationLogicalConnector(root->type[0]) && root->type[1] == '\0'){
        return 1 + findSize(root->nod1);
    }
    else
        return 1 + findSize(root->nod1) + findSize(root->nod2);
}

void subf(struct Nod *root){
    bool valid = true;
    if(root->nod1 != NULL){
        for(int i = 0; i < lengthListSubformulae; ++i){
            if(strcmp(root->child1, listSubformulae[i]) == 0){
                valid = false;
            }
        }
        if(valid){
            strcpy(listSubformulae[lengthListSubformulae++], root->child1);
            subf(root->nod1);
        }
    }
    valid = true;
    if(root->nod2 != NULL){
        for(int i = 0; i < lengthListSubformulae; ++i){
            if(strcmp(root->child2, listSubformulae[i]) == 0){
                valid = false;
            }
        }
        if(valid){
            strcpy(listSubformulae[lengthListSubformulae++], root->child2);
            subf(root->nod2);
        }
    }
}

void printSubf(struct Nod *root, char s[MAX_SIZE]){
    strcpy(listSubformulae[lengthListSubformulae++], s);
    cout << "Subformulae : {" ;
    subf(root);
    for(int i = 0; i < lengthListSubformulae; ++i){
        SetConsoleTextAttribute(hConsole, 3);
        cout << listSubformulae[i] ;
        SetConsoleTextAttribute(hConsole, 7);
        cout << ", ";
    }
    cout << "\b\b}\n";
}

void drawNod(int yPoz, int xPoz, char type[20], int heightTree){
    char image[20];
    int units_y = 100 ;
    setcolor(0);
    int unit_width = windowWidth/pow(2, heightTree);
    int start = unit_width* (pow(2, heightTree - yPoz) );
    int space_between = pow(2, heightTree+1 - yPoz);
    strcpy(image, "var/");
    strcat(image, type);
    strcat(image, ".jpg\0");
    readimagefile(image, start + (xPoz -1) * unit_width * space_between - size_circle, 100 + (yPoz-1)*units_y  - size_circle, start +  (xPoz -1) *space_between* unit_width + size_circle, 100+(yPoz-1)*units_y + size_circle);
}

void drawLine(int xFather, int yFather, int xChild, int yChild, int heightTree){
    int units_y = 100 ;
    int unit_width = windowWidth/pow(2, heightTree);

    int start = unit_width* (pow(2, heightTree - yFather) );
    int space_between = pow(2, heightTree+1 - yFather);
    int xx = start + (xFather -1) * unit_width * space_between;
    int yy = 100 + (yFather-1)*units_y + size_circle;

    start = unit_width* (pow(2, heightTree - yChild) );
    space_between = pow(2, heightTree+1 - yChild);
    int xxx = start + (xChild -1) * unit_width * space_between;
    int yyy = 100 + (yChild-1)*units_y -size_circle ;
    line(xx, yy, xxx, yyy);
}

void printTreeRecursive(struct Nod *root, int x, int y, int heightTree){
    drawNod(y, x, root->type, heightTree);
    if(root->nod1 != NULL){
        drawLine(x, y, 2*x-1, y+1, heightTree);
        printTreeRecursive(root->nod1, 2*x-1, y+1, heightTree);
    }
    if(root->nod2 != NULL){
        drawLine(x, y, 2*x, y+1, heightTree);
        printTreeRecursive(root->nod2, 2*x, y+1, heightTree);
    }
}

void drawTree(struct Nod *root){
    int height = findHeight(root);
    if(height == 1){
        windowWidth = 150;
    }
    windowWidth = size_circle * pow(2, height+1);
    while(windowWidth > GetSystemMetrics(SM_CXSCREEN)){
        size_circle--;
        windowWidth = size_circle * pow(2, height+1);
    }
    if(height == 1){
        windowWidth = 150;
    }
    initwindow(windowWidth, windowHeight, "",0, 0);
    setbkcolor(15);
    cleardevice();
    printTreeRecursive(root, 1, 1, findHeight(root));
}

void drawText(char str[MAX_SIZE], int height, int size_tree){
    setcolor(COLOR(75,158,219));
    settextstyle(3, 0, 3);
    for(int i = 0; i < strlen(str); ++i){
        if(isNegationLogicalConnector(str[i])){
            str[i] = '¬';
        }
        else if(str[i] == 'v'){
            char aux[MAX_SIZE];
            strcpy(aux, str+i+1);
            str[i] = '\\';
            str[i+1] = '/';
            strcpy(str+i+2, aux);
        }
        else if(str[i] == '^'){
            char aux[MAX_SIZE];
            strcpy(aux, str+i+1);
            str[i] = '/';
            str[i+1] = '\\';
            strcpy(str+i+2, aux);
        }
    }
    outtextxy(10, 10, str);

    char aux[MAX_SIZE];
    strcpy(aux, "size(");
    strcat(aux, str);
    setcolor(0);
    outtextxy(10, windowHeight-30, "size(");
    setcolor(COLOR(75,158,219));
    outtextxy(10 + textwidth("size("), windowHeight-30, str);
    setcolor(0);
    outtextxy(10 + textwidth(aux), windowHeight-30, ")=");
    strcat(aux, ")=");
    char size_tree_string[10];
    itoa(size_tree, size_tree_string, 10);
    outtextxy(10 + textwidth(aux), windowHeight-30, size_tree_string);

    strcpy(aux, "height(");
    strcat(aux, str);
    setcolor(0);
    outtextxy(10, windowHeight-60, "height(");
    setcolor(COLOR(75,158,219));
    outtextxy(10 + textwidth("height("), windowHeight-60, str);
    setcolor(0);
    outtextxy(10 + textwidth(aux), windowHeight-60, ")=");
    strcat(aux, ")=");
    char height_string[10];
    itoa(height, height_string, 10);
    outtextxy(10 + textwidth(aux), windowHeight-60, height_string);
}

void coutAlphabet(){
    cout << "Propositional variables : {";
    for(int i = 0; i < 15; i++){
        SetConsoleTextAttribute(hConsole, 3);
        cout << propositionalVariables[i];
        SetConsoleTextAttribute(hConsole, 7);
        cout << ", ";
    }
    cout << "\b\b}";

    cout << "\nAuxiliary symbols : {";
    SetConsoleTextAttribute(hConsole, 3);
    cout << "(";
    SetConsoleTextAttribute(hConsole, 7);
    cout << ",";
    SetConsoleTextAttribute(hConsole, 3);
    cout << ")";
    SetConsoleTextAttribute(hConsole, 7);
    cout << "}";

    cout << "\nLogical connectives : \n\t-Conjunction : ";
    SetConsoleTextAttribute(hConsole, 3);
    cout << "^";
    SetConsoleTextAttribute(hConsole, 7);
    cout << "\n\t-Disjunction : ";
    SetConsoleTextAttribute(hConsole, 3);
    cout << "v";
    SetConsoleTextAttribute(hConsole, 7);
    cout << "\n\t-Negation : ";
    SetConsoleTextAttribute(hConsole, 3);
    cout << "n";
    SetConsoleTextAttribute(hConsole, 7);
    cout << "\nExample : ";
    SetConsoleTextAttribute(hConsole, 3);
    cout << "((q1vnp)^(r'vq3))";

}

int main(){
    /// MAX HEIGHT = 7
    char s[] = "(((npv(r'vr3))v((r'^q')v(r2vq2)))v(((p^q1)v(r'vr1))^(np2v(r3vq2))))";
    coutAlphabet();
    SetConsoleTextAttribute(hConsole, 7);
    cout << "\nGive an element from PL : ";
    SetConsoleTextAttribute(hConsole, 3);
    cin.getline(s, MAX_SIZE);
    SetConsoleTextAttribute(hConsole, 7);
    Nod *root = new Nod;
    if(isNegationLogicalConnector(s[0])){
        strcpy(root->type, "n");
        strcpy(root->child1, s+1);
        strcpy(root->child2, "NULL");
    }
    else if(s[0] == '('){
        struct char20 result = findCase(s);
        strcpy(root->type, result.s);
        strncpy(root->child1, s+1, result.index - 1);
        root->child1[result.index-1] = '\0';
        strcpy(root->child2, s+result.index + 1);
        root->child2[strlen(s)-result.index - 2] = '\0';
    }
    else{
        strcpy(root->type, s);
        strcpy(root->child1, "NULL");
        strcpy(root->child2, "NULL");
    }
    buildTree(root);
    cout << "Height : " <<findHeight(root) << '\n';
    cout << "Size : " << findSize(root) << '\n';
    printSubf(root, s);
    windowHeight = 110 + 100*findHeight(root);
    drawTree(root);
    drawText(s, findHeight(root), findSize(root));
    getch();
    return 0;
}
