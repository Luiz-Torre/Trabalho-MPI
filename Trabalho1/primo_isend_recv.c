#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int primo (long int n) { // Função para verificar se um número é primo
    int i;

    for (i = 3; i < (int)(sqrt(n) + 1); i += 2) { // Percorre os números ímpares de 3 até a raiz quadrada de n
        if (n % i == 0) return 0; // Se o número for divisível por algum i, não é primo
    }

    return 1; // Se não encontrou divisores, é primo
}

int main(int argc, char *argv[]) {
    double t_inicial, t_final;
    int cont = 0, total = 0;
    long int i, n;
    int meu_ranque, num_procs, inicio, salto;
    int tag = 0; // Tag para identificar a mensagem

    if (argc < 2) { // Verifica se o usuário forneceu o número como argumento de linha de comando
        printf("Valor inválido! Entre com um valor do maior inteiro\n");
        return 0;
    } else {
        n = strtol(argv[1], (char **) NULL, 10); // Obtém o maior inteiro do intervalo a partir dos argumentos de linha de comando
    }

    MPI_Init(&argc, &argv); // Inicia o ambiente MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque); // Obtém o identificador do processo atual
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs); // Obtém o número total de processos em execução
    t_inicial = MPI_Wtime(); // Marca o tempo de início da execução

    inicio = 3 + meu_ranque * 2; // Define o início do intervalo para cada processo
    salto = num_procs * 2; // Define o tamanho do salto entre os números a serem verificados por cada processo

    for (i = inicio; i <= n; i += salto) { // Percorre os números no intervalo atribuído a cada processo
        if (primo(i) == 1) cont++; // Conta os números primos no intervalo atribuído a cada processo
    }

    if (num_procs > 1) { // Se houver mais de um processo em execução
        if (meu_ranque != 0) {
        // MPI_Isend
        MPI_Request request;
        MPI_Isend(&cont, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &request); // Envia o valor de cont para o processo de ranque 0 de forma assíncrona
        } else {
            total = cont;
            for (int proc = 1; proc < num_procs; proc++) {
                int temp_cont;
                MPI_Recv(&temp_cont, 1, MPI_INT, proc, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recebe o valor de cont de cada processo
                total += temp_cont; // Soma os valores de cont
            }
        }
    } else {
        total = cont;
    }

    t_final = MPI_Wtime(); // Marca o tempo de fim da execução

    if (meu_ranque == 0) { // Processo de ranque 0
        total += 1; // Acrescenta o dois, que também é primo
        printf("Quant. de primos entre 1 e n: %d \n", total); // Imprime a quantidade total de primos encontrados
        printf("Tempo de execucao: %1.3f \n", t_final - t_inicial); // Imprime o tempo de execução
    }

    MPI_Finalize(); // Finaliza o ambiente MPI
    return 0;
}