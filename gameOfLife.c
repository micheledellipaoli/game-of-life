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

	//Computazione da effettuare su singolo processore
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

	}else if(sizeWorld>1){	//Computazione da effettuare con processori multipli

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
