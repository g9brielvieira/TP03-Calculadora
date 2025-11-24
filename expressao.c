#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


#include "expressao.h"

#define PI 3.14159265358979323846




typedef struct NodeVal {
    float data;
    struct NodeVal *next;
} NodeVal;

// Pilha para strings e precedência - Usada na conversão Infixa
typedef struct NodeStr {
    char *str;
    int precedence; 
    struct NodeStr *next;
} NodeStr;

// funcoes aux da pilha
// revisar essa parte depois
void pushVal(NodeVal **top, float v) {
    NodeVal *novo = (NodeVal*)malloc(sizeof(NodeVal));
    if (novo) {
        novo->data = v;
        novo->next = *top;
        *top = novo;
    }
}

float popVal(NodeVal **top) {
    if (*top == NULL) return 0.0;
    NodeVal *temp = *top;
    float v = temp->data;
    *top = temp->next;
    free(temp);
    return v;
}

void pushStr(NodeStr **top, char *s, int prec) {
    NodeStr *novo = (NodeStr*)malloc(sizeof(NodeStr));
    if (novo) {
        novo->str = strdup(s); // faz a alocacao da copiia da string
        novo->precedence = prec;
        novo->next = *top;
        *top = novo;
    }
}

// Retorna a string e libera o nó, mas NÃO libera a string retornada (tive uma dificuldade absruda de fazer)
char* popStr(NodeStr **top, int *precRet) {
    if (*top == NULL) return NULL;
    NodeStr *temp = *top;
    char *s = temp->str;
    if (precRet) *precRet = temp->precedence;
    *top = temp->next;
    free(temp);
    return s;
}

// --- Funções Auxiliares de Lógica ---

// Define precedência: Números/Funções (4), ^ (3), * / % (2), + - (1)
int getPrecedence(char *op) {
    if (strcmp(op, "^") == 0) return 3;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 2;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    return 4; // Operandos e funções unárias têm prioridade máxim
}

int isOperator(char *token) {
    return (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 || 
            strcmp(token, "*") == 0 || strcmp(token, "/") == 0 || 
            strcmp(token, "%") == 0 || strcmp(token, "^") == 0);
}

int isFunction(char *token) {
    return (strcmp(token, "raiz") == 0 || strcmp(token, "sen") == 0 || 
            strcmp(token, "cos") == 0 || strcmp(token, "tg") == 0 || 
            strcmp(token, "log") == 0);
}

// --- Implementação das Funções Principais ---

float getValorPosFixa(char *StrPosFixa) {
    if (StrPosFixa == NULL) return 0.0;

    NodeVal *pilha = NULL;
    char *copia = strdup(StrPosFixa); // Cópia segura para strtok
    char *token = strtok(copia, " ");

    while (token != NULL) {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            // É número
            pushVal(&pilha, atof(token));
        } 
        else if (isOperator(token)) {
            // Operações binárias [cite: 16]
            float v2 = popVal(&pilha);
            float v1 = popVal(&pilha);
            float res = 0.0;

            if (strcmp(token, "+") == 0) res = v1 + v2;
            else if (strcmp(token, "-") == 0) res = v1 - v2;
            else if (strcmp(token, "*") == 0) res = v1 * v2;
            else if (strcmp(token, "/") == 0) res = v1 / v2;
            else if (strcmp(token, "%") == 0) res = fmod(v1, v2); // Modulo para float
            else if (strcmp(token, "^") == 0) res = pow(v1, v2);

            pushVal(&pilha, res);
        } 
        else if (isFunction(token)) {
            // Funções especiais [cite: 17]
            float v = popVal(&pilha);
            float res = 0.0;

            // Trigonométricas recebem graus [cite: 18]
            if (strcmp(token, "raiz") == 0) res = sqrt(v);
            else if (strcmp(token, "sen") == 0) res = sin(v * PI / 180.0);
            else if (strcmp(token, "cos") == 0) res = cos(v * PI / 180.0);
            else if (strcmp(token, "tg") == 0) res = tan(v * PI / 180.0);
            else if (strcmp(token, "log") == 0) res = log10(v); 

            pushVal(&pilha, res);
        }
        token = strtok(NULL, " ");
    }

    float resultadoFinal = popVal(&pilha);
    free(copia);
    return resultadoFinal;
}

char *getFormaInFixa(char *Str) {
    if (Str == NULL) return NULL; // [cite: 51]

    NodeStr *pilha = NULL;
    char *copia = strdup(Str);
    char *token = strtok(copia, " ");
    char buffer[512];

    while (token != NULL) {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])) || 
            (!isOperator(token) && !isFunction(token))) {
            // É operando (número ou variável n mapeada)
            pushStr(&pilha, token, 4); // precedencia maxima (não precisa de parenteses isolado)
        } 
        else if (isFunction(token)) {
            // função unaria: func(A)
            int precA;
            char *A = popStr(&pilha, &precA);
            if (!A) { free(copia); return NULL; } // Erro de expressão mal formada

            // Funções sempre envolvem parênteses: sen(45)
            sprintf(buffer, "%s(%s)", token, A);
            pushStr(&pilha, buffer, 4); // Resultado tratado como bloco atômico
            free(A);
        } 
        else if (isOperator(token)) {
            // Operador binário: A op B
            int precB, precA;
            char *B = popStr(&pilha, &precB);
            char *A = popStr(&pilha, &precA);
            
            if (!A || !B) { 
                if(A) free(A); if(B) free(B);
                free(copia); return NULL; 
            }

            int currentPrec = getPrecedence(token);
            
            // Lógica de parênteses "estritamente necessários" 
            // Se o operando filho tiver precedência MENOR que o atual, precisa de parênteses.
            // Ex: (1+2) * 3 -> O '+' tem prec menor que '*', então põe parênteses.
            
            char strA[256], strB[256];

            // Trata operando Esquerdo
            if (precA < currentPrec) sprintf(strA, "(%s)", A);
            else strcpy(strA, A);

            // Trata operando Direito
            // Atenção para associatividade ou subtração/divisão (não comutativos)
            // Se prec for igual em operador não comutativo, o da direita precisa proteger (Ex: a-(b-c))
            // Aqui simplificamos protegendo se for menor ou igual em casos de dúvida para segurança.
            if (precB < currentPrec || (precB == currentPrec && (strcmp(token, "-") == 0 || strcmp(token, "/") == 0 || strcmp(token, "^") == 0))) {
                sprintf(strB, "(%s)", B);
            } else {
                strcpy(strB, B);
            }

            // Monta sem espaços 
            sprintf(buffer, "%s%s%s", strA, token, strB);
            pushStr(&pilha, buffer, currentPrec);

            free(A);
            free(B);
        }
        token = strtok(NULL, " ");
    }

    int finalPrec;
    char *resultado = popStr(&pilha, &finalPrec);
    
    
    if (pilha != NULL) {
        // Limpa resto da pilha se houver erro
        while(pilha != NULL) { char *lixo = popStr(&pilha, NULL); free(lixo); }
    }

    free(copia);
    return resultado;
}
