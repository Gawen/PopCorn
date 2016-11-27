#define __MAIN__
#include "popcorn.hpp"

#define CRN_TAG_HALATION    0x2

void Debug__(void);

// Vid�o
void InitializeVideo(void);               // D�finit la taille de l'�cran

typedef struct SplashObjsStruct {
    PopCorn::ObjScene ScSplash;           // Introduction, les images de d�but, les Splashes
                                          // Dans le fichier splash.cpp
    
    PopCorn::MatTexImage WQSpl;           // Le logo WQ
    PopCorn::ObjPlane Image;              // Permettra d'afficher le logo WQ
    PopCorn::ObjFunction Func;            // Callback qui permet les effets de fondus
    PopCorn::TimerEvent Timer;            // Timer d'attente pour le splash
    PopCorn::ObjSound Scape;
} SplashObjsClass;
    
void CallSplashes(void);
void QuitSplashes(void);

void CallMenu(void);
