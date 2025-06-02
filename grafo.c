
#include <stdlib.h>
// #define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <string.h>

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

// BFS para calcular distância máxima a partir de um vértice
int bfs_distancia_maxima (vertice *inicio, grafo *g) {
	// Mapeia índices temporários para vértices
	vertice **vertices_array = malloc (sizeof (vertice *) * g->n_vertices);
	int index = 0;
	for ( vertice *v = g->vertices; v != NULL; v = v->prox ) {
		vertices_array[index++] = v;
		v->visitado = 0;
	}

	int *dist = malloc (sizeof (int) * g->n_vertices);
	for ( unsigned int i = 0; i < g->n_vertices; i++ ) dist[i] = -1;

	vertice **fila = malloc (sizeof (vertice *) * g->n_vertices);
	int ini = 0, fim = 0;

	inicio->visitado = 1;
	dist[0] = 0;  // distância do início até ele mesmo
	fila[fim++] = inicio;

	int max_dist = 0;

	while ( ini < fim ) {
		vertice *atual = fila[ini++];
		int atual_idx = -1;
		for ( unsigned i = 0; i < g->n_vertices; i++ ) {
			if ( vertices_array[i] == atual ) {
				atual_idx = (int)i;
				break;
			}
		}

		for ( vizinho *viz = atual->vizinhos; viz; viz = viz->prox ) {
			if ( !viz->destino->visitado ) {
				viz->destino->visitado = 1;
				for ( unsigned i = 0; i < g->n_vertices; i++ ) {
					if ( vertices_array[i] == viz->destino ) {
						dist[i] = dist[atual_idx] + 1;
						if ( dist[i] > max_dist ) max_dist = dist[i];
						fila[fim++] = viz->destino;
						break;
					}
				}
			}
		}
	}

	free (vertices_array);
	free (dist);
	free (fila);
	return max_dist;
}

char *diametros (grafo *g) {
	// Marca todos como não visitados
	for ( vertice *v = g->vertices; v != NULL; v = v->prox )
		v->visitado = 0;

	int diams[MAX_COMPONENTES];
	int n_diams = 0;

	for ( vertice *v = g->vertices; v != NULL; v = v->prox ) {
		if ( !v->visitado ) {
			// Descobrir todos do componente
			vertice **componente = malloc (sizeof (vertice *) * g->n_vertices);
			int n_comp = 0;

			// BFS para identificar o componente
			vertice **fila = malloc (sizeof (vertice *) * g->n_vertices);
			int ini = 0, fim = 0;

			v->visitado = 1;
			fila[fim++] = v;

			while ( ini < fim ) {
				vertice *u = fila[ini++];
				componente[n_comp++] = u;

				for ( vizinho *viz = u->vizinhos; viz; viz = viz->prox ) {
					if ( !viz->destino->visitado ) {
						viz->destino->visitado = 1;
						fila[fim++] = viz->destino;
					}
				}
			}

			// Para cada vértice do componente, calcula a distância máxima
			int diam = 0;
			for ( int i = 0; i < n_comp; i++ ) {
				for ( vertice *tmp = g->vertices; tmp; tmp = tmp->prox )
					tmp->visitado = 0;

				int dist = bfs_distancia_maxima (componente[i], g);
				if ( dist > diam ) diam = dist;
			}

			diams[n_diams++] = diam;

			free (fila);
			free (componente);
		}
	}

	// Ordena os diâmetros em ordem crescente (bubble sort simples)
	for ( int i = 0; i < n_diams; i++ ) {
		for ( int j = i + 1; j < n_diams; j++ ) {
			if ( diams[i] > diams[j] ) {
				int tmp = diams[i];
				diams[i] = diams[j];
				diams[j] = tmp;
			}
		}
	}

	// Monta a string de saída
	char *res = malloc (16 * (size_t)n_diams);	// tamanho suficiente
	res[0] = '\0';
	char buffer[16];
	for ( int i = 0; i < n_diams; i++ ) {
		sprintf (buffer, "%d", diams[i]);
		strcat (res, buffer);
		if ( i < n_diams - 1 ) strcat (res, " ");
	}

	return res;
}