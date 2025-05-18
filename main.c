#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_OPS 100
#define MAX_MACHINES 64

// Estrutura de uma operação
typedef struct {
    int machine;
    int duration;
    int start_time;
    int end_time;
} Operation;

// Estrutura de um Job
typedef struct {
    Operation ops[MAX_OPS];
    int num_ops;
} Job;

int max(int a, int b) { return a > b ? a : b; }

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <dadosDeEntrada.txt> <dadosDeSaida.txt>\n", argv[0]);  // CORRIGIDO argv[0]
        return 1;
    }

    // // Medição de tempo
    // struct timespec start_time, end_time;
    // clock_gettime(CLOCK_MONOTONIC, &start_time);

    FILE *fin = fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "w");
    if (!fin || !fout) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    int num_jobs, num_machines;
    if (fscanf(fin, "%d %d", &num_jobs, &num_machines) != 2) {
        fprintf(stderr, "Erro ao ler o número de jobs e máquinas.\n");
        fclose(fin);
        fclose(fout);
        return 1;
    }

    // ALOCAÇÃO DINÂMICA (evita stack overflow)
    Job *jobs = malloc(num_jobs * sizeof(Job));
    if (!jobs) {
        fprintf(stderr, "Erro de alocação de memória para jobs.\n");
        fclose(fin);
        fclose(fout);
        return 1;
    }

    int machines_available[MAX_MACHINES] = {0};

    for (int j = 0; j < num_jobs; j++) {
        jobs[j].num_ops = num_machines;
        int current_time = 0;
        for (int o = 0; o < num_machines; o++) {
            if (fscanf(fin, "%d %d", &jobs[j].ops[o].machine, &jobs[j].ops[o].duration) != 2) {
                fprintf(stderr, "Erro ao ler os dados do job %d, operação %d.\n", j, o);
                free(jobs); // LIBERA MEMÓRIA
                fclose(fin);
                fclose(fout);
                return 1;
            }
            int m = jobs[j].ops[o].machine;
            int earliest_start = max(current_time, machines_available[m]);
            jobs[j].ops[o].start_time = earliest_start;
            jobs[j].ops[o].end_time = earliest_start + jobs[j].ops[o].duration;
            current_time = jobs[j].ops[o].end_time;
            machines_available[m] = jobs[j].ops[o].end_time;
        }
    }

    // Medição de tempo
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Escrever resultado
    int makespan = 0;
    for (int j = 0; j < num_jobs; j++) {
        fprintf(fout, "Job %d:\n", j);
        for (int o = 0; o < jobs[j].num_ops; o++) {
            Operation *op = &jobs[j].ops[o];
            fprintf(fout, "  Op %d (M%d): start=%d, end=%d\n", o, op->machine, op->start_time, op->end_time);
            if (op->end_time > makespan) {
                makespan = op->end_time;
            }
        }
    }
    fprintf(fout, "Makespan: %d\n", makespan);

    // Liberação de recursos
    free(jobs);
    fclose(fin);
    fclose(fout);

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Tempo de execucao (s): %6f\n", elapsed);

    return 0;
}
