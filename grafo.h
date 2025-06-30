#ifndef GRAFO_H
#define GRAFO_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
// estrutura de dados para representar um grafo

#define MAX_COMPONENTES 1024
#define MAX_LINHA 2048

typedef struct vertice {
	char *nome;
	struct vizinho *vizinhos;
	struct vertice *prox;
	int visitado;  // para BFS/DFS
} vertice;

typedef struct vizinho {
	vertice *destino;
	int peso;
	struct vizinho *prox;
} vizinho;

struct grafo {
	char *nome;
	vertice *vertices;
	unsigned int n_vertices;
	unsigned int n_arestas;
	unsigned int n_componentes;
};

typedef struct grafo grafo;

//------------------------------------------------------------------------------
// lê um grafo de f e o devolve
//
// um grafo é representado num arquivo por uma "string" que é o nome
// do grafo seguido de uma lista de vértices e arestas
//
// o nome do grafo, cada vértice e cada aresta aparecem numa linha por si só
// cada linha tem no máximo 2047 caracteres
// linhas iniciando por // são consideradas comentários e são ignoradas
//
// um vértice é representado por uma "string"
//
// uma aresta é representada por uma linha
// xxx -- yyy ppp
//
// onde xxx e yyy são nomes de vértices e ppp (opcional) é um int indicando o peso da aresta
//
// se um vértice faz parte de uma aresta, não é necessário nomeá-lo individualmente em uma linha
//
// a função supõe que a entrada está corretamente construída e não faz nenhuma checagem
// caso a entrada não esteja corretamente construída, o comportamento da função é indefinido
//
// abaixo, a título de exemplo, a representação de um grafo com quatro
// vértices e dois componentes, um K_3 e um K_1
/*

// o nome do grafo
triângulo_com_vértice

// uma lista com três arestas e seus pesos
um -- dois 12
dois -- quatro 24
quatro -- um 41

// um vértice isolado
três

*/

grafo *le_grafo (FILE *f);

//------------------------------------------------------------------------------
// desaloca toda a estrutura de dados alocada em g
//
// devolve 1 em caso de sucesso e 0 em caso de erro

unsigned int destroi_grafo (grafo *g);

//------------------------------------------------------------------------------
// devolve o nome de g

char *nome (grafo *g);

//------------------------------------------------------------------------------
// devolve 1 se g é bipartido e 0 caso contrário

unsigned int bipartido (grafo *g);

//------------------------------------------------------------------------------
// devolve o número de vértices em g

unsigned int n_vertices (grafo *g);

//------------------------------------------------------------------------------
// devolve o número de arestas em g

unsigned int n_arestas (grafo *g);

//------------------------------------------------------------------------------
// devolve o número de componentes em g

unsigned int n_componentes (grafo *g);

//------------------------------------------------------------------------------
// devolve uma "string" com os diâmetros dos componentes de g separados por brancos
// em ordem não decrescente

char *diametros (grafo *g);

//------------------------------------------------------------------------------
// devolve uma "string" com os nomes dos vértices de corte de g em
// ordem alfabética, separados por brancos

char *vertices_corte (grafo *g);

//------------------------------------------------------------------------------
// devolve uma "string" com as arestas de corte de g em ordem alfabética, separadas por brancos
// cada aresta é o par de nomes de seus vértices em ordem alfabética, separadas por brancos
//
// por exemplo, se as arestas de corte são {z, a}, {x, b} e {y, c}, a resposta será a string
// "a z b x c y"

char *arestas_corte (grafo *g);

//------------------------------------------------------------------------------
// funcoes criadas

/**
 * Calcula o numero de componentes conexas em um grafo nao direcionado.
 *
 * @param g Ponteiro para o grafo cuja conectividade sera analisada.
 *
 * Atualiza o campo 'n_componentes' do grafo com o numero de componentes conexas.
 */
void calcula_componentes (grafo *g);

/**
 * Executa a busca em profundidade (DFS) a partir de um vertice dado.
 *
 * @param v Ponteiro para o vertice inicial da busca.
 *
 * Marca todos os vertices alcancaveis a partir de 'v' como visitados.
 */
void dfs (vertice *v);

/**
 * Retorna o indice de um vertice em um vetor de ponteiros para vertices.
 *
 * @param v_arr Vetor de ponteiros para vertices.
 * @param n Numero de elementos no vetor.
 * @param v Ponteiro para o vertice que se deseja encontrar.
 *
 * @return Indice do vertice no vetor, ou -1 se nao for encontrado.
 */
