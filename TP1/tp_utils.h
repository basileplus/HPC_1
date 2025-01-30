#ifndef	_TP_UTILS_H_
#define	_TP_UTILS_H_

#define WINDOWS_VERSION

#define ALIGNMENT	16

#define vec_border_ps(a,b) _mm_shuffle_ps(a,b,0x4e)
#define vec_middle_ps(a,b) _mm_shuffle_ps(a,b,0x99)
 
#define vec_left1_ps(v0, v1)  vec_middle_ps(vec_border_ps(v0, v1), v1)
#define vec_right1_ps(v1, v2) vec_middle_ps(v1, vec_border_ps(v1, v2))

// La fonction suivante alloue un tableau de float aligné sur "ALIGNMENT" octets
// mais ne l'initialise pas.
float*	float_CreateArray(unsigned short Length);

// La fonction suivante renvoie un tableau de float tel que:
// R[0] = First, ..., R[i] = First + i x Step, ..., R[Length-1] = First + (Length-1) x Step
// Elle renvoie NULL en cas d'erreur durant l'allocation ou si Length est nul.
// LE TABLEAU CREE EST ALIGNE SUR "ALIGNMENT" OCTETS
float*	float_InitArrayWithStep(float First, float Step, unsigned short Length);

// La fonction suivante affiche le tableau de float qui lui est passé en argument comme suit:
// str[0] = X str[1] = X ... str[3] = X
// ...
// str[i-2] = X str[i-1] = X
void float_PrintArray(char* str, float* Array, unsigned short Length);

// La fonction suivante détruit le tableau aligné qui lui est passé en argument
void float_FreeArray(float* Array);

#endif
