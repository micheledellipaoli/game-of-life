# Un algoritmo parallelo per la risoluzione di Game Of Life 


#### Descrizione del problema
**Game of Life** è un gioco matematico "*senza giocatori*", in cui l'evoluzione è determinata dal suo stato iniziale, senza necessità di alcun input da parte di giocatori umani. <br>
Il gioco si svolge su una griglia quadrata (o **matrice**) di celle (dette anche **elementi**). <br>
Ogni cella ha 8 vicini, che corrispondono alle celle ad essa **adiacenti**, includendo quelle in senso diagonale. <br>
Ogni cella può trovarsi in due stati: **viva** o **morta**. <br>
Lo stato della griglia evolve in intervalli, detti "*generazioni*", scanditi in maniera netta. <br>
Gli stati di tutti gli elementi in una data generazione sono usati per calcolare lo stato degli elementi alla generazione successiva.<br>
Tutte le celle della matrice vengono quindi aggiornate simultaneamente nel passaggio da una generazione a quella successiva, in base alle seguenti condizioni:<br>

1. qualsiasi cella viva con meno di due celle vive adiacenti muore, come per effetto d'isolamento;
2. qualsiasi cella viva con due o tre celle vive adiacenti sopravvive alla generazione successiva;
3. qualsiasi cella viva con più di tre celle vive adiacenti muore, come per effetto di sovrappopolazione;
4. qualsiasi cella morta con esattamente tre celle vive adiacenti diventa una cella viva, come per effetto di riproduzione.

<br>

**N.B.** Considereremo che la matrice sia **toroidale**, ovvero che la prima e l'ultima riga siano **vicine**, così come la prima e l'ultima colonna.
<br><br>

#### Soluzione alla problematica
Di seguito, è descritta una soluzione basata sulla programmazione parallela il cui obiettivo è la risoluzione di Game of Life.
<br>

<br><br>
#### Principio alla base della soluzione
Sia **N** una delle dimensioni della matrice quadrata, e **P** il numero di processi che eseguono la computazione in parallelo.<br>
Si supponga che il processo principale responsabile della coordinazione sia chiamato Master, mentre gli altri processi che svolgono la computazione in maniera parallela siano chiamati Slave.<br>

Il principio alla base della soluzione basata sulla computazione parallela è il seguente:<br><br>
il Master suddivide la matrice per blocchi di righe, ciascuno dei quali formato da:<br>
-**N/P** righe se N%P = 0;<br>
-altrimenti calcolerà un **array di indicizzazione** che memorizza le **dimensioni parziali** dei blocchi di righe destinati a ciascun processo.<br>

Ciascun blocco di righe viene inviato ad un processo con una Scatterv.

Ciascun processo riceve il blocco di righe dal Master.

Il processo, inizialmente, può computare solo gli elementi delle **righe interne** del blocco.

Gli elementi che appartengono alla prima e all’ultima riga del blocco ricevuto richiedono la **comunicazione** con il processo di rank-1 e con il processo di rank+1 per essere computati.<br>


![](Immagine1.png)
<br>
*Immagine 1: rappresentazione della suddivione della matrice in blocchi di righe destinati ai diversi processi per la computazione in parallelo.*
<br><br><br>

#### Algoritmo risolutivo
L'algoritmo descritto di seguito è implementato in linguaggio C, ed utilizza MPI per la comunicazione tra i processori.<br>

Il Master alloca memoria per la **Matrice di Lettura**, di tipo **char**, e la inizializza, grazie alla funzione initializeMatrix(), in maniera random o con uno dei pattern noti, con i seguenti valori:<br>
‘**#**’ per indicare che l’elemento è **vivo**;<br>
‘**.**’ (**punto**) per indicare che l’elemento è **morto**.<br><br>
Inoltre, Il Master alloca memoria per una **Matrice di Scrittura** in cui memorizza, generazione dopo generazione, gli elementi computati prendendo in input Matrice di Lettura.

Se p==1 effettua la computazione **sequenziale**{<br>
<div style="margin-left: 15px">

viene eseguito un ciclo for che itera le generazioni{<br>

<div style="margin-left: 20px">

-computa ogni singolo elemento della Matrice di Scrittura grazie alla funzione **computeElement()**, a cui viene passata come parametro la Matrice di Lettura;<br>
-aggiorna la Matrice di Lettura con la Matrice di Scrittura appena calcolata (*Double Buffering*).<br>
</div>
}</div>
}
<br><br>

