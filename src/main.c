#include "main.h"

typedef struct {
	FILE * fptr;
}path_arq; //struct para captura da matriz de entrada

 ///* Regiao de Variaves Globais no Escopo main.c *
static lst_ptr_th colun_date[QTD_COLLUN];
static char arq_origem[] = "C:\\GitHub\\Paralela-Matriz-Normalizacao\\arq_csvs\\dataset_00_1000.csv";
//static pthread_mutex_t mutex;

static controles control_process;

static char*** dataset_data;
static char*** dataset_normalizado;
static path_arq path_arq_t[1];
args_memory my_set_memory;



static void calloc_memory_dataset(unsigned int n)
{
    dataset_data =  (char ***) calloc(n, sizeof(char **));
    dataset_normalizado = calloc(n, sizeof(char **));
    unsigned int i;

    #ifdef INSTALL_OMP
        #pragma omp parallel for
    #endif // INSTALL_OMP
    for (i = 0; i < n; i++) {
        dataset_data[i] = (char**) calloc(QTD_COLLUN, sizeof(char *));
        dataset_normalizado[i] = (char**) calloc(QTD_COLLUN, sizeof(char *));
    }
    #ifdef INSTALL_OMP
        #pragma omp parallel for
    #endif // INSTALL_OMP
    for (i = 0; i < n; i++) {
        for (unsigned int j = 0; j < QTD_COLLUN; j++) {
            dataset_data[i][j] =  (char*) calloc(QTD_WORD, sizeof(char));
            dataset_data[i][j][0] = 0;
            dataset_normalizado[i][j] =  (char*) calloc(QTD_WORD, sizeof(char));
            dataset_normalizado[i][j][0] = 0;
        }
    }
}

static void clear_memory_dataset(void * _args)
{
    #ifndef INSTALL_OMP
        #pragma cmp parallel for
        unsigned int i;
        for (i = 0; i < N; i++) {
            for (unsigned int j = 0; j < QTD_COLLUN; j++) {
                strcpy(dataset_normalizado[i][j], "");
                strcpy(dataset_data[i][j], "");
            }
        }
    #endif // INSTALL_OMP
    #ifdef INSTALL_OMP
    args * _m = (args *) _args;
    lst_ptr l = _m->lista;
    while (l != NULL) {
        for (unsigned int j = 0; j < QTD_COLLUN; j++) {
            int debug = l->dado;
            strcpy(dataset_data[l->dado][j], "");
            strcpy(dataset_normalizado[l->dado][j], "");
            //lst_print(l);
            //printf("\n");
        }
        l = l->prox;
    }
    #endif // INSTALL_OMP
}

static void * solicitacao_arquivo_job(void * _args)
{
    args * _args_t = (args*) _args;

    lst_ptr l = _args_t->lista;

    while (l != NULL) {
        lst_ptr_th p = colun_date[l->dado - 1];
        while (p != NULL) {
            if (p->dado.id == 0)
                fprintf(_args_t->fptr_destinos[(l->dado - 1) % QTD_COLLUN_THREAD], "%s,,\n", p->dado.word);
            fprintf(_args_t->fptr_destinos[(l->dado - 1) % QTD_COLLUN_THREAD], "%d,%s,%d\n", p->dado.id, p->dado.word, p->dado.count);
            p = p->prox;
        }
        l = l->prox;
    }
    return 0;
}

/// *Adicionar infos das colunas do arquivo de entrada*
static void add_lst_info_distinct(lst_ptr_th * l, char * str)
{
    int id_t = 0;
	lst_info_th info_t;
	strcpy(info_t.word, str);

	if (!lst_existing_th(*l, info_t, &id_t)) {
		info_t.id = id_t;
        info_t.count = 1;
		lst_ins_th(l, info_t);
	}
}

/// *Verifica se info da coluna é um job a processar*
static bool is_my_job(lst_ptr l, int colun)
{
    while (l != NULL) {
        if (l->dado == colun)
            return true;
        l = l->prox;
    }
    return false;
}

/// *normaliza info separadamente do arquivo de entrada principal*
static int normalize_info_date(args args_t, char * str, int colun, int * _id_word)
{
    if (strcmp(str, "") != 0) {
        lst_info_th info_t;
        strcpy(info_t.word, str);

        if (is_my_job(args_t.lista, colun)) {
            int id = lst_info_id_th(colun_date[colun - 1], info_t);
            if (id != NOT_EXIST) {
                *_id_word = id;
                return PROCEED;
            }
            else
                return HOLD;
        }
        return NEXT;
    }
    else return HOLD;
}

/// *normaliza info unico do arquivo de entrada em unica saida principal*
static void * normaliza_colun_date(void * _args)
{
    //pthread_mutex_lock(&mutex);
	args * args_t = (args*) _args;
	char word[200];
	tipoDado i, id_word, count = 0;

    do {
        lst_ptr l = args_t->lista;
        sem_wait(&control_process.mutexs_threads[args_t->id - 1]);
        sem_wait(&control_process.mutexs_process[args_t->id - 1]);
        while (l != NULL) {
        #ifdef INSTALL_OMP
            #pragma omp parallel for
        #endif // INSTALL_OMP
            for (i = 0; i < N;) {
                switch (normalize_info_date(*args_t, dataset_data[i][l->dado - 1], l->dado, &id_word)) {
                    case HOLD :
                    break;
                    case PROCEED :
                        if (id_word == 0) {
                            sprintf(word, "%s,", dataset_data[i][l->dado - 1]);
                            strcpy(dataset_normalizado[i][l->dado - 1], word);
                        }
                        else {
                            sprintf(word, "%d,", id_word);
                            strcpy(dataset_normalizado[i][l->dado - 1], word);
                        }
                        i += 1;
                    break;
                    case NEXT :
                        i += 1;
                    break;
                 }
            }
            l = l->prox;
        }
        count += i;
        sem_post(&control_process.mutexs_process[args_t->id - 1]);
    } while (count < N_TOTAL);
    //pthread_mutex_unlock(&mutex);
    return;
}

