#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/*
 * here is the main thing about this project:
 * I need to code a "best effort hash table". i suspect this will be done through 2d arrays and a special struct class to include the tag index etc.
 * through the main method, i need to calculate the assoc and the set size and the subsequent block and their contents. i do NOT know how to implement the read and write stuff except that it depends on the type of associativity i think.
 * WHAT IS PREFETCH??????
 *first, create the parameters of my structures and then do in and fill them in later.
 *
 *argv[0] = ./first
 *[1] = cache size
 *[2] = associativity
 *[3] = cache policy ("fifo" or "lru")
 *[4] = block size
 *[5] = trace file
 *
 */ 
//I'm going to declare my variables that will be used in all of my functions
//If i'm doing the extra credit, i;ll make specific ones for that


//LRU prefetch helper method read
//LRU prefetch helper method write
//LRU method read
//LRU method write
//main method


//for no prefetch AKA regular:
int readR; //memory reads,
int writeR; //memory writes
int hitR;//hit for no prefetch
int missR; //misses for no prefetch
unsigned long int countR;

//for prefetch
int readP;//memory read for prefetch
int writeP;//memory writes for prefetch
int hitP; //hits for prefetch
int missP;//misses for prefetch
unsigned long int countP; //i will use this to keep track of whichever one is the least value in FIFO

//declare the node for my cache. it's the simplest block which has tag, index, and block offset. DO NOT NEED BLOCK OFFSET FOR THIS ASSIGNMENT. INSTEAD NEED TO SEE HOW MANY TIMES IT HAS BEEN ACCESSED!!!!
//why is this running an error when compiling?
struct cBlock** cacheR; //the regular cache
struct cBlock** cacheP; //the prefetch cache

typedef struct cBlock{
	int valid; 
	unsigned long int tag;
	unsigned long int linkCount;
}cBlock;

//SIGNATURES:
//structure cBlock here
bool powerOfTwo(int x);//power of method
void prefetchHelper(unsigned long int tagIndex, unsigned long int setIndex, int assoc);//prefetch helper method read
void writePrefetch(unsigned long int tagIndex, unsigned long int setIndex, int assoc, unsigned long int ptagIndex, unsigned long int psetIndex, int marker);//prefetch read
void readPrefetch(unsigned long int tagIndex, unsigned long int setIndex, int assoc, unsigned long int ptagIndex, unsigned long int psetIndex, int marker);//prefetch write
void writeReg(unsigned long int tagIndex, unsigned long int setIndex, int assoc, int marker);
void readReg(unsigned long int tagIndex, unsigned long int setIndex, int assoc, int marker);


//char type;

/*
typedef struct cBlock{
	int valid; //0 or 1, this is the value bit to see if it's been accessed yet
	unsigned long int tag; //keep the tag value
	int linkCount; //how many times
};
*/

//struct cBlock** cache;
bool powerOfTwo(int x) {
//	printf("beg power\n");
	return (x & (x-1)) == 0;
//	printf("end power\n");
}
//readPrefetch helper
//write prefetch helper!
//void readPrefetchHelper(unsigned long int tagIndex, unsigned long int setIndex, int assoc){
void prefetchHelper(unsigned long int tagIndex, unsigned long int setIndex, int assoc){
//	printf("beg read pref helper\n");	
	int i = 0; int j = 0; int first;
	for (i = 0; i < assoc; i++){
		if (cacheP[setIndex][i].valid == 0) {
			readP++;
			countP++;		
			cacheP[setIndex][i].tag = tagIndex;
			cacheP[setIndex][i].valid = 1;
			cacheP[setIndex][i].linkCount = countP;
			return;
		}	else {
			if (cacheP[setIndex][i].tag == tagIndex) {	
				//im not gonnna increment the variables bc i've done this already	
				return;
			} 
			if (i == (assoc-1)){
				readP++;
				 first = 0;			
				for (j = 0; j < assoc; j++) {
					if ((cacheP[setIndex][j].linkCount < cacheP[setIndex][first].linkCount) || (cacheP[setIndex][j].linkCount < cacheP[setIndex][first].linkCount)) {
						first = j;				
					}				
				}
				cacheP[setIndex][first].valid = 1;
				cacheP[setIndex][first].tag = tagIndex;
				countP++;
				cacheP[setIndex][first].linkCount = countP;
				return;
			}
		}
	
	
	}
//	printf("end read pref helper\n");
}

