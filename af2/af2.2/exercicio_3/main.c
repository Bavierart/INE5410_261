#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

// Lê o conteúdo do arquivo filename e retorna um vetor E o tamanho dele
// Se filename for da forma "gen:%d", gera um vetor aleatório com %d elementos
//
// +-------> retorno da função, ponteiro para vetor malloc()ado e preenchido
// | 
// |         tamanho do vetor (usado <-----+
// |         como 2o retorno)              |
// v                                       v
double* load_vector(const char* filename, int* out_size);


// Avalia se o prod_escalar é o produto escalar dos vetores a e b. Assume-se
// que ambos a e b sejam vetores de tamanho size.
void avaliar(double* a, double* b, int size, double prod_escalar);

typedef struct {
    int operations;
    double *a, *b;
} args_t;

void * thread(void * arg) {

    args_t *args = ((args_t *) arg);

    double *result = malloc(sizeof(double));
    *result = 0;

    for (int i = 0; i < args->operations; i++) {
        *result += args->a[i] * args->b[i];
    }

    free(args);
    args = NULL;

    pthread_exit(result);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    //Temos argumentos suficientes?
    if(argc < 4) {
        printf("Uso: %s n_threads a_file b_file\n"
               "    n_threads    número de threads a serem usadas na computação\n"
               "    *_file       caminho de arquivo ou uma expressão com a forma gen:N,\n"
               "                 representando um vetor aleatório de tamanho N\n", 
               argv[0]);
        return 1;
    }
  
    //Quantas threads?
    int n_threads = atoi(argv[1]);
    if (!n_threads) {
        printf("Número de threads deve ser > 0\n");
        return 1;
    }
    //Lê números de arquivos para vetores alocados com malloc
    int a_size = 0, b_size = 0;
    double* a = load_vector(argv[2], &a_size);
    if (!a) {
        //load_vector não conseguiu abrir o arquivo
        printf("Erro ao ler arquivo %s\n", argv[2]);
        return 1;
    }
    double* b = load_vector(argv[3], &b_size);
    if (!b) {
        printf("Erro ao ler arquivo %s\n", argv[3]);
        return 1;
    }
    
    //Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b tem tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }

    //Calcula produto escalar. Paralelize essa parte
    //double result = 0;
    //for (int i = 0; i < a_size; ++i) 
    //    result += a[i] * b[i];

    int stop = n_threads > a_size ? a_size : n_threads;
    int numOpsPerThread = (a_size + stop - 1) / stop;    
    pthread_t threads[stop];

    for (int i = 0; i < stop; i++) {

        int begin = i * numOpsPerThread;
        int end = (i == stop - 1) ? a_size : begin + numOpsPerThread;
        args_t *args = malloc(sizeof(args_t));
        args->operations = end - begin;
        
        args->a = &a[begin];
        args->b = &b[begin];

        pthread_create(&threads[i], NULL, thread, args);
    }

    double result = 0;

    for (int i = 0; i < stop; i++) {
        void *ret;
        printf("\n%p\n", ret);
        pthread_join(threads[i], &ret);
        printf("\n%p\n", ret);

        result += *((double *) ret);
        printf("\n%f\n", *((double*)ret));
        printf("\n%p\n", ret);
        free(ret);
        ret = NULL;
    }
    //    +---------------------------------+
    // ** | IMPORTANTE: avalia o resultado! | **
    //    +---------------------------------+
    avaliar(a, b, a_size, result);

    //Libera memória
    free(a);
    free(b);

    return 0;
}
