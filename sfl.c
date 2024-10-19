#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stddef.h>

typedef struct node_t {//structura pt nod
	size_t adr;
	void *data;
	unsigned int size_node;
	struct node_t *prev, *next;
} node_t;

typedef struct dll_t {//structura pt lista dublu inlantuita
	node_t *head;
	unsigned int size_node;
	unsigned int dim;
} dll_t;

typedef struct heap_t {//structura pt heap
	size_t start;//adresa de start
	unsigned int nrl, nrb, tip, total;//nr de liste,bytes si tipul
	dll_t **list;
	unsigned int nrmal, nrfrg, nrfree;//nr de mallocuri,fragmentari si free
} heap_t;

dll_t *create(unsigned int bytes)
{//fct care imi creaza o lista cu noduri de dim bytes
	dll_t *list = malloc(sizeof(dll_t));
	list->head = NULL;
	list->dim = 0;
	list->size_node = bytes;
	return list;
}

void
add_nth_node(dll_t *list, unsigned int n, size_t adr, unsigned int size_node)
{//fct care imi adauga un nod pe pozitia n in lista
	node_t *new_node = malloc(sizeof(node_t));
	new_node->adr = adr;
	new_node->size_node = size_node;
	if (n > list->dim)
		n = list->dim;
	if (n == 0 || !list->head) {
		if (list->head)
			list->head->prev = new_node;
		new_node->next = list->head;
		list->head = new_node;
		new_node->prev = NULL;
		list->dim++;
		new_node->data = NULL;
		return;
	}
	node_t *node = list->head;
	for (unsigned int i = 0; i < n - 1; i++)
		node = node->next;
	new_node->next = node->next;
	if (node->next)
		node->next->prev = new_node;
	node->next = new_node;
	new_node->prev = node;
	new_node->data = NULL;
	list->dim++;
}

node_t *remove_nth_node(dll_t *list, unsigned int n)
{//fct care imi scoate al n-lea nod din lista
	unsigned int i;
	node_t *old_node;
	node_t *node = list->head;
	if (n == 0) {
		old_node = node;
		list->head = node->next;
		if (list->dim > 1)
			list->head->prev = NULL;
		list->dim--;
		return old_node;
	}
	for (i = 0; i < n - 1 ; i++)
		node = node->next;
	old_node = node->next;
	node->next = node->next->next;
	if (node->next)
		node->next->prev = node;
	list->dim--;
	return old_node;
}

node_t *get_nth_node(dll_t *list, unsigned int n)
{//fct care imi returneaza nodul de pe pozitia n
	if (n == 0)
		return list->head;
	if (n >= list->dim)
		n = n % list->dim;
	node_t *node = list->head;
	for (unsigned int i = 0; i < n - 1; i++)
		node = node->next;
	return node->next;
}

void init_heap(heap_t *heap)
{//fct de initializare a heapului
	unsigned int adr;
	scanf("%lx%d%d%d", &heap->start, &heap->nrl, &heap->nrb, &heap->tip);
	unsigned int bytes = 8;
	adr = heap->start;
	heap->list = malloc(heap->nrl * sizeof(dll_t *));
	heap->nrfree = 0;
	heap->nrfrg = 0;
	heap->nrmal = 0;
	for (unsigned int i = 0; i < heap->nrl; i++) {
		heap->list[i] = create(bytes);//creez cate o lista de nrl ori
		for (unsigned int j = 0; j < heap->nrb / bytes; j++) {
			add_nth_node(heap->list[i], j, adr, bytes);
			adr += bytes;
		}
		bytes *= 2;//maresc size ul nodurilor din lista viitoare
	}
	heap->total = heap->nrl * heap->nrb;//la final calc memoria totala
}

void free_list(dll_t *pp_list)
{//fct care imi elibereaza tot ceea ce contine lista,inclusiv pe ea
	node_t *node = pp_list->head;
	do {
		node_t *aux = node;
		node = node->next;
		free(aux->data);
		free(aux);
	} while (node);
	free(pp_list);
}