int indice_vertice (vertice **v_arr, unsigned int n, vertice *v);

/**
 * Executa uma busca em profundidade (DFS) para verificar se o grafo e bipartido.
 *
 * @param v Vertice atual da busca.
 * @param v_arr Vetor com todos os vertices do grafo.
 * @param cores Vetor de inteiros representando as cores dos vertices (0 = nao visitado, 1 e -1 = cores diferentes).
 * @param n Numero total de vertices.
 * @param cor_atual Cor atribuida ao vertice atual (1 ou -1).
 *
 * @return 1 se for possivel colorir o grafo de forma bipartida a partir deste vertice, 0 caso contrario.
 */
int dfs_bipartido (vertice *v, vertice **v_arr, int *cores, unsigned int n, int cor_atual);

/**
 * Calcula a maior distancia (em numero de arestas) a partir de um vertice usando busca em largura (BFS).
 *
 * @param inicio Ponteiro para o vertice de inicio da busca.
 * @param g Ponteiro para o grafo onde a busca sera realizada.
 *
 * @return Valor inteiro correspondente a maior distancia encontrada a partir do vertice inicial.
 */
int bfs_distancia_maxima(vertice *inicio, vertice **componente, int n_comp) ;

/**
 * Adiciona um novo vizinho (aresta) ao vertice de origem.
 *
 * @param origem Ponteiro para o vertice de origem da aresta.
 * @param destino Ponteiro para o vertice de destino da aresta.
 * @param peso Peso associado a aresta.
 *
 * Cria uma nova estrutura de vizinho e a insere no inicio da lista de vizinhos do vertice de origem.
 */
void adiciona_vizinho (vertice *origem, vertice *destino, int peso);

/**
 * Busca um vertice pelo nome no grafo. Caso nao exista, cria um novo vertice com esse nome.
 *
 * @param g Ponteiro para o grafo onde sera feita a busca ou insercao.
 * @param nome Nome do vertice a ser buscado ou criado.
 *
 * @return Ponteiro para o vertice encontrado ou recem-criado.
 */
vertice *busca_ou_cria_vertice (grafo *g, const char *nome);

/**
 * Preenche um vetor com todos os vertices do grafo e zera o campo 'visitado' de cada um.
 *
 * @param g Ponteiro para o grafo que contem os vertices.
 * @param v_arr Vetor a ser preenchido com os ponteiros para os vertices do grafo.
 *
 * @return true se o vetor foi preenchido com sucesso, false em caso de erro.
 *
 * O vetor deve ter tamanho igual a g->n_vertices.
 */
bool montar_vetor_vertices (grafo *g, vertice **v_arr);

/**
 * Aloca e inicializa os vetores auxiliares usados na DFS para encontrar vertices de corte.
 *
 * @param desc Ponteiro para o vetor de tempos de descoberta dos vertices.
 * @param low Ponteiro para o vetor de menores tempos acessiveis.
 * @param pai Ponteiro para o vetor de pais dos vertices na DFS.
 * @param articulacao Ponteiro para o vetor booleano que marca vertices de corte.
 * @param n Numero de vertices no grafo.
 *
 * @return true se todos os vetores foram alocados corretamente, false em caso de erro.
 *
 * Todos os vetores sao inicializados com valores padrao:
 * - desc, low, pai com -1
 * - articulacao com false
 */
bool inicializar_vetores (int **desc, int **low, int **pai, bool **articulacao, unsigned int n);

/**
 * Monta uma string com os nomes dos vertices de corte em ordem alfabetica, separados por espacos.
 *
 * @param v_arr Vetor com todos os vertices do grafo.
 * @param articulacao Vetor booleano indicando quais vertices sao de corte.
 * @param n Numero total de vertices no grafo.
 *
 * @return Uma string alocada dinamicamente com os nomes dos vertices de corte separados por espaco.
 *         - Retorna NULL em caso de erro de alocacao.
 *         - Retorna uma string vazia ("") se nenhum vertice for de corte.
 *
 * A string retornada deve ser liberada com free pelo chamador.
 */
char *montar_string_vertices_corte (vertice **v_arr, bool *articulacao, unsigned int n);

/**
 * Libera o vetor de ponteiros para vertices.
 *
 * @param v_arr Vetor alocado dinamicamente contendo os ponteiros para os vertices do grafo.
 *
 * Esta funcao nao libera os vertices em si, apenas o vetor que os armazena.
 */