/// *Lidar com controle de escrita do arquivo de saida principal*
static void * solicitacao_arquivo(void * argsArq)
{
	ptr_args_arq _argssArq = (ptr_args_arq) argsArq;
	tipoDado i, j, count = 0;

    do {
        sem_wait(&control_process.mutexs_threads[NUM_THREADS]);
        sem_wait(&control_process.mutexs_process[NUM_THREADS]);
        for (i = 0; i < N; i++) {
            for (j = 0; j < QTD_COLLUN;) {
                if (strcmp(dataset_normalizado[i][j], "") != 0) {
                    fprintf(_argssArq->arq_main, "%s", dataset_normalizado[i][j]);
                    j++;
                }
            }
            fprintf(_argssArq->arq_main, "\n");
        }
        count += i;
        sem_post(&control_process.mutexs_process[NUM_THREADS]);
    } while (count < N_TOTAL);
    fclose(_argssArq->arq_main);
	return 0;
}


static void aguarda_processos_threads()
{
    for (unsigned int i = 0; i < NUM_THREADS + 1; i++)
        sem_wait(&control_process.mutexs_process[i]);
}

static void liberar_processos_threads()
{
    for (unsigned int i = 0; i < NUM_THREADS + 1; i++)
        sem_post(&control_process.mutexs_process[i]);
}

static void desbloqueio_threads()
{
    for (unsigned int i = 0; i < NUM_THREADS + 1; i++)
        sem_post(&control_process.mutexs_threads[i]);
}

static void clear_memory()
{
    #ifdef INSTALL_OMP
    int status;
    for (unsigned int i = 0; i < NUM_THREADS; i++) {
        if (status_create( status = pthread_create((&my_set_memory._my_set[i].thread), NULL, clear_memory_dataset, (void *)&my_set_memory._my_set[i])));
        else exit(0xF);
	}
    #endif // INSTALL_OMP
    #ifndef INSTALL_OMP
     clear_memory_dataset(NULL);
    #endif // INSTALL_OMP

}

static void * ler_matriz_entrada(void * args)
{
	path_arq * _path_arq_t = (path_arq*) args;
	char str[1001], *token;
	unsigned int i;
	tipoDado count = 0;

	for (i = 0; i < QTD_COLLUN; i++)
        lst_init_th(&colun_date[i]);
    do {
        if (count != 0 ) {
            //printf("Aguardando Todos\n");
            aguarda_processos_threads();
            //printf("Liberando Todos\n");
            clear_memory();
            liberar_processos_threads();
            desbloqueio_threads();
        }
        for (i = 0; fscanf(_path_arq_t->fptr, " %500[^\n]s", str) != EOF && i < N; i++) {
            token = strtok(str, ",");
            for (int j = 0; token != NULL && j < QTD_COLLUN; j++) {
                strcpy(dataset_data[i][j], token);
                add_lst_info_distinct(&colun_date[j], token);
                token = strtok(NULL, ",");
            }
        }
        count += i;
    } while (count < N_TOTAL - 1);

	return;
}


int main ()
{
	int i, status;
	clock_t tempo;
	args _args[NUM_THREADS]; //numero de args por threads de CPU
	struct args_arq args_main;
	pthread_t thread_1; //thread responsavel pelo arquivo de entrada

    create_threads(_args, NUM_THREADS, arq_origem, &args_main, &control_process);
    thread_jobs(_args, QTD_COLLUN, NUM_THREADS, &args_main); //repassa trabalhos
    create_threads_mmory_set(&my_set_memory, N);
    //print_responsabilidade_thread(my_set_memory._my_set);
    print_responsabilidade_thread(_args);

    path_arq_t[0].fptr = open_arquivo(arq_origem, "r"); //path dataset

    tempo = clock();
    printf("\nEm execucao ...\n");

    calloc_memory_dataset(N);
    if (status_create( status = pthread_create((&thread_1), NULL, ler_matriz_entrada, (void *)&path_arq_t[0])));
    else exit(0xF);

    if (status_create( status = pthread_create((&args_main.thread), NULL, solicitacao_arquivo, (void *)&args_main)));
    else exit(0xF);

    /*Repassa função de trabalho*/
	for(i = 0; i < NUM_THREADS; i++) {
		if (status_create(status = pthread_create((&_args[i].thread), NULL, normaliza_colun_date, (void *)&_args[i])));
		else exit(0xFF);
	}

    /*Thread principal aguarda todas as thredes de trabalhos finalizarem*/
	for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(_args[i].thread, NULL);
	}
	pthread_join(thread_1, NULL);
	pthread_join(args_main.thread, NULL);
	printf("cheguei aqui\n");

	for (i = 0; i < NUM_THREADS; i++) {
        if (status_create( status = pthread_create((&_args[i].thread), NULL, solicitacao_arquivo_job, (void *)&_args[i])));
        else exit(0xF);
	}
    /*Thread principal aguarda todas as thredes de trabalhos finalizarem*/
	for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(_args[i].thread, NULL);
	}

    printf("\nTerminando processo ...\n");
	printf("\n\n[Tempo Total Do Processo: %fs]\n", (float) (clock() - tempo)  / CLOCKS_PER_SEC);
	return 0;
}