void alloc(heap_t *heap, dll_t *list)
{//fct de malloc
	unsigned int bytes, i, j, ok = 0;
	scanf("%d", &bytes);
	for (i = 0; i < heap->nrl; i++)
		if (heap->list[i]->size_node >= bytes)
			break;//daca gaseste pozitia ceruta
	if (i == heap->nrl) {//daca nu a gasit
		printf("Out of memory\n");
		return;
	} //daca pozitia corespunzatoare contine 0 noduri in lista,cautam in cont
	if (heap->list[i]->dim == 0)
		while (i < heap->nrl) {
			if (heap->list[i]->dim == 0)
				i++;
			else
				break;
		}
	if (i == heap->nrl) {//daca nu a gasit
		printf("Out of memory\n");
		return;
	}
	if (heap->list[i]->size_node == bytes) {
		//daca mi a gasit lista cu noduri corespunzatoare,scot pe primul
		node_t *node = remove_nth_node(heap->list[i], 0);
		for (j = 0; j < list->dim; j++) {
			if (get_nth_node(list, j)->adr > node->adr) {
				add_nth_node(list, j, node->adr, bytes);
				break;//il bag in lista de alocate ca sa fie crescatoare
			}
		}
		if (j == list->dim)//il bagam la final daca este cel mai mare
			add_nth_node(list, list->dim, node->adr, bytes);
		free(node);//eliberam nodul scos
	} else if (heap->list[i]->size_node != bytes) {//fragmentare
		node_t *node = remove_nth_node(heap->list[i], 0);
		int aux_adr = bytes;//scoatem din heap si calc noua valoare a adresei
		bytes = heap->list[i]->size_node - bytes;
		for (i = 0; i < heap->nrl; i++)
			if (heap->list[i]->size_node == bytes) {
				node_t *aux_node = heap->list[i]->head;
				for (j = 0; j < heap->list[i]->dim; j++) {
					if (node->adr + aux_adr < aux_node->adr)
						break;//punem in ordine crescatoare
					aux_node = aux_node->next;
				}
				add_nth_node(heap->list[i], j, node->adr + aux_adr, bytes);
				ok = 1;
				heap->nrfrg++;//am verif daca dupa fragmentare,putem pune
				break;//ce a ramas nealocat intr o lista existenta
			}
		if (ok == 0) {//altfel,creeam una noua care sa fie in loc crescator
			for (i = 0; i < heap->nrl; i++)
				if (bytes < heap->list[i]->size_node)
					break;
			heap->nrl++;//marim dimensiunea vectorului de liste
			unsigned int x = heap->nrl;
			heap->list = realloc(heap->list, x * sizeof(dll_t *));
			for (j = heap->nrl - 1; j > i; j--)
				heap->list[j] = heap->list[j - 1];
			heap->list[i] = create(bytes);
			add_nth_node(heap->list[i], 0, node->adr + aux_adr, bytes);
			heap->nrfrg++;
		}
	for (j = 0; j < list->dim; j++) {
		if (get_nth_node(list, j)->adr > node->adr) {
			add_nth_node(list, j, node->adr, aux_adr);
			break;//bagam in lista de alocate in ordine crescatoare
		}
	}
	if (j == list->dim)//punem la final daca e maxim
		add_nth_node(list, list->dim, node->adr, aux_adr);
	free(node);
	}
	heap->nrmal++;
}

void free_b(heap_t *heap, dll_t *list)
{//functia de free din lista de alocate
	unsigned int adr, i, j;
	scanf("%x", &adr);
	node_t *node = list->head;
	for (i = 0; i < list->dim; i++) {
		if (node->adr == adr)
			break;
		node = node->next;
	} //daca nu am gasit adresa ceruta
	if (i == list->dim) {
		printf("Invalid free\n");
		return;
	}
	node = remove_nth_node(list, i);
	for (i = 0; i < heap->nrl; i++) {
		if (node->size_node <= heap->list[i]->size_node)
			break;//dupa ce am scos,cautam sa punem inapoi unde era
	}
	if (node->size_node < heap->list[i]->size_node) {
		heap->nrl++;//daca nu mai exista acel loc, atunci cream o lista cresc
		unsigned int x = heap->nrl;
		heap->list = realloc(heap->list, x * sizeof(dll_t *));
		for (j = heap->nrl - 1; j > i; j--)//mutam restul elem din vect
			heap->list[j] = heap->list[j - 1];//de la poz i cu una la dreapta
		heap->list[i] = create(node->size_node);
		add_nth_node(heap->list[i], 0, node->adr, node->size_node);
		heap->nrfree++;//bagam in locul ramas liber
		free(node->data);
		free(node);
		return;
	}
	node_t *aux_node = heap->list[i]->head;
	for (j = 0; j < heap->list[i]->dim; j++) {
		if (node->adr < aux_node->adr) {
			add_nth_node(heap->list[i], j, node->adr, node->size_node);
			break;//altfel,bagam inapoi in lista ai. sa fie cresc
		}
		aux_node = aux_node->next;
	}
	if (j == heap->list[i]->dim) {
		size_t x = node->adr;
		unsigned int y = node->size_node;//sau bagam la finalul listei
		add_nth_node(heap->list[i], heap->list[i]->dim, x, y);
	}
	free(node->data);
	free(node);
	heap->nrfree++;
}

