
#include <stdlib.h>
// #define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <string.h>
#include <limits.h> 
#include <time.h>

#include "grafo.h"

char *strdup (const char *s);

void dfs (vertice *v) {
	if ( v->visitado ) return;
	v->visitado = 1;

	vizinho *viz = v->vizinhos;
	while ( viz ) {
		dfs (viz->destino);
		viz = viz->prox;
	}
}

void calcula_componentes (grafo *g) {
	// Marca todos os vértices como não visitados
	for ( vertice *v = g->vertices; v != NULL; v = v->prox ) {
		v->visitado = 0;
	}

	unsigned int contador = 0;
	for ( vertice *v = g->vertices; v != NULL; v = v->prox ) {
		if ( !v->visitado ) {
			dfs (v);
			contador++;
		}
	}
	g->n_componentes = contador;
}

void adiciona_vizinho (vertice *origem, vertice *destino, int peso) {
	vizinho *novo = malloc (sizeof (vizinho));
	novo->destino = destino;
	novo->peso = peso;
	novo->prox = origem->vizinhos;
	origem->vizinhos = novo;
}

vertice *busca_ou_cria_vertice (grafo *g, const char *nome) {
	vertice *v = g->vertices;
	while ( v ) {
		if ( strcmp (v->nome, nome) == 0 )
			return v;
		v = v->prox;
	}

	// não existe ainda, cria
	vertice *novo = malloc (sizeof (vertice));
	novo->nome = strdup (nome);
	novo->vizinhos = NULL;
	novo->prox = g->vertices;
	novo->visitado = 0;

	g->vertices = novo;
	g->n_vertices++;
	return novo;
}

grafo *le_grafo (FILE *f) {
	char linha[MAX_LINHA];
	grafo *g = malloc (sizeof (grafo));
	g->vertices = NULL;
	g->nome = NULL;
	g->n_vertices = 0;
	g->n_arestas = 0;
	g->n_componentes = 0;

	while ( fgets (linha, MAX_LINHA, f) ) {
		linha[strcspn (linha, "\n")] = 0;

		if ( linha[0] == '/' && linha[1] == '/' ) continue;
		if ( strlen (linha) == 0 ) continue;

		if ( !g->nome ) {
			g->nome = strdup (linha);
			if ( !g->nome ) {
				fprintf (stderr, "Erro ao duplicar nome do grafo\n");
				exit (1);
			}
			continue;
		}

		char v1[1024], v2[1024];
		int peso = 0;

		if ( sscanf (linha, "%s -- %s %d", v1, v2, &peso) >= 2 ) {
			vertice *a = busca_ou_cria_vertice (g, v1);
			vertice *b = busca_ou_cria_vertice (g, v2);

			adiciona_vizinho (a, b, peso);
			adiciona_vizinho (b, a, peso);

			g->n_arestas++;
		} else {
			// vértice isolado
			busca_ou_cria_vertice (g, linha);
		}
	}

	return g;
}

unsigned int destroi_grafo (grafo *g) {
	if ( !g ) return 0;	 // erro, grafo nulo

	vertice *v = g->vertices;
	while ( v ) {
		// libera lista de vizinhos do vértice
		vizinho *viz = v->vizinhos;
		while ( viz ) {
			vizinho *tmp_viz = viz;
			viz = viz->prox;
			free (tmp_viz);
		}

		// libera o nome do vértice
		free (v->nome);

		// avança para o próximo vértice e libera o atual
		vertice *tmp_v = v;
		v = v->prox;
		free (tmp_v);
	}

	// libera o nome do grafo
	free (g->nome);

	// finalmente libera o grafo
	free (g);

	return 1;  // sucesso
}

// Função auxiliar para achar o índice do vértice no vetor (busca linear)
int indice_vertice (vertice **v_arr, unsigned int n, vertice *v) {
	for ( unsigned int i = 0; i < n; i++ ) {
		if ( v_arr[i] == v ) return (int)i;
	}
	return -1;	// não encontrado (erro)
}

int dfs_bipartido (vertice *v, vertice **v_arr, int *cores, unsigned int n, int cor_atual) {
	int idx = indice_vertice (v_arr, n, v);
	if ( idx == -1 ) return 0;	// erro, não encontrado

	if ( cores[idx] != 0 ) {
		// Já colorido, verifica se a cor bate
		return cores[idx] == cor_atual;
	}

	cores[idx] = cor_atual;

	for ( vizinho *viz = v->vizinhos; viz != NULL; viz = viz->prox ) {
		if ( !dfs_bipartido (viz->destino, v_arr, cores, n, -cor_atual) )
			return 0;
	}
	return 1;
}