void destruir_vetor_vertices (vertice **v_arr);

/**
 * Libera os vetores auxiliares utilizados na DFS para identificacao de vertices de corte.
 *
 * @param desc Vetor de tempos de descoberta.
 * @param low Vetor de menores tempos alcançaveis.
 * @param pai Vetor de pais dos vertices na DFS.
 * @param articulacao Vetor booleano que indica os vertices de corte.
 *
 * Todos os ponteiros devem ter sido alocados dinamicamente.
 */
void destruir_vetores_auxiliares (int *desc, int *low, int *pai, bool *articulacao);

/**
 * Executa uma DFS para identificar vertices de corte, atualizando desc, low, pai e articulacao.
 *
 * @param v Vertice atual.
 * @param v_arr Vetor com todos os vertices.
 * @param desc Vetor de tempos de descoberta.
 * @param low Vetor de menores tempos acessiveis.
 * @param pai Vetor de pais na DFS.
 * @param articulacao Vetor booleano de vertices de corte.
 * @param n Numero total de vertices.
 * @param tempo Ponteiro para o contador de tempo global.
 */
void dfs_articulacoes (vertice *v, vertice **v_arr, int *desc, int *low, int *pai, bool *articulacao, unsigned int n, int *tempo);

int cmpstr (const void *a, const void *b);

/**
 * Inicializa o vetor de strings para armazenar arestas de corte.
 *
 * @param arestas Ponteiro para o vetor de strings que sera alocado.
 * @param capacidade Numero inicial estimado de arestas. Se for 0, aloca capacidade 1.
 *
 * @return true se a alocacao foi bem-sucedida, false em caso de erro.
 */
bool inicializar_arestas (char ***arestas, unsigned int capacidade);

/**
 * Libera todas as strings do vetor de arestas e o proprio vetor.
 *
 * @param arestas Vetor de strings representando as arestas de corte.
 * @param total Quantidade de strings preenchidas no vetor.
 */
void destruir_arestas (char **arestas, unsigned int total);

/**
 * Cria uma string com os nomes dos vertices ordenados no formato "a b" e adiciona ao vetor de arestas.
 *
 * @param nome1 Nome de um dos vertices da aresta.
 * @param nome2 Nome do outro vertice da aresta.
 * @param arestas Ponteiro para o vetor de strings onde a aresta sera armazenada.
 * @param total Ponteiro para o contador atual de arestas armazenadas.
 * @param capacidade Ponteiro para a capacidade atual do vetor de arestas (sera dobrada se necessario).
 *
 * @return true se a aresta foi criada e armazenada com sucesso, false em caso de erro de alocacao ou realocacao.
 */
bool adicionar_aresta_corte (const char *nome1, const char *nome2, char ***arestas, unsigned int *total, unsigned int *capacidade);

/**
 * Monta uma string unica contendo todas as arestas de corte ordenadas, separadas por espacos.
 *
 * @param arestas Vetor de strings, cada uma representando uma aresta no formato "a b".
 * @param total Quantidade de arestas armazenadas no vetor.
 *
 * @return Uma string alocada dinamicamente com as arestas ordenadas e separadas por espacos.
 *         Retorna "" se nao houver arestas, ou NULL em caso de erro de alocacao.
 */
char *montar_string_arestas_corte (char **arestas, unsigned int total);

/**
 * Executa uma DFS para identificar arestas de corte (pontes) e armazena-las como strings no formato "a b".
 *
 * @param v Vertice atual.
 * @param v_arr Vetor com todos os vertices do grafo.
 * @param desc Vetor de tempos de descoberta dos vertices.
 * @param low Vetor de menores tempos alcançaveis.
 * @param pai Vetor de pais dos vertices na DFS.
 * @param n Numero total de vertices.
 * @param tempo Ponteiro para o contador de tempo global.
 * @param arestas Vetor de strings onde as arestas de corte serao armazenadas.
 * @param total Ponteiro para a quantidade atual de arestas armazenadas.
 * @param capacidade Ponteiro para a capacidade atual do vetor de arestas.
 */
void dfs_arestas_corte (vertice *v, vertice **v_arr, int *desc, int *low, int *pai, unsigned int n, int *tempo, char **arestas, unsigned int *total, unsigned int *capacidade);
int dijkstra_distancia_maxima(vertice *inicio, vertice **componente, int n_comp);
int encontra_min_distancia(int *dist, bool *visitado, int n_comp);

#endif
