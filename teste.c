#include <stdio.h>
#include <stdlib.h>

#include "grafo.h"

//------------------------------------------------------------------------------
int main (void) {
	grafo *g = le_grafo (stdin);
	if ( !g ) {
		fprintf (stderr, "Erro ao ler o grafo.\n");
		return 1;
	}
	calcula_componentes (g);
	char *s;

	printf ("grafo: %s\n", nome (g));
	printf ("%d vertices\n", n_vertices (g));
	printf ("%d arestas\n", n_arestas (g));
	printf ("%d componentes\n", n_componentes (g));

	printf ("%sbipartido\n", bipartido (g) ? "" : "não ");

	printf ("diâmetros: %s\n", s = diametros (g));

	printf ("vértices de corte: %s\n", s = vertices_corte (g));

	printf ("arestas de corte: %s\n", s = arestas_corte (g));
	free (s);
	// return 0;

	return !destroi_grafo (g);
}