Se p>1{<br>

<div style="margin-left: 15px">

Il Master computa un array di indicizzazione, chiamato “**arrayIndexPartial**”, che memorizza le dimensioni parziali dei blocchi di righe destinati a ciascun processo.<br>
Tale array sarà utile per suddividere la Matrice di Lettura in blocchi di righe da inviare a ciascun processo Slave.<br><br>
Ciascun processo alloca una **Matrice di Lettura Parziale** ed una **Matrice di Scrittura Parziale**, entrambe di dimensione pari al blocco di righe ricevuto dal Master. 

**N.B.** Se il processo riceve dal Master un numero di righe pari a 1, la dimensione della Matrice di Lettura Parziale e quella della Matrice di Scrittura Parziale sarà 1*numColonne. 

Successivamente, viene effettuato un ciclo for che itera le generazioni{<br><br>

<div style="margin-left: 20px">


Ciascun blocco di righe viene inviato al relativo processo grazie ad una **Scatterv**.
<br><br>

Se il numero di righe che il processo riceve dal Master è maggiore di 2{<br><br>

<div style="margin-left: 20px">

-il processo riceve il proprio blocco di righe dal Master e lo memorizza nella Matrice di Lettura Parziale.<br>
**N.B.** anche il Master riceve un blocco di righe da sé stesso, partecipando quindi alla computazione parallela. 

-il processo effettua una **Send non bloccante** a rank-1 inviando la **prima riga** del blocco ricevuto dal Master.<br>
(Se il rank del processo attuale è pari a 0, si setta rank-1 = sizeWorld-1, ovvero pari all’ultimo processo del Cluster di nodi).

-il processo effettua una **Send non bloccante** a rank+1 inviando l’**ultima riga** del blocco ricevuto.<br>
(Se il rank del processo attuale è pari a sizeWorld-1, si setta rank+1 = 0, ovvero pari al primo processo del Cluster di nodi).<br>

**N.B.** per scegliere la destinazione della Send viene effettuato anche un **controllo** sul numero di righe ricevuto dai processi vicini, per assicurarsi che essi abbiano ricevuto **almeno una riga**.<br>
Se infatti i processi vicini ricevono **zero righe**, essi non dovranno partecipare alla computazione, né tantomeno alla comunicazione non bloccante.<br>

-il processo effettua una **Receive non bloccante** da rank-1, utile a calcolare gli elementi della prima riga del blocco ricevuto dal Master.<br>
Questi elementi provenienti dal “vicino di sinistra” saranno memorizzati nella **prima riga** di una **Matrice Temporanea di Sinistra**, nella quale verranno ricopiate anche le prime due righe della Matrice di Lettura Parziale.<br>

-il processo effettua una **Receive non bloccante** da rank+1, utile a calcolare gli elementi dell’ultima riga del blocco ricevuto dal Master.<br>
Questi elementi provenienti dal “vicino di destra” saranno memorizzati nell’**ultima riga** di una **Matrice Temporanea di Destra**, nella quale verranno ricopiate, nella prima e nella seconda riga, rispettivamente la penultima e l’ultima riga della Matrice di Lettura Parziale.<br>

-il processo computa gli elementi delle **righe interne** della Matrice di Lettura Parziale (che non richiedono la comunicazione con i processi vicini), e li inserisce nella righe interne della Matrice di Scrittura Parziale.<br>

-il processo effettua due **MPI_Wait** per attendere che la comunicazione non bloccante venga completata.<br>

-il processo computa gli elementi delle **righe di bordo** della Matrice di Lettura Parziale, ovvero quelli della prima e dell’ultima riga del blocco ricevuto dal Master, grazie alla Matrice Temporanea di Sinistra e alla Matrice Temporanea di Destra, e li inserisce nelle righe di bordo della Matrice di Scrittura Parziale.<br><br>
</div>
}<br><br>

Se il numero di righe che il processo riceve dal Master è uguale a 2{<br><br>

<div style="margin-left: 20px">

-il processo effettua tutti i passaggi descritti nel blocco precedente, tuttavia **non computerà** gli elementi delle **righe interne** della Matrice di Lettura Parziale poiché la dimensione è di 2*numColonne.<br>

In questo caso, dunque, viene allocata un’**unica Matrice Temporanea** di **4 righe**:<br>
-nella prima e nell’ultima riga memorizza le righe provenienti rispettivamente dal vicino di sinistra e dal vicino di destra;<br>
-nelle due righe centrali memorizza le righe della Matrice di Lettura Parziale.<br>

-il processo effettua due **MPI_Wait** per attendere la terminazione della comunicazione non bloccante.<br>

-il processo computa gli elementi delle uniche due righe della Matrice di Lettura Parziale, grazie alla Matrice Temporanea, e li inserisce nella Matrice di Scrittura Parziale. <br><br>
</div>
}<br><br>







Se il numero di righe che il processo riceve dal Master è uguale a 1{<br><br>

<div style="margin-left: 20px">

-il processo riceve la riga dal Master e la memorizza nell’unica riga della Matrice di Lettura Parziale. <br>

-il processo effettua una **Send non bloccante** a rank-1 inviando la riga del blocco ricevuto dal Master.<br> 
(Se il rank del processo attuale è pari a 0, si setta rank-1 = sizeWorld-1, ovvero pari all’ultimo processo del Cluster di nodi).<br>

-il processo effettua una Send non bloccante a rank+1 inviando la riga del blocco ricevuto dal Master.<br>
(Se il rank del processo attuale è pari a sizeWorld-1, si setta rank+1 = 0, ovvero pari al primo processo del Cluster).<br>

**N.B.** per scegliere la destinazione della Send viene effettuato, anche stavolta, un controllo sul numero di righe ricevuto dai processi vicini, per assicurarsi che essi abbiano ricevuto **almeno** una riga.<br>
Se infatti i processi vicini hanno ricevuto zero righe, essi non dovranno partecipare alla computazione, né tantomeno alla comunicazione non bloccante.<br>

-il processo effettua una **Receive non bloccante** da rank-1, utile a calcolare gli elementi della riga del blocco ricevuto dal Master.<br>
Questi elementi provenienti dal “vicino di sinistra” saranno memorizzati nella **prima riga** dell’unica **Matrice Temporanea** allocata, di dimensione 3*numColonne, nella quale verrà ricopiata, nella seconda riga, anche la riga della Matrice di Lettura Parziale.

-il processo effettua una **Receive non bloccante** da rank+1, utile a calcolare gli elementi dell’ultima riga del blocco ricevuto dal Master.
Questi elementi provenienti dal “vicino di destra” saranno memorizzati nell’**ultima riga** della Matrice Temporanea.<br>

-il processo effettua due **MPI_Wait** per attendere la terminazione della comunicazione non bloccante.<br>

-il processo computa gli elementi della riga centrale della Matrice Temporanea, e li inserisce nella Matrice di Scrittura Parziale.<br><br>
</div>
}<br><br>

-completata la computazione di tutti gli elementi, ciascun processo invia la propria Matrice di Scrittura Parziale al Master che le raccoglie con una **Gatherv**.<br>

-il Master **aggiorna** la Matrice di Lettura con quella appena costruita sulla base delle Matrici di Scrittura Parziali ricevute da ciascun processo (*Double Buffering*).<br>
</div>}
</div>}
<br><br>

<br><br>