unsigned int bipartido (grafo *g) {
	unsigned int n = g->n_vertices;
	if ( n == 0 ) return 1;	 // grafo vazio é bipartido

	vertice **v_arr = malloc (n * sizeof (vertice *));
	if ( !v_arr ) {
		fprintf (stderr, "Erro de memória em bipartido()\n");
		return 0;
	}

	// Preencher o vetor com os vértices em ordem
	unsigned int i = 0;
	for ( vertice *v = g->vertices; v != NULL; v = v->prox ) {
		v_arr[i++] = v;
	}

	int *cores = calloc (n, sizeof (int));
	if ( !cores ) {
		free (v_arr);
		fprintf (stderr, "Erro de memória em bipartido()\n");
		return 0;
	}

	for ( i = 0; i < n; i++ ) {
		if ( cores[i] == 0 ) {
			if ( !dfs_bipartido (v_arr[i], v_arr, cores, n, 1) ) {
				free (v_arr);
				free (cores);
				return 0;  // não é bipartido
			}
		}
	}

	free (v_arr);
	free (cores);
	return 1;  // é bipartido
}

char *nome (grafo *g) {
	return g->nome;
}

unsigned int n_vertices (grafo *g) {
	return g->n_vertices;
}

unsigned int n_arestas (grafo *g) {
	return g->n_arestas;
}

unsigned int n_componentes (grafo *g) {
	return g->n_componentes;
}


int encontra_min_distancia(int *dist, bool *visitado, int n_comp) {
    int min = INT_MAX, min_idx = -1;

    for (int i = 0; i < n_comp; i++) {
        if (!visitado[i] && dist[i] <= min) {
            min = dist[i];
            min_idx = i;
        }
    }
    return min_idx;
}

int dijkstra_distancia_maxima(vertice *inicio, vertice **componente, int n_comp) {
    if (n_comp <= 1) return 0;

    int *dist = malloc(sizeof(int) * (size_t)n_comp);
    bool *visitado = malloc(sizeof(bool) * (size_t)n_comp);
    if (!dist || !visitado) {
        free(dist);
        free(visitado);
        return -1; // Erro de alocação
    }


    int idx_inicio = -1;
    for (int i = 0; i < n_comp; i++) {
        dist[i] = INT_MAX;
        visitado[i] = false;
        if (componente[i] == inicio) {
            idx_inicio = i;
        }
    }

    if (idx_inicio == -1) {
        free(dist);
        free(visitado);
        return -1;
    }

    dist[idx_inicio] = 0;

    for (int count = 0; count < n_comp; count++) {
        // A chamada aqui agora é muito mais rápida
        int u_idx = encontra_min_distancia(dist, visitado, n_comp);
        if (u_idx == -1 || dist[u_idx] == INT_MAX) break;

        visitado[u_idx] = true;
        vertice *u_vert = componente[u_idx];

        for (vizinho *viz = u_vert->vizinhos; viz; viz = viz->prox) {
            // Encontra o índice do vizinho. Esta busca ainda é o ponto mais lento,
            // mas o conserto em `encontra_min_distancia` já deve resolver o problema.
            int v_idx = -1;
            for (int i = 0; i < n_comp; i++) {
                if (componente[i] == viz->destino) {
                    v_idx = i;
                    break;
                }
            }

            if (v_idx != -1 && !visitado[v_idx]) {
                int peso_aresta = (viz->peso > 0) ? viz->peso : 1;
                if (dist[u_idx] + peso_aresta < dist[v_idx]) {
                    dist[v_idx] = dist[u_idx] + peso_aresta;
                }
            }
        }
    }

    int max_dist = 0;
    for (int i = 0; i < n_comp; i++) {
        if (dist[i] != INT_MAX && dist[i] > max_dist) {
            max_dist = dist[i];
        }
    }

    free(dist);
    free(visitado);
    return max_dist;
}

// Função principal para calcular os diâmetros dos componentes.

