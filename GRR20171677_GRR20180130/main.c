#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct TransactionT {
    int tempo;
    int identificador;
    char operacao;
    char atributo;
} TransactionT;

typedef int EscalonadorT;

TransactionT *criaTransacao(int tempo, int identificador, char operacao, char atributo)
{
    TransactionT *transacao = (TransactionT *)malloc(sizeof(TransactionT) * 50);

    transacao->tempo = tempo;
    transacao->identificador = identificador;
    transacao->operacao = operacao;
    transacao->atributo = atributo;

    return transacao;
}

void mostraTransacao(TransactionT *transacao)
{
    printf("tempo = %d ", transacao->tempo);
    printf("identificador = %d ", transacao->identificador);
    printf("operacao = %c ", transacao->operacao);
    printf("atributo = %c\n", transacao->atributo);
}

int id_esta_no_array(int *array_ids, int id, int tamanho)
{
    for (int i = 0; i < tamanho; i++) {
        if (array_ids[i] == id) {
            return 1;
        }
    }
    return 0;
}

int operacao_escrita(TransactionT *transacao)
{
    return transacao->operacao == 'W' ? 1 : 0;
}

int operacao_leitura(TransactionT *transacao)
{
    return transacao->operacao == 'R' ? 1 : 0;
}

int operacao_commit(TransactionT *transacao)
{
    return transacao->operacao == 'C' ? 1 : 0;
}

TransactionT **leituraArquivo(int *limiar, EscalonadorT ***escalonador, int *numero_escalonacoes) 
{
    TransactionT **transacoes;
    transacoes = (TransactionT **)malloc((*limiar) * sizeof(TransactionT *));

    int tempo;
    int identificador;
    char operacao;
    char atributo;
    
    int tamanho_array = 10; // controla o tamanho total do array 
    int indice_array_atual = 0; // controla o indice atual do array
    int numero_commits = 0; //controla numero de commits
    int escalonacao = 0; //controla a escalonacao atual
    (*escalonador)[escalonacao] = (int *)malloc(sizeof(int) * tamanho_array);

    int indice = 0;
    while(scanf("%d", &tempo) != EOF) {
        fscanf(stdin, "%d %c %c", &identificador, &operacao, &atributo);

        if (tempo >= (*limiar)) {
            (*limiar) *= 2;
            transacoes = (TransactionT **)realloc(transacoes,(*limiar) * sizeof(TransactionT *));

            if (!transacoes) {
                printf("Problema ao realocar a memória\n");
                exit(0);
            }
        }
        TransactionT *transacao = criaTransacao(tempo, identificador, operacao, atributo);
        transacoes[indice] = transacao;

        if (operacao_commit(transacoes[indice]) == 1) {
            numero_commits--;
        }
        if (!id_esta_no_array((*escalonador)[escalonacao], transacao->identificador, tamanho_array)) {
            if (indice_array_atual >= tamanho_array) {
                tamanho_array *= 2;
                (*escalonador)[escalonacao] = (int *)realloc(escalonador[escalonacao], (tamanho_array) * sizeof(int));
            }
            numero_commits++;
            (*escalonador)[escalonacao][indice_array_atual] = transacao->identificador;
            indice_array_atual++;
        }
        

        if (numero_commits == 0) {
            (*escalonador)[escalonacao][indice_array_atual] = -1;
            
            
            tamanho_array = 10; // controla o tamanho total do array 
            indice_array_atual = 0; // controla o indice atual do array
            numero_commits = 0; //controla numero de commits
            escalonacao++; //controla a escalonacao atual
            
            (*escalonador)[escalonacao] = (int *)malloc(sizeof(int) * tamanho_array);
        }
        indice++;
    }

    (*numero_escalonacoes) = escalonacao;
    (*limiar) = indice;
    return transacoes;
}

void imprime_grafo(int **grafo, int tamanho)
{
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {
            printf("%d \t", grafo[i][j]);
        }
        printf("\n");
    }
}


int mesmo_atributo(TransactionT *transacao_a, TransactionT *transacao_b)
{
    return transacao_a->atributo == transacao_b->atributo ? 1 : 0;
}

int mesmo_identificador(TransactionT *transacao_a, TransactionT *transacao_b)
{
    return transacao_a->identificador == transacao_b->identificador ? 1 : 0;
}

int calcula_tamanho_array(int *array) 
{
    int i;
    for (i = 0; array[i] != -1; i++);

    return i;
}