void writePrefetch(unsigned long int tagIndex, unsigned long int setIndex, int assoc, unsigned long int ptagIndex, unsigned long int psetIndex, int marker) {
/*
* the write code is very similar to read except there's the extra part of immediately writing the data back into memory
* i need to keep track of the last one for the FIFO algorithm, implement this into my read function
* i need to use the regular variables!!!!!!!!!!!
* fuck ALL the above:
*i'm going to do what i did in prefetch for read.
*/
//write-hit: immediately write back into memory, DIRECTLY into memory. literally the same code as a read EXCEPT incrememnt the write regular variable
//	printf("beg write pref\n");	
	int i = 0; int j = 0; int first; 
	for (i = 0; i < assoc; i++) {
		if (cacheP[setIndex][i].valid ==0) {
			writeP++; //increment the write counter too
			readP++;		
			missP++;
			countP++;
			//now put the mem address into the cache
			cacheP[setIndex][i].tag = tagIndex;
			cacheP[setIndex][i].linkCount = countP;
			cacheP[setIndex][i].valid =1;
			//it's a miss so i'm going to kick something out according to FIFO and replace it with whatever is at my new tag index
			prefetchHelper(ptagIndex, psetIndex, assoc);			
			return;		
		}else {
			/*
				if (cacheP[setIndex][i].tag == tagIndex-1) {
				hitP++;			
				writeP++;
				return;
			}
			*/
			//case 1 if i have a hit i dont need to write
			if (cacheP[setIndex][i].tag == tagIndex) {
				hitP++;			
				writeP++;
				if (marker == 1){
					countP++;
					cacheP[setIndex][i].linkCount=countP;
							
				}
				return;
			}
			//replace that ish
			/*
			*so i'm having a major issue getting this work. it wont access the last block. SOLVED
			* case where the minimum is equal to the smallest value
			*/
			if (i == (assoc-1)) {
				missP++;
				readP++;
				writeP++;
				first = 0;
				for (j = 0; j < assoc; j++) {
					if ((cacheP[setIndex][j].linkCount < cacheP[setIndex][first].linkCount) || (cacheP[setIndex][j].linkCount == cacheP[setIndex][first].linkCount) ) {				
						first = j;
					}	
				}	
				//now update the minimum tag
				cacheP[setIndex][first].valid = 1; //mark as visited
				countP++;
				cacheP[setIndex][first].linkCount = countP;
				cacheP[setIndex][first].tag = tagIndex;
				prefetchHelper(ptagIndex, psetIndex, assoc);				
				return;
			}
		
		}	
	
	}
//	printf("end write pref\n");
	return;
}

void readPrefetch(unsigned long int tagIndex, unsigned long int setIndex, int assoc, unsigned long int ptagIndex, unsigned long int psetIndex, int marker) {
//	printf("beg read pref\n");	
	/*
	* for prefetch, i do the same exact thing as everythig else EXCEPT i incrememnt different variables.
	* steps: for hit, continue as normal. for miss, read the next block in (which block??) and use my FIFO replacement policy to kick out the first block i put in.
	*for this, i can either attempt to put it in my actual prefetch method or make a helper method. i'm gonna do in this method first and then see if i actually need to put
	*it into a special helper method.
	*/		
		int i = 0; int j = 0; int first;
		for (i = 0; i < assoc; i++) {
			/*if i realize that it hasn't been touched AKA its valid bit is 0, i will change the valid bit to 1
			 * and then change the Miss counter and the Write counter to +1. i will have to change the node that i'm at to make the valid bit 1,
			 * the tag the one that i've already indexed, and make the number of times it's been hit changed also.
			 */ 
			 //not valid case, hasn't been accessed yet
			 //according to lecture slides: if tag matches hit, use data if not then mark as a miss and fetch data from memory
				if (cacheP[setIndex][i].valid == 0) { //ISSSSA MISSSSSSSS
						countP++;
						readP++;
						missP++;
						//now put the mem address into the cache
						cacheP[setIndex][i].linkCount = countP;
						cacheP[setIndex][i].tag = tagIndex; 
						cacheP[setIndex][i].valid = 1; //mark is as has been accessed
						//call my prefetch helper method here						
						prefetchHelper(ptagIndex, psetIndex, assoc);		//the ptag is gonna be the value i already calculated.
						return;
				}	else {
						if (cacheP[setIndex][i].tag == tagIndex){ //if i find it in there i'm just gonna mark it as a hit
							if (marker == 1) {
								countP++;
								cacheP[setIndex][i].linkCount = countP;
							} 							
 							hitP++;
							return;						
						}
						//if i'm on the second to last one
						//FIFO replacement policy! the first in will be the one that i need to put in.
						//the count variable exists because i have to keep track of which one is the first 
 						if (i == (assoc-1)) {
 							missP++; //mark as miss and then write it in
							readP++; 						
 							first = 0;
 							for (j = 0; j < assoc; j++) {
 								if ((cacheP[setIndex][j].linkCount < cacheP[setIndex][first].linkCount) || (cacheP[setIndex][j].linkCount == cacheP[setIndex][first].linkCount) ) 
 									 first = j; //i got rid of the old first so now this is what i have as the least.
 							}
 						
 						countP++; //increment the count so now i know which one is the lsat
 						cacheP[setIndex][first].linkCount = countP;
 						cacheP[setIndex][first].tag = tagIndex;
						cacheP[setIndex][first].valid = 1;
						prefetchHelper(ptagIndex, psetIndex, assoc); 						
 						return;
 					}
			}		
	}	
//	printf("end pref read\n");
	return;
}