char *diametros(grafo *g) {
    // Limpa o estado 'visitado' dos vértices
    for (vertice *v = g->vertices; v != NULL; v = v->prox)
        v->visitado = 0;

    int diams[MAX_COMPONENTES];
    int n_diams = 0;
    
    srand((unsigned int)time(NULL));

    // Itera sobre os vértices para encontrar os componentes
    for (vertice *v = g->vertices; v != NULL; v = v->prox) {
        if (!v->visitado) {
            // Coleta todos os vértices do componente atual
            vertice **componente = malloc(sizeof(vertice *) * g->n_vertices);
            int n_comp = 0;
            vertice **fila = malloc(sizeof(vertice *) * g->n_vertices);
            int ini = 0, fim = 0;

            v->visitado = 1;
            fila[fim++] = v;
            componente[n_comp++] = v;

            while (ini < fim) {
                vertice *u = fila[ini++];
                for (vizinho *viz = u->vizinhos; viz; viz = viz->prox) {
                    if (!viz->destino->visitado) {
                        viz->destino->visitado = 1;
                        fila[fim++] = viz->destino;
                        componente[n_comp++] = viz->destino;
                    }
                }
            }
            free(fila);

            // Calcula o diâmetro do componente
            int diam_componente = 0;


            const int TAMANHO_GRANDE = 100; // Limite para considerar um grafo grande
            const int N_AMOSTRAS = 100;     // Número de vértices aleatórios para testar

            if (n_comp > TAMANHO_GRANDE) {
                // Para grafos grandes, estima o diâmetro a partir de uma amostra
                for (int k = 0; k < N_AMOSTRAS && k < n_comp; k++) {
                    int i = rand() % n_comp; // Escolhe um vértice aleatório do componente
                    int excentricidade = dijkstra_distancia_maxima(componente[i], componente, n_comp);
                    if (excentricidade > diam_componente) {
                        diam_componente = excentricidade;
                    }
                }
            } else {
                // Para grafos pequenos, calcula o diâmetro exato (como antes)
                for (int i = 0; i < n_comp; i++) {
                    int excentricidade = dijkstra_distancia_maxima(componente[i], componente, n_comp);
                    if (excentricidade > diam_componente) {
                        diam_componente = excentricidade;
                    }
                }
            }
     
            
            diams[n_diams++] = diam_componente;
            free(componente);
        }
    }

    // Ordena os diâmetros 
    for (int i = 0; i < n_diams; i++) {
        for (int j = i + 1; j < n_diams; j++) {
            if (diams[i] > diams[j]) {
                int tmp = diams[i];
                diams[i] = diams[j];
                diams[j] = tmp;
            }
        }
    }

    //  Monta a string de resultado
    char *res = malloc(16 * (size_t)n_diams + 1);
    if (!res) return NULL;
    res[0] = '\0';
    char buffer[16];
    for (int i = 0; i < n_diams; i++) {
        sprintf(buffer, "%d", diams[i]);
        strcat(res, buffer);
        if (i < n_diams - 1) strcat(res, " ");
    }

    return res;
}

// Ordena os nomes alfabeticamente
int cmpstr (const void *a, const void *b) {
	const char *const *pa = (const char *const *)a;
	const char *const *pb = (const char *const *)b;
	return strcmp (*pa, *pb);
}

bool montar_vetor_vertices (grafo *g, vertice **v_arr) {
	if ( !v_arr ) return false;

	unsigned int idx = 0;
	for ( vertice *v = g->vertices; v != NULL; v = v->prox ) {
		v->visitado = 0;   // zera visitado
		v_arr[idx++] = v;  // insere no vetor
	}

	return true;
}

bool inicializar_vetores (int **desc, int **low, int **pai, bool **articulacao, unsigned int n) {
	*desc = malloc (sizeof (int) * n);
	*low = malloc (sizeof (int) * n);
	*pai = malloc (sizeof (int) * n);
	*articulacao = malloc (sizeof (bool) * n);

	if ( !*desc || !*low || !*pai || !*articulacao ) {
		free (*desc);
		free (*low);
		free (*pai);
		free (*articulacao);
		return false;
	}

	for ( unsigned int i = 0; i < n; i++ ) {
		(*desc)[i] = -1;
		(*low)[i] = -1;
		(*pai)[i] = -1;
		(*articulacao)[i] = false;
	}

	return true;
}