int write(dll_t *list)
{//fct de write
	unsigned int adr, bytes, k, i, ok = 0;
	char line[666], aux[666];//auxilara pt copiere
	scanf("%x", &adr);//adresa
	fgets(line, 666, stdin);
	strcpy(aux, line + 1);//am eliminat primul spatiu
	strcpy(line, aux);//si punem inapoi in line
	k = strlen(line) - 1;
	while (line[k] != ' ')
		k--;//k ramane retine pozitia la ultimul spatiu
	bytes = atoi(line + k);//astfel,line + k returneaza numarul de bytes
	strncpy(aux, line, k - 1);//punem in aux doar propozitia cu ghilimele
	aux[k - 1] = '\0';//punem terminatorul de linie
	strcpy(line, aux + 1);//copiem in linie prop care are ghilimeaua de final
	line[strlen(line)] = '\0';//stergem ultima ghilimea
	if (bytes > strlen(line))
		bytes = strlen(line);//conditia din enunt
	node_t *node = list->head;
	for (i = 0; i < list->dim; i++) {
		if (adr == node->adr)
			break;
		node = node->next;
	}
	if (i == list->dim)//daca nu gasim adresa
		return -1;
	if (bytes <= node->size_node) {
		if (!node->data) {//la primul write pe block,alocam pe data si scriem
			node->data = malloc(bytes + 1);
			memcpy(node->data, line, bytes);
			((char *)node->data)[bytes] = '\0';
		} else {//daca a mai fost alocat mai demult,doar rescriem
			strncpy((char *)node->data, line, bytes);
		}
		ok = 1;//ca sa stim ca am scris
	} else {//daca nu incape pe un singur block,incercam in continuare
		if (!node->data) {//alocam daca e primul si scriem
			node->data = malloc(node->size_node + 1);
			memcpy(node->data, line, node->size_node);
			((char *)node->data)[node->size_node] = '\0';
			strcpy(aux, line + node->size_node);
			strcpy(line, aux);//taiem prop ai. sa ramana ce nu a fost scris
		} else {//daca a mai fost scris,doar rescriem
			strncpy((char *)node->data, line, node->size_node);
			strcpy(aux, line + node->size_node);//taiem ca mai sus
			strcpy(line, aux);
		}
		while (i < list->dim - 1) {//cat timp putem merge in cont prin lista
			if (node->adr + node->size_node != node->next->adr)
				break;//verif daca se respecta conditia cu adrese consecutive
			bytes -= node->size_node, node = node->next;//scadem ca sa ramana
			if (bytes <= node->size_node) {//doar ce mai avem
				if (!node->data) {//daca suntem pe blocul final si putem,scriem
					node->data = malloc(bytes + 1);
					memcpy(node->data, line, bytes);
					((char *)node->data)[bytes] = '\0';
					ok = 1;
					if (ok == 1)
						break;
				} else {
					strncpy((char *)node->data, line, bytes);
					ok = 1;
				}
			} else {
				if (!node->data) {//daca inca nu suntem la final,scriem
					node->data = malloc(node->size_node + 1);//si continuam
					memcpy(node->data, line, node->size_node);
					((char *)node->data)[node->size_node] = '\0';
					strcpy(aux, line + node->size_node);
					strcpy(line, aux);
				} else {
					strncpy((char *)node->data, line, node->size_node);
					strcpy(aux, line + node->size_node);
					strcpy(line, aux);
				}
			}
		}
	}
	if (ok == 0)//daca nu s a putut scrie atunci procedam ca in enunt
		return -1;
	return 0;//inseamna ca a mers
}