void writeReg(unsigned long int tagIndex, unsigned long int setIndex, int assoc, int marker) {
/*
* the write code is very similar to read except there's the extra part of immediately writing the data back into memory
* i need to keep track of the last one for the FIFO algorithm, implement this into my read function
* i need to use the regular variables!!!!!!!!!!!
*/
//write-hit: immediately write back into memory, DIRECTLY into memory. literally the same code as a read EXCEPT incrememnt the write regular variable
//	printf("beg writeReg\n");	
	int i = 0; int j = 0; int first; 
	for (i = 0; i < assoc; i++) {
		if (cacheR[setIndex][i].valid ==0) {
			writeR++; //increment the write counter too
			readR++;		//when i write, do not increment the read counter
			missR++;
			countR++;
			//now put the mem address into the cache
			cacheR[setIndex][i].tag = tagIndex;
			cacheR[setIndex][i].linkCount = countR;
			cacheR[setIndex][i].valid =1;
			return;		
		}else {
			//case 1 if i have a hit i dont need to write
			if (cacheR[setIndex][i].tag == tagIndex) {
				hitR++;			
				writeR++;
				
				if (marker == 1){
					countR++;
					cacheR[setIndex][i].linkCount = countR;				
				}
				return;
			}
			if (i == (assoc-1)) { //replace it
				missR++;
				readR++; //when i write, do not increment the read counter
				writeR++;
				first = 0;
				for (j = 0; j < assoc; j++) {
					if ((cacheR[setIndex][j].linkCount < cacheR[setIndex][first].linkCount) || (cacheR[setIndex][j].linkCount == cacheR[setIndex][first].linkCount) ) {				
						first = j;
					}	
				}	
				//now update the minimum tag
				cacheR[setIndex][first].valid = 1; //mark as visited
				countR++;
				cacheR[setIndex][first].linkCount = countR;
				cacheR[setIndex][first].tag = tagIndex;
				return;
			}
		
		}	
	
	}
//	printf("end writeReg\n");
	return;
}
void readReg(unsigned long int tagIndex, unsigned long int setIndex, int assoc, int marker) {
	
		//IN LRU, THE MARKER WILL BE SET TO 1!	
	
//		printf("beg readReg\n");		
		int i = 0; int j = 0; int first;
		for (i = 0; i < assoc; i++) {
			/*if i realize that it hasn't been touched AKA its valid bit is 0, i will change the valid bit to 1
			 * and then change the Miss counter and the Write counter to +1. i will have to change the node that i'm at to make the valid bit 1,
			 * the tag the one that i've already indexed, and make the number of times it's been hit changed also.
			 */ 
			 //not valid case, hasn't been accessed yet
			 //according to lecture slides: if tag matches hit, use data if not then mark as a miss and fetch data from memory
			 /*
			 *
			 *
			 */
				if (cacheR[setIndex][i].valid == 0) { //if whatever i'm at is a non-valid bit
						countR++; //increment the count as well as the read and miss c
						readR++;
						missR++; 
						//now put the mem address into the cache
						cacheR[setIndex][i].linkCount = countR;
						cacheR[setIndex][i].tag = tagIndex; 
						cacheR[setIndex][i].valid = 1; //mark is as has been accessed
						return;
				}else{ //if it's a hit i'm just going to increment the hit 
						if (cacheR[setIndex][i].tag == tagIndex){ //if i find it in there i'm just gonna mark it as a hit
 							hitR++;
 							//LRU!!!!!!!!!
							if (marker == 1) {
							countR++;
							cacheR[setIndex][i].linkCount = countR;
							}							
							return;						
						}
						//if i'm on the second to last one
						//FIFO replacement policy! the first in will be the one that i need to put in.
						//the count variable exists because i have to keep track of which one is the first 
 						if (i == (assoc-1)) {
  							missR++; //mark as miss and then write it in
				 			readR++; 						
 							first = 0;
 							for (j = 0; j < assoc; j++) {
 								if ((cacheR[setIndex][j].linkCount < cacheR[setIndex][first].linkCount) || (cacheR[setIndex][j].linkCount == cacheR[setIndex][first].linkCount) ) {
 									 first = j; //i got rid of the old first so now this is what i have as the least.
								} 							
 							}
 						countR++; //increment the count so now i know which one is the lsat
 						cacheR[setIndex][first].linkCount = countR;
 						cacheR[setIndex][first].tag = tagIndex;
						cacheR[setIndex][first].valid = 1;
 						return;
 					}
			}
	}
//	printf("end readReg\n");
return;
}	