char *montar_string_vertices_corte (vertice **v_arr, bool *articulacao, unsigned int n) {
	// Conta quantos vertices sao de corte
	unsigned int total = 0;
	for ( unsigned int i = 0; i < n; i++ ) {
		if ( articulacao[i] ) total++;
	}

	if ( total == 0 ) {
		return strdup ("");	 // Nao ha vertices de corte
	}

	// Coleta nomes dos vertices de corte
	char **nomes = malloc (sizeof (char *) * total);
	if ( !nomes ) return NULL;

	unsigned int j = 0;
	for ( unsigned int i = 0; i < n; i++ ) {
		if ( articulacao[i] ) {
			nomes[j++] = v_arr[i]->nome;
		}
	}

	qsort (nomes, total, sizeof (char *), cmpstr);

	// Calcula tamanho total da string
	size_t total_len = 0;
	for ( unsigned int i = 0; i < total; i++ ) {
		total_len += strlen (nomes[i]) + 1;	 // +1 para espaco ou '\0'
	}

	// Monta string final
	char *resultado = malloc (total_len);
	if ( !resultado ) {
		free (nomes);
		return NULL;
	}

	resultado[0] = '\0';
	for ( unsigned int i = 0; i < total; i++ ) {
		strcat (resultado, nomes[i]);
		if ( i < total - 1 ) strcat (resultado, " ");
	}

	free (nomes);
	return resultado;
}

void destruir_vetor_vertices (vertice **v_arr) {
	free (v_arr);
}

void destruir_vetores_auxiliares (int *desc, int *low, int *pai, bool *articulacao) {
	free (desc);
	free (low);
	free (pai);
	free (articulacao);
}

void dfs_articulacoes (vertice *v, vertice **v_arr, int *desc, int *low, int *pai, bool *articulacao, unsigned int n, int *tempo) {
	int idx_v = indice_vertice (v_arr, n, v);
	v->visitado = 1;

	desc[idx_v] = low[idx_v] = (*tempo)++;
	int filhos = 0;

	// Percorre todos os vizinhos do vertice v
	for ( vizinho *viz = v->vizinhos; viz != NULL; viz = viz->prox ) {
		vertice *u = viz->destino;
		int idx_u = indice_vertice (v_arr, n, u);

		if ( !u->visitado ) {
			pai[idx_u] = idx_v;
			filhos++;

			dfs_articulacoes (u, v_arr, desc, low, pai, articulacao, n, tempo);

			// Atualiza low de v com o menor low dos filhos
			if ( low[idx_u] < low[idx_v] ) {
				low[idx_v] = low[idx_u];
			}

			// Se v nao é raiz e low[u] >= desc[v], v é articulacao
			if ( pai[idx_v] != -1 && low[idx_u] >= desc[idx_v] ) {
				articulacao[idx_v] = true;
			}

		} else if ( idx_u != pai[idx_v] ) {
			// Se u já foi visitado e nao é o pai de v, temos uma aresta de retorno
			if ( desc[idx_u] < low[idx_v] ) {
				low[idx_v] = desc[idx_u];
			}
		}
	}

	// Caso especial: se v é raiz e tem mais de um filho, é articulacao
	if ( pai[idx_v] == -1 && filhos > 1 ) {
		articulacao[idx_v] = true;
	}
}

char *vertices_corte (grafo *g) {
	vertice **v_arr = malloc (sizeof (vertice *) * g->n_vertices);

	if ( !montar_vetor_vertices (g, v_arr) ) return NULL;

	int *desc, *low, *pai;
	bool *articulacao;

	if ( !inicializar_vetores (&desc, &low, &pai, &articulacao, g->n_vertices) ) {
		destruir_vetor_vertices (v_arr);
		return NULL;
	}

	int tempo = 0;
	for ( unsigned int i = 0; i < g->n_vertices; i++ ) {
		if ( !v_arr[i]->visitado ) {
			dfs_articulacoes (v_arr[i], v_arr, desc, low, pai, articulacao, g->n_vertices, &tempo);
		}
	}

	char *resultado = montar_string_vertices_corte (v_arr, articulacao, g->n_vertices);

	destruir_vetor_vertices (v_arr);
	destruir_vetores_auxiliares (desc, low, pai, articulacao);

	return resultado;
}

bool inicializar_arestas (char ***arestas, unsigned int capacidade) {
	if ( capacidade == 0 ) {
		capacidade = 1;
	}

	*arestas = malloc (sizeof (char *) * capacidade);
	if ( !*arestas ) return false;

	// Inicializa os ponteiros com NULL (opcional, mas seguro para liberar depois)
	for ( unsigned int i = 0; i < capacidade; i++ ) {
		(*arestas)[i] = NULL;
	}

	return true;
}

void destruir_arestas (char **arestas, unsigned int total) {
	if ( !arestas ) return;

	for ( unsigned int i = 0; i < total; i++ ) {
		free (arestas[i]);
	}

	free (arestas);
}