int **aloca_grafo(int numero_vertices, int *tarefa_escalonada, int numero_transacoes, TransactionT **transacoes)
{
    int **grafo = (int **)calloc(numero_vertices, sizeof(int *));
    for (int i = 0; i < numero_vertices; i++) {
        grafo[i] = (int *)calloc(numero_vertices, sizeof(int));

        for (int j = 0; j < numero_vertices; j++) {
            grafo[i][j] = 0;
        }
    }

    for (int j = 0; j < numero_transacoes; j++) {
        TransactionT *transacao_a = transacoes[j];
        if (!id_esta_no_array(tarefa_escalonada, transacao_a->identificador, numero_vertices) || operacao_commit(transacao_a)) {
            continue;
        } 

        for (int k = j + 1; k < numero_transacoes; k++) {
            TransactionT *transacao_b = transacoes[k];
            if (!id_esta_no_array(tarefa_escalonada, transacao_b->identificador, numero_vertices) || operacao_commit(transacao_b)) {
                continue;
            }

            if (
                transacao_a->atributo == transacao_b->atributo &&
                transacao_a->identificador != transacao_b->identificador &&
                (operacao_escrita(transacao_a) || operacao_escrita(transacao_b))

            ){
                grafo[transacao_a->identificador - 1][transacao_b->identificador - 1] = 1;
            }
        }
    }

    return grafo;
    
}

int conta_tamanho_array(int *array) 
{
    int j;
    for (j = 0; array[j] != -1; j++) {
    }

    return j;
}

void troca_ordem_id(int *inicio, int *fim) 
{
    int tmp = (*fim);
    (*fim) = (*inicio);
    (*inicio) = tmp;
}

int mesmo_objeto(TransactionT *transacao_a, TransactionT *transacao_b)
{
    if (transacao_a->atributo != transacao_b->atributo) {
        return 0;
    }
    if (transacao_a->identificador != transacao_b->identificador) {
        return 0;
    }
    if (transacao_a->operacao != transacao_b->operacao) {
        return 0;
    }
    if (transacao_a->tempo != transacao_b->tempo) {
        return 0;
    }

    return 1;
}

int testa_visao_equivalente(TransactionT **transacoes, int tamanho_transacoes, int *escalonacao, int tamanho_escalonacao)
{
    // regra 1 vem de brinde, ja sabemos que aquilo é verdade
    TransactionT **transacaoOrdenada = (TransactionT **)malloc(tamanho_transacoes * sizeof(TransactionT *));
    int k = 0;
    for (int i = 0 ; i < tamanho_escalonacao; i++) {
        int id = escalonacao[i];

        for (int j = 0; j < tamanho_transacoes; j++) {
            if (transacoes[j]->identificador == id && !operacao_commit(transacoes[j])) {
                transacaoOrdenada[k] = transacoes[j];
                k++;
            }

        }
    }

    // Para cada r(x) de Ti, se o valor de x lido for escrito por w(x) de Tj, o mesmo deve permanecer para r(x) de Ti em S'
    for (int i = 0; i < k; i++) {
        if (operacao_escrita(transacaoOrdenada[i])) {
            continue;
        }
        for (int j = 0; j < k; j++) {
            if (
                operacao_leitura(transacaoOrdenada[j]) || 
                transacaoOrdenada[i]->identificador == transacaoOrdenada[j]->identificador ||
                transacaoOrdenada[i]->atributo != transacaoOrdenada[j]->atributo
            ) {
                continue;
            }

            if (transacaoOrdenada[j]->tempo > transacaoOrdenada[i]->tempo) {
                return 0;
            }

        }
    }

    // printf("chego aqui\n");
    // return 0;
    //Se o operador w(y) em Tk é a ultima escrita de y em S, então w(y) em Tk deve ser a última escrita em S'
    TransactionT *ultima_operacao = transacaoOrdenada[k - 1];
    TransactionT *ultima_operacao_normal = transacoes[0];
    for(int i = 0; i < tamanho_transacoes; i++) {
        if (id_esta_no_array(escalonacao, transacoes[i]->identificador, tamanho_escalonacao) && !operacao_commit(transacoes[i])) {
            ultima_operacao_normal = transacoes[i];
        }
    }

    if (operacao_escrita(ultima_operacao_normal)) {
        if (mesmo_objeto(ultima_operacao, ultima_operacao_normal)) {
            return 1;

        }
        return 0;
    }

    return 1;

}