int main (int argc, char* argv[]) {
	readR = 0; writeR = 0; hitR = 0; missR = 0; countR = 0;
	readP = 0; writeP = 0; hitP = 0; missP = 0; countP = 0;
	//error if there aren't 6  arguments
	int bbits;
	int sbits;
	char type;
	//i'm going to split the variables into prefetched variables and non-prefetched variables.
	//regardless of the cache policy, i will have to implement both prefetch AND non-prefetch.
	unsigned long int address; unsigned long int paddress;
	//unsigned long int setMask;
	unsigned long int tagIndex; unsigned long int ptagIndex;
	unsigned long int setIndex; unsigned long int psetIndex;

	if (argc != 6) {
		printf("error");
		return 0;
	}
	//open the trace file
	FILE * file = fopen(argv[5], "r");
	if (file == NULL) {
		printf("error");
		return 0;
	}
	int cacheSize = atoi(argv[1]);
	int blockSize = atoi(argv[4]);
	//block size, cache Size, and n for assoc:n has to be 
	if (!powerOfTwo(cacheSize)){
		printf("error");	
		return 0;	
	}
	if (!powerOfTwo(blockSize)){
		printf("error");
		return 0;	
	}

	//error
//	if (!isPowOfTwo(argv)) {
//		printf("error");
//		return 0;
//	}

	//calculate associativity
	//direct: 1; assoc: # of blocks which is cache size/block size;
	//assoc:n is just n which would be argv[2][6] with atoi called on it

	int assoc = 0; int sets = 0;
	
	
	
	
	//fully associative 
	if (strcmp(argv[2], "assoc") == 0) {
		//assoc = cache size / block size
		assoc = atoi(argv[1]) / atoi(argv[4]);	
	//this is a fully associated cache so sets will be only 1
		sets = 1;
	
	//directly associative
	} else if (strcmp(argv[2], "direct") == 0) {
		assoc = 1;
		//sets = atoi(argv[1]) / (assoc * atoi(argv[4]));
		sets = atoi(argv[1]) / atoi(argv[4]);
	
	//assoc:n...... WHY IS THIS NOT WORKING?
	} else {
		assoc = atoi(&argv[2][6]); //the number that comes after the colon
		//printf("this is the assoc of assoc:n %d\n", assoc);		
		//make sure this is a power of 0!
		if (!powerOfTwo(assoc)) {
			printf("error");
			return 0;
		}
		//int n = assoc;
		sets = atoi(argv[1]) / (assoc * atoi(argv[4]));
		
	}

	//sets
	//cache size = S x A x Block Size
	//S = cache size/ (A x Block Size)

//	int sets = 0;
//	sets = atoi(argv[1]) / (assoc * atoi(argv[4]));

//	cacheR = createR(sets, assoc); //why wont this initialize
//	cacheP = createP(sets, assoc); //why wont this initialize
//why i dont just create the caches here
//make cacheP
	int i = 0; int j = 0;
	cacheP =(cBlock**)malloc(sets* sizeof(cBlock*));
	//why isnt my cache initializing?
	
	//initialize all the valid bits to 0
	for (i = 0; i < sets; i ++) {
			cacheP[i]=(cBlock*)malloc((assoc)*sizeof(cBlock));
	}	
	for (i = 0; i < sets; i ++) {
		for (j = 0; j < assoc; j++) {
			//cacheP[i][j]->valid = 0;
			cacheP[i][j].valid = 0;
		}
	}
	
	//make cacheR 
	cacheR = (cBlock**) malloc ((sets) * sizeof(cBlock*));
	//why isnt my cache initializing?
	for (i = 0; i < sets; i ++) {
			cacheR[i]=(cBlock*)malloc((assoc) * sizeof(cBlock));
	}	
	
	//initialize all the valid bits to 0
	for (i = 0; i < sets; i ++) {
		for (j = 0; j < assoc; j++) {
			//cacheR[i][j]->valid = 0;
			cacheR[i][j].valid = 0;
		}
	}
	
	bbits = (log(atoi(argv[4])) / log(2)); 
	//log block size / log 2
	//bbits = 4; this worked!!!!
	//printf("bbits is: %d\n", bbits);
	sbits = log(sets) / log(2); //log sets / log 2
	//printf("sbits is: %d\n", sbits);
	//int sbitspow2 = ((int) pow(2, sbits));
	//printf("sbitspow2 is %d\n", sbitspow2);
	//setMask = ((1<<sbits)-1); //mask the set bits
	//int total = 0; int rScans=0; int wScans=0;


	while (fscanf (file, "%*x: %c %lx", &type, &address) == 2) {
		
	//	setIndex = (address >> bbits) & setMask;
		tagIndex = address >> (sbits + bbits); //u have to shift right by the number of bits that i have for the block size and the set size
		setIndex = (address >> bbits) % ((int) pow(2, sbits));	
		
				
		paddress = address + atoi(argv[4]);
		
		//psetIndex = (paddress >> bbits)&setMask;
		
		psetIndex = (paddress >> bbits) % ((int) pow(2, sbits));		
		ptagIndex = paddress >> (bbits+sbits);

		if((strcmp(argv[3], "fifo") == 0)) {
			if (type == 'R') {
				//rScans++;
				readReg(tagIndex, setIndex, assoc, 0); //done
				//readR++;
				//countR++;
				readPrefetch(tagIndex, setIndex, assoc, ptagIndex, psetIndex, 0);
			} else if (type == 'W') {
				//wScans++;
				writeReg(tagIndex, setIndex, assoc, 0); //done
				//readR++;
				//writeR++;
				//countR++;
				writePrefetch(tagIndex, setIndex, assoc, ptagIndex, psetIndex, 0);
			}
	
		} 
		else if ((strcmp(argv[3], "lru") == 0)) {
			if (type == 'R') {
				readReg(tagIndex, setIndex, assoc, 1);
				readPrefetch(tagIndex, setIndex, assoc, ptagIndex, psetIndex, 1);
			} else if (type == 'W') {
				writeReg(tagIndex, setIndex, assoc, 1);
				writePrefetch(tagIndex, setIndex, assoc, ptagIndex, psetIndex, 1);
			}
		}
		
	
 	}
//	printf("this is the total: %d\n", total);
 //	printf("this is the number of reads: %d\n", rScans);
//	printf("this is the number of writes: %d\n", wScans);
	printf("no-prefetch\n");
 	printf("Memory reads: %d\nMemory writes: %d\nCache hits: %d\nCache misses: %d\n", readR, writeR, hitR, missR);
//now empty the no-prefetch cache

//	int i = 0; int j = 0;
	for (i = 0; i < sets; i++) {
		for (j = 0; j < assoc; j++) {
			cacheR[i][j].tag = 0;
			cacheR[i][j].valid = 0;
			cacheR[i][j].linkCount = 0;
		}
	}	
	readR = 0; writeR = 0; hitR = 0; missR = 0; countR = 0;

	printf("with-prefetch\n");
 	printf("Memory reads: %d\nMemory writes: %d\nCache hits: %d\nCache misses: %d\n", readP, writeP, hitP, missP);	
//now empty the with-prefetch cache
	//int i = 0; int j = 0;
	for (i = 0; i < sets; i++) {
		for (j = 0; j < assoc; j++) {
			cacheP[i][j].tag = 0;
			cacheP[i][j].valid = 0;
			cacheP[i][j].linkCount = 0;
		}
	}
	readP = 0; writeP = 0; hitP = 0; missP = 0; countP = 0;

return 0;
}