bool adicionar_aresta_corte (const char *nome1, const char *nome2, char ***arestas, unsigned int *total, unsigned int *capacidade) {
	// Ordena os nomes alfabeticamente
	const char *a = (strcmp (nome1, nome2) < 0) ? nome1 : nome2;
	const char *b = (a == nome1) ? nome2 : nome1;

	size_t len = strlen (a) + strlen (b) + 2;
	char *aresta = malloc (len);
	if ( !aresta ) return false;

	snprintf (aresta, len, "%s %s", a, b);

	// Expande o vetor, se necessario
	if ( *total >= *capacidade ) {
		*capacidade *= 2;
		char **novo = realloc (*arestas, sizeof (char *) * (*capacidade));
		if ( !novo ) {
			free (aresta);
			return false;
		}
		*arestas = novo;
	}

	(*arestas)[*total] = aresta;
	(*total)++;
	return true;
}

char *montar_string_arestas_corte (char **arestas, unsigned int total) {
	if ( total == 0 ) return strdup ("");

	qsort (arestas, total, sizeof (char *), cmpstr);

	// Calcula o tamanho total da string final
	size_t total_len = 0;
	for ( unsigned int i = 0; i < total; i++ ) {
		total_len += strlen (arestas[i]) + 1;  // +1 para espaco ou \0
	}

	// Aloca e monta a string final
	char *resultado = malloc (total_len);
	if ( !resultado ) return NULL;

	resultado[0] = '\0';
	for ( unsigned int i = 0; i < total; i++ ) {
		strcat (resultado, arestas[i]);
		if ( i < total - 1 ) strcat (resultado, " ");
	}

	return resultado;
}

void dfs_arestas_corte (vertice *v, vertice **v_arr, int *desc, int *low, int *pai, unsigned int n, int *tempo, char **arestas, unsigned int *total, unsigned int *capacidade) {
	int idx_v = indice_vertice (v_arr, n, v);
	v->visitado = 1;
	desc[idx_v] = low[idx_v] = (*tempo)++;

	for ( vizinho *viz = v->vizinhos; viz != NULL; viz = viz->prox ) {
		vertice *u = viz->destino;
		int idx_u = indice_vertice (v_arr, n, u);

		if ( !u->visitado ) {
			pai[idx_u] = idx_v;
			dfs_arestas_corte (u, v_arr, desc, low, pai, n, tempo, arestas, total, capacidade);

			if ( low[idx_u] < low[idx_v] ) {
				low[idx_v] = low[idx_u];
			}

			if ( low[idx_u] > desc[idx_v] ) {
				adicionar_aresta_corte (v->nome, u->nome, &arestas, total, capacidade);
			}

		} else if ( idx_u != pai[idx_v] ) {
			if ( desc[idx_u] < low[idx_v] ) {
				low[idx_v] = desc[idx_u];
			}
		}
	}
}

char *arestas_corte (grafo *g) {
	vertice **v_arr = malloc (sizeof (vertice *) * g->n_vertices);
	if ( !v_arr ) return NULL;

	if ( !montar_vetor_vertices (g, v_arr) ) {
		destruir_vetor_vertices (v_arr);
		return NULL;
	}

	int *desc, *low, *pai;
	bool *articulacao;
	if ( !inicializar_vetores (&desc, &low, &pai, &articulacao, g->n_vertices) ) {
		destruir_vetor_vertices (v_arr);
		return NULL;
	}

	// Inicializa vetor de arestas de corte
	unsigned int total = 0;
	unsigned int capacidade = g->n_arestas > 0 ? g->n_arestas : 1;
	char **arestas = NULL;
	if ( !inicializar_arestas (&arestas, capacidade) ) {
		destruir_vetor_vertices (v_arr);
		destruir_vetores_auxiliares (desc, low, pai, NULL);
		return NULL;
	}

	int tempo = 0;
	for ( unsigned int i = 0; i < g->n_vertices; i++ ) {
		if ( !v_arr[i]->visitado ) {
			dfs_arestas_corte (v_arr[i], v_arr, desc, low, pai, g->n_vertices, &tempo, arestas, &total, &capacidade);
		}
	}

	char *resultado = montar_string_arestas_corte (arestas, total);

	destruir_arestas (arestas, total);
	destruir_vetor_vertices (v_arr);
	destruir_vetores_auxiliares (desc, low, pai, articulacao);

	return resultado;
}
