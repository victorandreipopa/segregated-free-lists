**Nume:Popa Victor-Andrei**
**Grupă:313CAb**

## Segregated Free Lists

### Descriere:

* Scurtă descriere a funcționalității:

Proiectul prezinta un program în limbajul C care simuleaza operatiile de alocare
si dealocare a memoriei intr-un heap folosind o implementare de vector de
liste dublu inlantuite. Programul permite utilizatorilor sa initializeze
un heap, sa aloce blocuri de memorie, sa elibereze blocuri de memorie,
sa scrie date în blocurile de memorie alocate, sa citească date din
blocurile de memorie alocate, sa afiseze informatii despre memorie
si sa distruga heap-ul.

* Explicații suplimentare ale functiilor:

create: Aceasta functie creeaza o noua lista dublu inlantuita cu noduri
de o dimensiune specificata in antetul ei.

add_nth_node: Adauga un nod la pozitia specificata intr-o lista data.

remove_nth_node: Elimina nodul de la o pozitie data dintr-o lista si
returneaza nodul eliminat.

get_nth_node: Returneaza nodul de la pozitia n dintr-o lista data.

init_heap: Initializeaza heap-ul cu adresa de start, numarul de blocuri,
dimensiunea blocului si tipul blocului specificat.

free_list: Elibereaza memoria ocupata de toate nodurile dintr-o lista
dublu inlantuita si apoi elibereaza si memoria ocupata de lista.

alloc: Realizeaza operatia de alocare a unei noi portiuni de memorie
in heap pentru dimensiunea specificata, ceea ce consta in scoaterea unui nod
dintr-o lista si adaugarea acestuia in lista de "memorie alocata" in ordine
crescatare.In cazul fragmentarii, memoria ramasa nealocata se introduce
intr-o lista din heap care corespunde cu size-ul nodului si in caz contrar,
se creeaza o noua lista cu acel nod astfel incat lista sa fie pozitionata
si fie pozitionata crescator in vectorul de liste.

free_b: Realizeaza operatia de eliberare a unei portiuni de memorie alocate
anterior si o pune inapoi in heap in functie de size-ul blockului astfel incat
sa fie crescator in lista dupa adresa.

write: Scrie date in blocurile de memorie alocate, până la dimensiunea
specificată.Daca datele incap in primul block,atunci o scriem.In caz contrar,
verificam daca a mai fost alocat un block cu o adresa consecutiva dupa
size si incercam sa scriem in ea.Facem acest lucru pana cand nu se mai
gasesc blockuri alocate.Daca nu se reuseste scrierea totala sau daca nu se
gaseste adresa ceruta, atunci afisam segmentation fault,dump la memorie
si eliberez toate resursele folosite.

read: Citeste date din blocurile de memorie alocate pana la dimensiunea
ceruta.In prealabil,verificam daca au fost scrise date in intervalul
adresa,adresa + bytes si in caz contrar afisam segmentation fault,dump
si eliberez resursele.

dump: Afiseaza informatii detaliate despre starea curenta a heap-ului
si a listei de "blocuri alocate", si anume memoria totala, memoria alocata,
memoria libera, blocurile alocate impreuna cu dimensiunile lor, etc.

destroy: Elibereaza toate resursele alocate pentru heap si lista.

main: Creez lista pentru blockurile "alocate", citesc comenzile pentru
a apela functiile si la final eliberez tot.