int read(dll_t list)
{
	unsigned int adr, bytes, i, j, ok = 0, s = 0;
	scanf("%x%d", &adr, &bytes);//adresa + biti
	node_t *node = list.head;
	for (i = 0; i < list.dim; i++) {
		if (adr == node->adr)
			break;//cautam adresa
		node = node->next;
	}
	if (i == list.dim)
		return -1;//nu s a gasit
	if (bytes <= node->size_node) {
		ok = 1;//daca se poate din prima,o scriem
		printf("%s\n", (char *)node->data);
	} else {//altfel,cautam in prealabil daca a fost alocat tot
		node_t *nodx = node;
		j = i;
		while (j < list.dim - 1) {
			s += nodx->size_node;
			if (nodx->adr + nodx->size_node != nodx->next->adr)
				break;
			nodx = nodx->next;
			j++;
		}
		s += nodx->size_node;
		if (s < bytes)//daca nu s a alocat tot,atunci procedam ca in enunt
			return -1;
		printf("%s", (char *)node->data);//daca am ajuns aici,inseamna
		while (i < list.dim - 1) {//ca putem afisa tot
			if (node->adr + node->size_node != node->next->adr)
				break;
			bytes -= node->size_node;
			node = node->next;
			if (bytes <= node->size_node) {
				ok = 1;
				printf("%s\n", (char *)node->data);
				if (ok == 1)
					break;
			} else {
				printf("%s", (char *)node->data);
			}
		}
	}
	if (ok == 0)
		return -1;
	return 0;
}

void dump(heap_t heap, dll_t list)
{//fct de dump memory
	printf("+++++DUMP+++++\n");
	printf("Total memory: %d bytes\n", heap.total);
	unsigned int freeb = 0, total_free = 0;
	for (unsigned int i = 0; i < heap.nrl; i++) {
		freeb += heap.list[i]->dim;
		total_free += heap.list[i]->dim * heap.list[i]->size_node;
	}
	printf("Total allocated memory: %d bytes\n", heap.total - total_free);
	printf("Total free memory: %d bytes\n", total_free);
	printf("Free blocks: %d\n", freeb);
	printf("Number of allocated blocks: %d\n", list.dim);
	printf("Number of malloc calls: %d\n", heap.nrmal);
	printf("Number of fragmentations: %d\n", heap.nrfrg);
	printf("Number of free calls: %d\n", heap.nrfree);
	for (unsigned int i = 0; i < heap.nrl; i++) {
		if (heap.list[i]->dim == 0)
			continue;
		unsigned int x = heap.list[i]->size_node, y = heap.list[i]->dim;
		printf("Blocks with %d bytes - %d free block(s) :", x, y);
			node_t *node = heap.list[i]->head;
		for (unsigned int j = 0; j < heap.list[i]->dim; j++) {
			printf(" 0x%lx", node->adr);
			node = node->next;
		}
		printf("\n");
	}
	printf("Allocated blocks :");
	node_t *node = list.head;
	for (unsigned int i = 0; i < list.dim; i++) {
		printf(" (0x%lx - %d)", node->adr, node->size_node);
		node = node->next;
	}
	printf("\n-----DUMP-----\n");
}

void destroy(heap_t *heap, dll_t *list)
{//fct de destroy heap care elibereaza toate resursele,heap+lista
	for (unsigned int i = 0; i < heap->nrl; i++)
		if (heap->list[i]->head)
			free_list(heap->list[i]);
		else
			free(heap->list[i]);
	free(heap->list);
	if (list->dim > 0)
		free_list(list);
	else if (list)
		free(list);
}

int main(void)
{
	heap_t heap;
	dll_t *list;
	list = create(0);
	char command[20];
	int ok;
	while (1) {//cat timp comanda e diferita de destroy,executam
		scanf("%s", command);
		if (strcmp(command, "INIT_HEAP") == 0) {
			init_heap(&heap);
		} else if (strcmp(command, "MALLOC") == 0) {
			alloc(&heap, list);
		} else if (strcmp(command, "FREE") == 0) {
			free_b(&heap, list);
		} else if (strcmp(command, "WRITE") == 0) {
			ok = write(list);
			if (ok == -1) {//nu putem face operatia dorita
				printf("Segmentation fault (core dumped)\n");
				dump(heap, *list);
				destroy(&heap, list);
				return 0;
			}
		} else if (strcmp(command, "READ") == 0) {
			ok = read(*list);
			if (ok == -1) {//nu putem face operatia dorita
				printf("Segmentation fault (core dumped)\n");
				dump(heap, *list);
				destroy(&heap, list);
				return 0;
			}
		} else if (strcmp(command, "DUMP_MEMORY") == 0) {
			dump(heap, *list);
		} else if (strcmp(command, "DESTROY_HEAP") == 0) {
			destroy(&heap, list);
			break;
		}
	}
	return 0;
}