#### Codice
```C
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define live '#'
#define dead '.'

void initializeMatrix(int n, int config, char **matrix){

	//Configurazione di tipo "Still lifes" denominata Block
	if(config == 1){

		if(n == 4){
			int a[4][4] = { {dead,dead,dead,dead},
					{dead,live,live,dead},
					{dead,live,live,dead},
					{dead,dead,dead,dead} };

			printf("\nMatrice di lettura: \n");
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matrix[i][j] = a[i][j];
					printf("%c ", matrix[i][j]);
				}
				printf("\n");
			}
		}else{

			//Se la dimensione della matrice e` diversa da 4, tutti gli elementi sono inizializzati a dead.
			printf("\nMatrice di lettura: \n");
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matrix[i][j] = dead;
					printf("%c ", matrix[i][j]);
				}
				printf("\n");
			}
		}
	}else if (config == 2){

		//Configurazione di tipo "Still lifes" denominata Tub
		if(n == 5){
			int a[5][5] = { {dead,dead,dead,dead,dead},
					{dead,dead,live,dead,dead},
					{dead,live,dead,live,dead},
					{dead,dead,live,dead,dead},
					{dead,dead,dead,dead,dead} };

			printf("\nMatrice di lettura: \n");
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matrix[i][j] = a[i][j];
					printf("%c ", matrix[i][j]);
				}
				printf("\n");
			}
		}else{

			//Se la dimensione della matrice e` diversa da 5, tutti gli elementi sono inizializzati a dead.
			printf("\nMatrice di lettura: \n");
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matrix[i][j] = dead;
					printf("%c ", matrix[i][j]);
				}
				printf("\n");
			}
		}
	}
	else if(config == 3){

		//Configurazione di tipo "Oscillators" denominata Blinker
		if(n == 5){
			int a[5][5] = { {dead,dead,dead,dead,dead},
					{dead,dead,live,dead,dead},
					{dead,dead,live,dead,dead},
					{dead,dead,live,dead,dead},
					{dead,dead,dead,dead,dead} };

			printf("\nMatrice di lettura: \n");
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matrix[i][j] = a[i][j];
					printf("%c ", matrix[i][j]);
				}
				printf("\n");
			}
		}else{

			//Se la dimensione della matrice e` diversa da 5, tutti gli elementi sono inizializzati a dead.
			printf("\nMatrice di lettura: \n");
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matrix[i][j] = dead;
					printf("%c ", matrix[i][j]);
				}
				printf("\n");
			}
		}
	}
	else if(config == 4){

		//Configurazione di tipo "Oscillators" denominata Toad
		if(n == 6){
			int a[6][6] = { {dead,dead,dead,dead,dead,dead},
					{dead,dead,dead,dead,dead,dead},
					{dead,dead,live,live,live,dead},
					{dead,live,live,live,dead,dead},
					{dead,dead,dead,dead,dead,dead},
					{dead,dead,dead,dead,dead,dead}};

			printf("\nMatrice di lettura: \n");
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matrix[i][j] = a[i][j];
					printf("%c ", matrix[i][j]);
				}
				printf("\n");
			}
		}else{

			//Se la dimensione della matrice e` diversa da 6, tutti gli elementi sono inizializzati a dead.
			printf("\nMatrice di lettura: \n");
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matrix[i][j] = dead;
					printf("%c ", matrix[i][j]);
				}
				printf("\n");
			}
		}
	}
	else if(config == 5){

		//Configurazione di tipo "Oscillators" denominata Beacon
		if(n == 6){
			int a[6][6] = { {dead,dead,dead,dead,dead,dead},
					{dead,live,live,dead,dead,dead},
					{dead,live,dead,dead,dead,dead},
					{dead,dead,dead,dead,live,dead},
					{dead,dead,dead,live,live,dead},
					{dead,dead,dead,dead,dead,dead}};

			printf("\nMatrice di lettura: \n");
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matrix[i][j] = a[i][j];
					printf("%c ", matrix[i][j]);
				}
				printf("\n");
			}
		}else{

			//Se la dimensione della matrice e` diversa da 6, tutti gli elementi sono inizializzati a dead.
			printf("\nMatrice di lettura: \n");
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matrix[i][j] = dead;
					printf("%c ", matrix[i][j]);
				}
				printf("\n");
			}
		}
	}else{
		//Configurazione nel caso in cui nel comando mpirun viene passato un parametro diverso da quelli sopra elencati
		//Configurazione Random della matrice
		printf("\nMatrice di lettura: \n");
		for(int i=0; i<n; i++){
			for(int j=0; j<n; j++){
				if(rand() % 2){
					matrix[i][j] = live;
				}else{
					matrix[i][j] = dead;
				}
				printf("%c ", matrix[i][j]);
			}
			printf("\n");
		}
	}

}




//computa l'array di indicizzazione che memorizza il numero di righe che il Master deve inviare a ciascun processo
void computeArrayIndexPartialDataType(int quoziente, int resto, int numColonne, int sizeWorld, int *array){

	for(int i=0; i<sizeWorld;i++){
		if(resto>0){
			array[i] = (quoziente + 1);
			resto--;
		}else{
			array[i] = quoziente;
		}
	}

}


//computa l'elemento tenendo presente che la matrice è toroidale
char computeElement(int indiceRiga, int indiceColonna, int numRighe, int numColonne, char **matrice){

	int sum = 0;
	char elem = matrice[indiceRiga][indiceColonna];
	char computedElement;

	char topLeft, topCenter, topRight, centerLeft, centerRight, bottomLeft, bottomCenter, bottomRight;

	//Inizializzazione degli indici delle righe e delle colonne degli 8 elementi adiacenti all' elemento 
	//che stiamo calcolando
	int nextRow = indiceRiga+1;
	int previousRow = indiceRiga-1;
	int nextColumn = indiceColonna+1;
	int previousColumn = indiceColonna-1;

	//condizione in cui l'elemento appartiene all'ultima riga, quindi indiceRiga+1 corrisponde a riga0, 
	//ovvero la riga che succede l'ultima riga corrisponde alla prima.
	if(indiceRiga == numRighe-1){
		nextRow = 0;
	}

	//condizione in cui l'elemento appartiene alla prima riga, quindi indiceRiga-1 corrisponde a numRighe-1,
	//ovvero la riga che precede riga0 corrisponde all'ultima riga.
	if(indiceRiga == 0){
		previousRow = numRighe-1;
	}

	//condizione in cui l'elemento appartiene alla prima colonna, quindi indiceColonna-1 corrisponde a
	//numColonne-1, ovvero la colonna che precede la prima colonna corrisponde all'ultima.
	if(indiceColonna == 0){
		previousColumn = numColonne-1;
	}

	//condizione in cui l'elemento appartiene all'ultima colonna, quindi indiceColonna+1 corrisponde a
	//colonna0, ovvero la colonna che succede l'ultima colonna corrisponde alla prima.
	if(indiceColonna == numColonne-1){
		nextColumn = 0;
	}

	//condizione in cui l'elemento non appartiene né alla prima né all'ultima riga, né alla prima 
	//né all'ultima colonna

	topLeft = matrice[previousRow][previousColumn];
	topCenter = matrice[previousRow][indiceColonna];
	topRight = matrice[previousRow][nextColumn];
	centerLeft = matrice[indiceRiga][previousColumn];
	centerRight = matrice[indiceRiga][nextColumn];
	bottomLeft = matrice[nextRow][previousColumn];
	bottomCenter = matrice[nextRow][indiceColonna];
	bottomRight = matrice[nextRow][nextColumn];

	if(topLeft == live){
		sum++;
	}
	if(topCenter == live){
		sum++;
	}
	if(topRight == live){
		sum++;
	}
	if(centerLeft == live){
		sum++;
	}
	if(centerRight == live){
		sum++;
	}
	if(bottomLeft == live){
		sum++;
	}
	if(bottomCenter == live){
		sum++;
	}
	if(bottomRight == live){
		sum++;
	}


	if( (elem == live) && (sum < 2) ){
		computedElement = dead;
	}
	else if( (elem == live) && ( (sum >= 2) && (sum<=3) ) ){
		computedElement = live;
	}
	else if( (elem == live) && (sum > 3) ){
		computedElement = dead;
	}
	else if( (elem == dead) && (sum == 3) ){
		computedElement = live;
	}
	else if( (elem == dead) && ( (sum < 3) || (sum > 3) ) ){
		computedElement = dead;
	}

	return computedElement;
}


