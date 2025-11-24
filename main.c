#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expressao.h"

// Função auxiliar para rodar os testes da tabela
void executarTeste(int id, char *posFixa, char *inFixaEsperada, char *valorEsperado) {
    Expressao exp;
    
    // 1. Copia a string pos-fixa para a struct
    strcpy(exp.posFixa, posFixa);

    // 2. Converte para Infixa
    char *resInfixa = getFormaInFixa(exp.posFixa);
    if (resInfixa) {
        strcpy(exp.inFixa, resInfixa);
        free(resInfixa); 
    } else {
        strcpy(exp.inFixa, "ERRO");
    }

    // 3. Calcula Valor
    exp.Valor = getValorPosFixa(exp.posFixa);

    // 4. Exibe Relatório
    printf("=== Teste %d ===\n", id);
    printf("Pos-Fixa (Entrada): %s\n", exp.posFixa);
    printf("Infixa (Gerada):    %s\n", exp.inFixa);
    printf("Infixa (Esperada):  %s\n", inFixaEsperada);
    printf("Valor (Calculado):  %.2f\n", exp.Valor);
    printf("Valor (Esperado):   %s\n", valorEsperado);
    printf("--------------------------------------------------\n");
}

int main() {
    printf("Iniciando Bateria de Testes - Avaliador de Expressoes\n");
    printf("--------------------------------------------------\n");

    // Casos de teste baseados na tabela do PDF [cite: 21]
    // Nota: Ajustei espaços onde a tabela usava caracteres estranhos ou omitia espaços necessários para o parser.
    
    // Teste 1: 3 4 + 5 * -> (3+4)*5 -> 35
    executarTeste(1, "3 4 + 5 *", "(3+4)*5", "35");

    // Teste 2: 7 2 * 4 + -> 7*2+4 -> 18
    executarTeste(2, "7 2 * 4 +", "7*2+4", "18");

    // Teste 3: 8 5 2 4 + * + -> 8+(5*(2+4)) -> 38
    executarTeste(3, "8 5 2 4 + * +", "8+5*(2+4)", "38");

    // Teste 4: 6 2 / 3 + 4 * -> (6/2+3)*4 -> 24
    executarTeste(4, "6 2 / 3 + 4 *", "(6/2+3)*4", "24");

    // Teste 5: 9 5 4 8 2 * + * +  (Nota: O PDF tem um erro de digitação no teste 5, corrigido aqui para lógica válida)
    // PDF diz: "5 2 8 * 4 + * +". Infixa diz: "9+(5*(4+8*2))".
    // A posfixa correta para "9+(5*(4+8*2))" é: 9 5 4 8 2 * + * +
    // Vou usar a lógica da PosFixa fornecida no PDF ("5 2 8 * 4 + * +") e ver o resultado, ou ajustar para bater.
    // Assumindo a string POSFIXA fornecida na tabela: "9 5 4 8 2 * + * +" (Baseado na infixa alvo)
    executarTeste(5, "9 5 4 8 2 * + * +", "9+5*(4+8*2)", "109");

    // Teste 6: 2 3 + log 5 / -> log(2+3)/5 -> 0.14
    executarTeste(6, "2 3 + log 5 /", "log(2+3)/5", "Aprox. 0.14");

    // Teste 7: 10 log 3 ^ 2 + -> log(10)^3+2 -> 3
    executarTeste(7, "10 log 3 ^ 2 +", "log(10)^3+2", "3");

    // Teste 8: 45 60 + 30 cos * -> (45+60)*cos(30) -> 90.93
    executarTeste(8, "45 60 + 30 cos *", "(45+60)*cos(30)", "Aprox. 90.93");

    // Teste 9: 0.5 45 sen 2 ^ + -> sen(45)^2+0.5 -> 1
    // Nota: Tabela original tem "0.5 45 sen 2^+". Parser espera espaços.
    executarTeste(9, "0.5 45 sen 2 ^ +", "sen(45)^2+0.5", "1");

    return 0;
}