int visao_equivalente(int *escalonacao, int inicio, int fim, TransactionT **transacoes, int quantidade_transacoes)
{
    if (inicio != fim) {
        for (int i = inicio; i <= fim; i++) {
            troca_ordem_id(&escalonacao[inicio], &escalonacao[i]);
            int resultado_visao_equivalente = visao_equivalente(escalonacao, inicio + 1 , fim, transacoes, quantidade_transacoes);
            troca_ordem_id(&escalonacao[inicio], &escalonacao[i]);
            if (resultado_visao_equivalente == 1) {
                return resultado_visao_equivalente;
            }
        }
    }else {
        int resultado_visao_equivalente = testa_visao_equivalente(transacoes, quantidade_transacoes, escalonacao, fim + 1);

        if (resultado_visao_equivalente) {
            return resultado_visao_equivalente;
        }
    }


    return 0;
}

int vertice_visitado(int *nos_visitados, int vertice)
{
    for (int i = 0; nos_visitados[i] != -2; i++) {
        if (nos_visitados[i] == vertice) {
            return 1;
        }
    }
    
    return 0;
}

void adiciona_vertice_nos_visitados(int **nos_visitados, int vertice)
{
    int i;
    for (i = 0; (*nos_visitados)[i] != -1; i++) {
    }

    (*nos_visitados)[i] = vertice;
}

int *pega_vizinhos_vertices(int **grafo, int vertice, int tamanho_grafo, int *num_vizinhos)
{
    int *vizinhos = (int *)malloc(tamanho_grafo * sizeof(int));
    for (int i = 0; i < tamanho_grafo; i++) {
        if (grafo[vertice][i] > 0) {
            vizinhos[(*num_vizinhos)] = i;
            (*num_vizinhos)++;
        }
    }

    return vizinhos;
}

int *aloca_array_visitados(int tamanho)
{
    int *array = (int *)malloc((tamanho + 1) * sizeof(int));

    for (int i = 0; i < tamanho; i++) {
        array[i] = -1;
    }

    array[tamanho] = -2;

    return array;
}

int tem_ciclo(int **grafo, int vertice, int tamanho_grafo, int *nos_visitados)
{
    if (vertice_visitado(nos_visitados, vertice)) {
        return 1;
    }
    adiciona_vertice_nos_visitados(&nos_visitados, vertice);

    int *nos_visitados_copia = aloca_array_visitados(tamanho_grafo * tamanho_grafo);

    for(int i = 0; nos_visitados[i] != -2; i++) {
        nos_visitados_copia[i] = nos_visitados[i];
    }

    int resultado = 0;
    int num_vizinhos = 0;
    int *vizinhos = pega_vizinhos_vertices(grafo, vertice, tamanho_grafo, &num_vizinhos);
    // para cada vizinho, chamo busca em profundidade
    for (int i = 0; i < num_vizinhos; i++) {
        resultado = tem_ciclo(grafo, vizinhos[i], tamanho_grafo, nos_visitados_copia);
        if (resultado == 1) {
            return resultado;
        }
    }

    return resultado;
}

int main() {
    int tamanho = 10;
    int numero_escalonacoes = 100;
    
    int **escalonadores = (int **)malloc(sizeof(int *) * numero_escalonacoes);

    int **grafo;

    TransactionT **transacoes = leituraArquivo(&tamanho, &escalonadores, &numero_escalonacoes);

    for (int i = 0; i < numero_escalonacoes; i++) {
        int *tarefa_escalonada = escalonadores[i];
        int tamanho_array = conta_tamanho_array(tarefa_escalonada);
        int k = 0;
        printf("%d ", i + 1);
        for (k = 0; k < tamanho_array - 1; k++) {
            printf("%d,", tarefa_escalonada[k]);
        }
        printf("%d ", tarefa_escalonada[k]);


        grafo = aloca_grafo(tamanho_array, tarefa_escalonada, tamanho, transacoes);

        int *nos_visitados = aloca_array_visitados(tamanho_array * tamanho_array);
        
        int ciclo = tem_ciclo(grafo, 0, tamanho_array, nos_visitados);
        if (ciclo) {
            printf("NS ");
        }else{
            printf("SS ");
        }

        if (visao_equivalente(tarefa_escalonada, 0, tamanho_array - 1, transacoes, tamanho)) {
            printf("SV");
        }else {
            printf("NV");
        }

        if (i != numero_escalonacoes -1 ){
            printf("\n");

        }
    }

}