int main(int argc, char *argv[]){

	int	myRank;
	int	sizeWorld;
	int rankMaster = 0;

	/*codice prestazioni*/
	double start;

	MPI_Status status1, status2;
	MPI_Request request1, request2;

	MPI_Init(NULL, NULL);

	/*codice prestazioni*/
	MPI_Barrier(MPI_COMM_WORLD);
	start = MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &sizeWorld);

	//dimensioni della matrice
	int n = atoi(argv[1]);
	//iterazioni da effettuare
	int iter = atoi(argv[2]);

	//configurazione iniziale della matrice
	int configuration;

	//Se nel comando mpirun non viene inserito il terzo parametro relativo alla configurazione,
	//viene settato configuration=0 che inizializza la matrice in maniera random.
	if (argv[3] == 0){
		configuration = 0;
	}else{
		//altrimenti, legge il parametro e lo memorizza nella variabile configuration
		configuration = atoi(argv[3]);
	}

	//dichiarazione Matrice di Lettura Globale
	char **matL;
	//dichiarazione Matrice di Scrittura Globale
	char **matS;

	if(myRank==rankMaster){

		//allocazione Matrice di Lettura
		matL = (char **) malloc(n*sizeof(char*));
		matL[0] = (char*) malloc(n*n*sizeof(char));
		for(int i=1; i<n; i++){
			matL[i] = matL[0] + i*n;
		}

		//allocazione Matrice di Scrittura
		matS = (char **) malloc(n*sizeof(char*));
		matS[0] = (char*) malloc(n*n*sizeof(char));
		for(int i=1; i<n; i++){
			matS[i] = matS[0] + i*n;
		}

		srand(myRank);

		//inizializzazione Matrice di Lettura
		initializeMatrix(n, configuration, matL);

	}

	//Computazione da effettuare con un singolo processo
	if(sizeWorld==1){

		//Esegue k iterazioni computando le generazioni della Matrice
		for(int k=0; k<iter; k++){

			//computazione Matrice di Scrittura prendendo come input la Matrice di Lettura
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matS[i][j] = computeElement(i, j, n, n, matL);
				}
			}

			//Double Buffering: Inseriamo nella Matrice di Lettura gli elementi della 
			//Matrice di Scrittura appena computata
			printf("\nGenerazione[%d] - Matrice Computata: \n", k+1);
			for(int i=0; i<n; i++){
				for(int j=0; j<n; j++){
					matL[i][j] = matS[i][j];
					printf("%c ", matL[i][j]);
				}
				printf("\n");
			}
		}

		free(matL);
		free(matS);

		MPI_Barrier(MPI_COMM_WORLD);
		double endSequential = MPI_Wtime();

		printf("\nTime in ms = %f\n", endSequential - start);

	}else if(sizeWorld>1){	//Computazione da effettuare con processi multipli

		int rankMaster = 0;

		int quoziente = n/(sizeWorld);
		int resto = n%(sizeWorld);

		//Alloca e computa l'array di indicizzazione che memorizza le dimensioni dei 
		//blocchi di righe, ovvero il numero di elementi, da inviare a ciascun processo.
		int *arrayIndexPartial = (int*) malloc(sizeWorld*sizeof(int));
		computeArrayIndexPartialDataType(quoziente, resto, n, sizeWorld, arrayIndexPartial);

		//Alloca e computa l'array displs utile per Scatterv e Gatherv.
		//Tale array memorizza, per ciascun processo i, un indice di partenza dal quale 
		//iniziare a ricevere il contenuto inviato dal processo root con la Scatterv.
		//Allo stesso modo, definisce, per il processo root, in quale posizione della 
		//matrice andare a memorizzare i buffer ricevuti dagli altri processi con la Gatherv.
		int *displs = (int*) malloc(sizeWorld*sizeof(int));
		int offsetDispls = 0;
		displs[0] = 0;
		for(int i=1; i<sizeWorld; i++){
			displs[i] = offsetDispls + arrayIndexPartial[i-1];
			offsetDispls = displs[i];
		}

		//dichiarazione Matrice di Lettura Parziale
		char **matLP;
		//dichiarazione Matrice di Scrittura Parziale
		char **matSP;

		//Dichiarazione di Array utili per memorizzare gli elementi di bordo da inviare e 
		//ricevere a/da processi vicini
		char *sendElementsLeft;
		char *sendElementsRight;
		char *receiveElementsLeft;
		char *receiveElementsRight;

		//dichiarazione di Matrici Temporanee utili per memorizzare gli elementi di bordo 
		//ricevuti dai vicini
		char **matTempLeft;
		char **matTempRight;
		char **matTemp;

		//numero di elementi che il processo corrente deve ricevere dal Master
		int recvCount = (arrayIndexPartial[myRank])*n;

		//numero di righe sulle quali il proocesso corrente distribuisce gli elementi ricevuti 
		//dal Master
		int nRigheParziali = arrayIndexPartial[myRank];

		//Definizione e Commit del nuovo datatype che rappresenta una riga della matrice.
		MPI_Datatype row;
		MPI_Type_contiguous(n, MPI_CHAR, &row);
		MPI_Type_commit(&row);


		//Se il processo corrente ricevera` (tramite Scatterv) dal Master ALMENO due righe, 
		//alloca una Matrice di Lettura Parziale ed una Matrice di Scrittura Parziale di 
		//dimensioni nRigheParziali x n
		if(nRigheParziali>=2){

			//allocazione Matrice di Lettura Parziale composta da:
			//due righe, nel caso in cui il blocco che il Master invia al processo corrente e` formato da due righe;
			//tre righe, nel caso in cui il blocco che il Master invia al processo corrente e` formato da tre o piu` righe.
			matLP = (char **) malloc(nRigheParziali*sizeof(char*));
			matLP[0] = (char*) malloc(nRigheParziali*n*sizeof(char));
			for(int i=1; i<nRigheParziali; i++){
				matLP[i] = matLP[0] + i*n;
			}

			//allocazione Matrice di Scrittura Parziale composta da:
			//due righe nel caso in cui il blocco che il Master invia al processo corrente e` formato da due righe;
			//tre righe, nel caso in cui il blocco che il Master invia al processo corrente e` formato da tre o piu` righe.
			matSP = (char **) malloc(nRigheParziali*sizeof(char*));
			matSP[0] = (char*) malloc(nRigheParziali*n*sizeof(char));
			for(int i=1; i<nRigheParziali; i++){
				matSP[i] = matSP[0] + i*n;
			}
		}

		//N.B. Se il processo NON riceve alcuna riga dal Master, alloca comunque una 
		//Matrice di Lettura Parziale e una Matrice di Scrittura Parziale per evitare 
		//errori durante Scatterv e Gatherv
		if(nRigheParziali==0 || nRigheParziali==1){

			//allocazione Matrice di Lettura Parziale composta da un' unica riga.
			matLP = (char **) malloc(1*sizeof(char*));
			matLP[0] = (char*) malloc(1*n*sizeof(char));

			//allocazione Matrice di Scrittura Parziale composta da un' unica riga.
			matSP = (char **) malloc(1*sizeof(char*));
			matSP[0] = (char*) malloc(1*n*sizeof(char));

		}


		for(int k=0; k<iter; k++){

			MPI_Scatterv(matL[0], arrayIndexPartial, displs, row, matLP[0], nRigheParziali, row, rankMaster, MPI_COMM_WORLD);

			//Se il processo corrente riceve ALMENO una riga dal Master, allora 
			//partecipera` alla computazione.
			if(nRigheParziali>0){

				int destLeft;
				if(myRank == 0){
					destLeft = sizeWorld-1;
				}else{
					destLeft = myRank-1;
				}
				//Ciclo che aiuta a scegliere il processo di sinistra piu` prossimo 
				//che abbia ricevuto ALMENO una riga dal Master.
				//In questo modo non invieremo gli elementi di bordo a processi che 
				//non partecipano alla computazione.
				while(arrayIndexPartial[destLeft]==0){
					destLeft--;
					destLeft = destLeft%sizeWorld;
				}

				int destRight;
				if(myRank == sizeWorld-1){
					destRight = 0;
				}else{
					destRight = myRank+1;
				}
				//Ciclo che aiuta a scegliere il processo di destra piu` prossimo che
				//abbia ricevuto ALMENO una riga dal Master.
				//In questo modo non invieremo gli elementi di bordo a processi che
				//non partecipano alla computazione.
				while(arrayIndexPartial[destRight]==0){
					destRight++;
					destRight = destRight%sizeWorld;
				}

				//Computa un array da inviare al vicino di sinistra 
				//formato dalla prima riga del blocco ricevuto dal Master
				sendElementsLeft = (char*) malloc(n*sizeof(char));
				for(int j=0; j<n; j++){
					sendElementsLeft[j] = matLP[0][j];
				}

				//Computa un array da inviare al vicino di destra 
				//formato dall'ultima riga del blocco ricevuto dal Master
				sendElementsRight = (char*) malloc(n*sizeof(char));
				for(int j=0; j<n; j++){
					sendElementsRight[j] = matLP[nRigheParziali-1][j];
				}

				//Invia ai propri vicini in modo non bloccante gli elementi di bordo
				MPI_Isend(sendElementsLeft, 1, row, destLeft, 0, MPI_COMM_WORLD, &request1);
				MPI_Isend(sendElementsRight, 1, row, destRight, 1, MPI_COMM_WORLD, &request2);

				int sourceLeft;
				if(myRank == 0){
					sourceLeft = sizeWorld-1;
				}else{
					sourceLeft = myRank-1;
				}
				//Ciclo che aiuta a scegliere il processo di sinistra piu` prossimo 
				//che abbia ricevuto ALMENO una riga dal Master.
				//In questo modo non riceveremo gli elementi di bordo da processi che 
				//non partecipano alla computazione.
				while(arrayIndexPartial[sourceLeft]==0){
					sourceLeft--;
					sourceLeft = sourceLeft%sizeWorld;
				}

				int sourceRight;
				if(myRank == sizeWorld-1){
					sourceRight = 0;
				}else{
					sourceRight = myRank+1;
				}
				//Ciclo che aiuta a scegliere il processo di destra piu` prossimo 
				//che abbia ricevuto ALMENO una riga dal Master.
				//In questo modo non riceveremo gli elementi di bordo da processi 
				//che non partecipano alla computazione.
				while(arrayIndexPartial[sourceRight]==0){
					sourceRight++;
					sourceRight = sourceRight%sizeWorld;
				}

				//Alloca array da ricevere dal vicino di sinistra formato 
				//dalla sua ultima riga del blocco ricevuto dal Master
				receiveElementsLeft = (char*) malloc(n*sizeof(char));
				//Alloca array da ricevere dal vicino di destra formato 
				//dalla sua prima riga del blocco ricevuto dal Master
				receiveElementsRight = (char*) malloc(n*sizeof(char));

				//Riceve dai propri vicini in modo non bloccante gli elementi di bordo
				if(sizeWorld==2){
					//Soluzione alla problematica di comunicazione con 2 processi
					MPI_Irecv(receiveElementsLeft, 1, row, sourceRight, 1, MPI_COMM_WORLD, &request2);
					MPI_Irecv(receiveElementsRight, 1, row, sourceLeft, 0, MPI_COMM_WORLD, &request1);
				}else{
					MPI_Irecv(receiveElementsLeft, 1, row, sourceLeft, 1, MPI_COMM_WORLD, &request2);
					MPI_Irecv(receiveElementsRight, 1, row, sourceRight, 0, MPI_COMM_WORLD, &request1);
				}

			}

			//Se il blocco di righe ricevuto dal Master e` formato da ALMENO 3 righe,
			//allora computo prima gli elementi delle righe interne e 
			//successivamente gli elementi delle righe di bordo.
			if(nRigheParziali>2){

				//Computa solo gli elementi interni della Matrice Parziale ricevuta dal Master,
				//ovvero quelli che non hanno bisogno della comunicazione con processi vicini.
				//Non vengono computati gli elementi appartenenti alla prima 
				//e all'ultima riga della Matrice Parziale.
				for(int i=1; i<nRigheParziali-1; i++){
					for(int j=0; j<n; j++){
						matSP[i][j] = computeElement(i, j, nRigheParziali, n, matLP);
					}
				}


				//Allocazione Matrice Temporanea di Sinistra che memorizzera` 3 righe:
				//l' ultima riga del blocco del vicino sinistro, 
				//la prima e la seconda riga del processo corrente.
				matTempLeft = (char **) malloc(3*sizeof(char*));
				matTempLeft[0] = (char*) malloc(3*n*sizeof(char));
				for(int i=1; i<3; i++){
					matTempLeft[i] = matTempLeft[0] + i*n;
				}

				MPI_Wait(&request2, &status2);

				//Inizializzazione della Matrice Temporanea di Sinistra
				//Inserimento della riga ricevuta dal vicino di sinistra
				for(int j=0; j<n; j++){
					matTempLeft[0][j] = receiveElementsLeft[j];
				}
				//Inserimento della prima e della seconda riga della Matrice Parziale ricevuta dal Master
				for(int i=1; i<3; i++){
					for(int j=0; j<n; j++){
						matTempLeft[i][j] = matLP[i-1][j];
					}
				}

				//Computa gli elementi appartenenti alla prima riga della Matrice Parziale
				//ricevuta dal Master inserendoli nella Matrice di Scrittura Parziale
				for(int j=0; j<n; j++){
					matSP[0][j] = computeElement(1, j, 3, n, matTempLeft);
				}



				//Allocazione Matrice Temporanea di Destra che memorizzera` 3 righe:
				//la penultima e l'ultima riga del processo corrente 
				//e la prima riga del blocco del vicino destro.
				matTempRight = (char **) malloc(3*sizeof(char*));
				matTempRight[0] = (char*) malloc(3*n*sizeof(char));
				for(int i=1; i<3; i++){
					matTempRight[i] = matTempRight[0] + i*n;
				}

				MPI_Wait(&request1, &status1);

				//Inizializzazione della Matrice Temporanea di Destra
				//Inserimento della penultima e dell'ultima riga 
				//della Matrice Parziale ricevuta dal Master
				for(int j=0; j<n; j++){
					matTempRight[0][j] = matLP[nRigheParziali-2][j];
					matTempRight[1][j] = matLP[nRigheParziali-1][j];
				}
				//Inserimento della riga ricevuta dal vicino di destra
				for(int j=0; j<n; j++){
					matTempRight[2][j] = receiveElementsRight[j];
				}

				//Computa gli elementi appartenenti all' ultima riga 
				//della Matrice Parziale ricevuta dal Master 
				//inserendoli nella Matrice di Scrittura Parziale
				for(int j=0; j<n; j++){
					matSP[nRigheParziali-1][j] = computeElement(1, j, 3, n, matTempRight);
				}

			}

			//Se il blocco di righe ricevuto dal Master e` formato da
			//ESATTAMENTE 2 righe, non e` possibile computare
			//gli elementi appartenenti alle righe interne.
			if(nRigheParziali==2){

				//Allocazione dell' UNICA Matrice Temporanea di dimensione 4*n 
				//che memorizzera` 4 righe: l' ultima riga del blocco del vicino sinistro,
				//le due righe ricevute dal processo Master 
				//e la prima riga del blocco del vicino destro.
				matTemp = (char **) malloc(4*sizeof(char*));
				matTemp[0] = (char*) malloc(4*n*sizeof(char));
				for(int i=1; i<4; i++){
					matTemp[i] = matTemp[0] + i*n;
				}

				MPI_Wait(&request2, &status2);
				MPI_Wait(&request1, &status1);


				//Inserimento nella Matrice Temporanea della riga ricevuta dal vicino di sinistra,
				//delle due righe del blocco ricevuto dal Master 
				//e della riga ricevuta dal vicino di destra
				for(int j=0; j<n; j++){
					matTemp[0][j] = receiveElementsLeft[j];
					matTemp[1][j] = matLP[0][j];
					matTemp[2][j] = matLP[1][j];
					matTemp[3][j] = receiveElementsRight[j];
				}

				//Computa gli elementi di riga1 e di riga2 della Matrice Temporanea
				//inserendoli nella Matrice di Scrittura Parziale
				for(int j=0; j<n; j++){
					matSP[0][j] = computeElement(1, j, 4, n, matTemp);
					matSP[1][j] = computeElement(2, j, 4, n, matTemp);
				}

			}

			//Se il blocco di righe ricevuto dal Master e` formato da UNA SOLA RIGA,
			//computiamo l' unica riga solo dopo aver ricevuto le righe dai processi vicini.
			if(nRigheParziali==1){

				//Allocazione dell' UNICA Matrice Temporanea che memorizzera` 3 righe:
				//l' ultima riga del blocco del vicino sinistro,
				//la riga ricevuta dal processo Master 
				//e la prima riga del blocco del vicino destro.
				matTemp = (char **) malloc(3*sizeof(char*));
				matTemp[0] = (char*) malloc(3*n*sizeof(char));
				for(int i=1; i<3; i++){
					matTemp[i] = matTemp[0] + i*n;
				}

				MPI_Wait(&request2, &status2);
				MPI_Wait(&request1, &status1);

				//Inserimento nella Matrice Temporanea della riga ricevuta dal vicino di sinistra,
				//dell' unica riga del blocco ricevuto dal Master 
				//e della riga ricevuta dal vicino di destra
				for(int j=0; j<n; j++){
					matTemp[0][j] = receiveElementsLeft[j];
					matTemp[1][j] = matLP[0][j];
					matTemp[2][j] = receiveElementsRight[j];
				}

				//Computa gli elementi della riga centrale della Matrice Temporanea
				//inserendoli nella Matrice di Scrittura Parziale
				for(int j=0; j<n; j++){
					matSP[0][j] = computeElement(1, j, 3, n, matTemp);
				}

			}

			//Double Buffering: Inseriamo nella Matrice di Lettura
			//gli elementi delle varie Matrici di Scrittura Parziale ricevute dai vari processi
			MPI_Gatherv(matSP[0], nRigheParziali, row, matL[0], arrayIndexPartial, displs, row, rankMaster, MPI_COMM_WORLD);


			if(myRank==rankMaster){
				printf("\nGenerazione[%d] - Matrice Computata: \n", k+1);
				for(int i=0; i<n; i++){
					for(int j=0; j<n; j++){
						printf("%c ", matL[i][j]);
					}
					printf("\n");
				}
			}


			if(nRigheParziali!=0){
				free(sendElementsLeft);
				free(sendElementsRight);
				free(receiveElementsLeft);
				free(receiveElementsRight);

				if(nRigheParziali>2){
					free(matTempLeft);
					free(matTempRight);
				}
				if(nRigheParziali==1 || nRigheParziali==2){
					free(matTemp);
				}
			}

		}//fine for iterazioni

		free(matLP);
		free(matSP);

		if(myRank==rankMaster){
			free(matL);
			free(matS);
		}

		MPI_Barrier(MPI_COMM_WORLD);
		double endParallel = MPI_Wtime();

		if(myRank==rankMaster){
			printf("\nTime in ms = %f\n", endParallel - start);
		}

	}

	MPI_Finalize();

	return 0;
}

```
<br><br>
<br><br>

#### Note sulla compilazione
Per eseguire l'algoritmo, bisogna copiare il codice riportato nella sezione precedente in un file con estensione *.c* da denominare **gameOfLife.c**

Dando per scontato di avere un ambiente MPI installato e funzionante, bisogna compilare il file gameOfLife.c con il seguente comando:<br><br>

````
mpicc gameOfLife.c -o gameOfLife
````
<br><br>
Si otterrà, in questo modo, il file compilato, che dovrà essere eseguito con il seguente comando:<br><br>

````
mpirun -np P gameOfLife N K
````
<br>
Nel precedente comando:

* **P** rappresenta il numero di processori da utilizzare;
* **N** rappresenta la dimensione della matrice quadrata; 
* **K** rappresenta il numero di iterazioni (le generazioni) da computare.
<br><br>
<br><br>

#### Note sull'implementazione
L'implementazione della soluzione descritta nelle sezioni precedenti presenta alcune caratteristiche:<br>
1. Il processo Master partecipa alla computazione parallela insieme agli altri processi Slave, oltre ad effettuare attività di coordinamento e comunicazione; <br>
2. Nella comunicazione tra i processi è stato introdotto il Datatype **“row”**, che rappresenta una singola riga della matrice. <br>
L'utilizzo di questo Datatype ha comportato, in linea generale, un piccolo miglioramento in termini di efficienza sia per un basso valore che per un alto valore di N, oltre che una maggiore astrazione e chiarezza del codice;<br>
3. Eseguendo la soluzione proposta su un Cluster di istanze EC2 di tipo **m4.large**, è stato constatato che per un valore di N di circa 33.000, quindi per una matrice di circa 33.000 x 33.000, circa 1.08 miliardi di elementi, il processo Master va in **segmentation fault**.<br>
Suppongo che ciò sia dovuto al fatto che il processo Master non riesce ad allocare una matrice di elementi così grande e quando tenta di accedervi va appunto in segmentation fault.<br>
Una soluzione per aumentare la scalabilità del programma in termini di N sarebbe stata quella di implementare un processo Master che alloca ed inizializza un blocco di righe alla volta, da inviare, di volta in volta, agli altri processi.<br>
Tuttavia, ho ritenuto meglio implementare un processo Master che alloca ed inizializza la Matrice nella sua completezza un’unica volta, suddividendo ed inviando i blocchi di righe agli altri processi utilizzando la **Scatterv**, e traendo vantaggio dall’ottimizzazione di quest’ultima rispetto ad una normale comunicazione implementata tramite Send e Receive.<br>
La scelta di procedere in questo modo è dovuta anche al fatto che nei test per la scalabilità forte viene considerato N = 8.000, dimensione per la quale il programma sequenziale su istanze EC2 m4.large impiega circa 212 secondi e per la quale i risultati, successivamente riportati, mostrano un evidente **speedup** all’aumentare dei processi;<br>
4. La comunicazione tra Master e Slave, eseguita tramite le routine di comunicazione collettiva Scatterv e Gatherv, viene effettuata per ogni generazione.<br>
Una versione alternativa del codice vede l'esecuzione della Scatterv prima del ciclo for che itera le generazioni, e l'esecuzione della Gatherv al termine del suddetto ciclo.<br>
In questo modo, ciascun processo inizialmente riceve dal Master il blocco di righe che memorizza nella sua Matrice di Lettura Parziale, e in ogni generazione computa la Matrice di Scrittura Parziale.<br>
Al termine di ciascuna iterazione, la Matrice di Lettura Parziale assumerà il valore della Matrice di Scrittura Parziale appena computata *(Double Buffering)*.<br>
Questa impelementazione alternativa permette di ridurre l'overhead della comunicazione tra i processi ed il Master, tuttavia non consente di avere una visione della Matrice nella sua completezza in ogni generazione computata.

<br><br>
<br><br>


#### Correttezza dei risultati
Il processo Master (con rank = 0) inizializza la matrice in maniera random, grazie alla funzione *initializeMatrix()*, utilizzando come **seed** della funzione srand() il valore del rank stesso.<br>
Ciò consente di ottenere, per uno specifico valore di N, risultati sempre uguali al termine di ogni computazione, indipendentemente dal numero di processi utilizzati.<br><br>
Lo stato della matrice, in ogni generazione, viene visualizzato sul canale stdout (terminale), rendendo possibile effettuare un confronto con i risultati generati da altre esecuzioni.
<br><br><br><br>
Inoltre, per verificare ulteriormente la correttezza dell'algoritmo, l'inizializzazione della matrice può essere effettutata utilizzando due tipologie di **pattern** noti:<br>
1. i pattern *Still lifes*, che NON sono soggetti a cambiamenti durante le generazioni;<br>
2. i pattern *Oscillators*, che ritornano allo stato iniziale dopo un certo numero di generazioni.

<br>

Specificando un terzo parametro, denominato **J**, nel comando di esecuzione del file compilato è possibile selezionare una tra le **5 configurazioni** predefinite della matrice:

<br>

##### Configurazione 1

Per computare un pattern di tipo Still Lifes chiamato "**Block**", bisogna specificare la seguente combinazione di parametri:
* **N = 4** 
* **J = 1**

````
mpirun -np P gameOfLife 4 K 1
````
<br>

La seguente *Immagine 2* riporta la visualizzazione su stdout dei risultati generati dall'algoritmo.<br>
Si nota che tutte le generazioni restano invariate.

![](Immagine2_Block.jpg)
<br>
*Immagine 2: generazioni computate del pattern Block.*

<br><br><br>

##### Configurazione 2

Per computare un pattern di tipo Still Lifes chiamato "**Tub**", bisogna specificare la seguente combinazione di parametri:
* **N = 5** 
* **J = 2**

````
mpirun -np P gameOfLife 5 K 2
````
<br>

La seguente *Immagine 3* riporta la visualizzazione su stdout dei risultati generati dall'algoritmo.

![](Immagine3_Tub.jpg)
<br>
*Immagine 3: generazioni computate del pattern Tub.*

<br><br><br>

##### Configurazione 3

Per computare un pattern di tipo Oscillators chiamato "**Blink**", che ritorna alla configurazione iniziale dopo 2 generazioni (Periodo 2), bisogna specificare la seguente combinazione di parametri:
* **N = 5** 
* **J = 3**

````
mpirun -np P gameOfLife 5 K 3
````
<br>

La seguente *Immagine 4* riporta la visualizzazione su stdout dei risultati generati dall'algoritmo.


![](Immagine4_Blinker.jpg)
<br>
*Immagine 4: generazioni computate del pattern Blink.*

<br><br><br>

##### Configurazione 4

Per computare un pattern di tipo Oscillators chiamato "**Toad**", che ritorna alla configurazione iniziale dopo 2 generazioni (Periodo 2), bisogna specificare la seguente combinazione di parametri:
* **N = 6** 
* **J = 4**

````
mpirun -np P gameOfLife 6 K 4
````
<br>

La seguente *Immagine 5* riporta la visualizzazione su stdout dei risultati generati dall'algoritmo.



![](Immagine5_Toad.jpg)
<br>
*Immagine 5: generazioni computate del pattern Toad.*

<br><br><br>

##### Configurazione 4

Per computare un pattern di tipo Oscillators chiamato "**Beacon**", che ritorna alla configurazione iniziale dopo 2 generazioni (Periodo 2), bisogna specificare la seguente combinazione di parametri:
* **N = 6** 
* **J = 5**

````
mpirun -np P gameOfLife 6 K 5
````
La seguente *Immagine 6* riporta la visualizzazione su stdout dei risultati generati dall'algoritmo.



![](Immagine6_Beacon.jpg)
<br>
*Immagine 6: generazioni computate del pattern Beacon.*

<br>

**N.B.** 
* Se il binomio formato dal parametro N e dal parametro J non corrisponde ad una delle configurazioni precedentemente elencate, la matrice verrà inizializzata con tutti gli elementi in stato "**dead**", per sottolineare il possibile errore in fase di esecuzione del comando;
* Se invece il parametro J non viene definito o viene definito pari a 0 (zero), la matrice sarà normalmente inizializzata in maniera **random**.



<br><br>

#### Risultati
I risultati di seguito riportati mostrano il comportamento del programma in termini di **Scalabilità forte** e **debole**.<br>
In entrambi i casi, il programma è stato testato su un Cluster di 16 istanze EC2 di tipo m4.large, ciascuna delle quali possiede due vCPU.<br>
Sia per la Scalabilità forte che per quella debole, è stato scelto un valore **K** (iterazioni) pari a 100.<br><br>

##### Strong Scalability
I grafici di seguito riportati sono relativi alla Scalabilità forte, per la quale è stato scelto N pari a 8000.<br>

Il primo grafico mostra il **tempo di esecuzione** impiegato dal programma all'aumentare del numero dei processi.<br>
Si noti che l'esecuzione con un singolo processo richiede 212 secondi.<br>
All'aumentare del numero dei processi, il tempo di esecuzione cala significativamente, ottenendo un'**efficienza massima** utilizzando 14 processi, con un tempo di esecuzione pari a 99 secondi.<br>
Da quel punto in poi, il tempo di esecuzione smette di decrescere e, all'aumentare del numero dei processi, subisce un lieve ma costante incremento.<br>
Il tempo di esecuzione del programma con 32 processi è di 108 secondi.
<br><br>

![](ImageStrongScalability1.png)<br>
*Grafico 1: Scalabilità forte in termini di tempo di esecuzione.*

<br>

Il secondo grafico mostra lo **speedup** ottenuto dal programma all'aumentare del numero dei processi.<br>
Tale grafico rispecchia l'andamento del Grafico 1, ottenendo uno **speedup massimo** pari a 2,14 con 14 processi.<br>
Da quel punto in poi, lo speedup smette di crescere e, all'aumentare del numero dei processi, subisce un lieve ma costante decremento.
<br><br>

![](ImageStrongScalability2.png)


*Grafico 2: Scalabilità forte in termini di speedup.*

<br>

![](ImageDataStrongScalability.png)
<br>

*Tabella 1: Risultati ottenuti per la scalabilità forte in termini di tempo di esecuzione e speedup.*
<br><br>

##### Weak Scalability
La Scalabilità debole consente di osservare l'**overhead delle comunicazione** all'aumentare del numero dei processi.<br>
Per P = 1 è stato scelto N = 250.<br>
All'aumentare dei processi, la dimensione del problema è stato aumentata in maniera uniforme, fino ad una coppia di valori P = 32 e N = 8000.


Dal Grafico 3 si evince che la comunicazione tra i processi, che si scambiano righe di elementi, aumenta all'aumentare di P e di N, tracciando una sorta di iperbole.<br>
Ciò è dovuto al fatto che, al crescere di N, i processi inviano e ricevono a/da i vicini N elementi, un **valore crescente**, e non una costante.

<br>

![](ImageWeakScalability1.png)
<br>
*Grafico 3: Scalabilità debole in termini di tempo di esecuzione.*

<br>

![](ImageDataWeakScalability.png)

*Tabella 2: Risultati ottenuti per la scalabilità debole in termini di tempo di esecuzione.*
<br>
<br>

#### Conclusioni
Dalle analisi delle prestazioni riportate nella sezione precedente, è possibile dedurre che l'esecuzione in parallelo del programma è fortemente influenzata dal parametro N, ovvero dalla dimensione della matrice iniziale.<br>
L'overhead di comunicazione tra i processi cresce in maniera considerevole all'aumentare della taglia del problema (N) proprio perchè ciascun processo invia e riceve N elementi a/da i processi vicini.<br><br>
Inoltre, come anticipato nella sezione "Note sull'implementazione", l'algoritmo prevede la comunicazione tra Master ed ogni altro processo Slave tramite routine di comunicazione collettiva Scatterv e Gatherv rispettivamente all'inzio e alla fine di ciascuna iterazione, al fine di ottenere una rappresentazione della Matrice nella sua completezza sul nodo Master ad ogni generazione.
<br>
Questo tipo di comunicazione, associata ad un elevato valore di N, risulta essere onerosa ed impatta significativamente i tempi di esecuzione, come si osserva dalla Tabella 2, in cui il tempo di esecuzione cresce all'aumentare di P ed N, anzichè restare costante.
<br>
Pertanto, l'algoritmo si dimostra essere efficiente per un valore non eccessivo di N ottenendo, nel caso rappresentato, uno speedup massimo con 14 processi.
<br>
