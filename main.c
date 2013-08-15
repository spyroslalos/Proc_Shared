/********ΔΟΥΛΕΥΕΙ ΤΕΛΕΙΑ ΧΩΡΙΣ ΒΗΜΑΤΑ 
ΓΙΑ ΚΑΘΕ PROCESS ME ΠΡΟΒΛΗΜΑ READERS WRITERS KAI PRODUCER-CONSUMER....**************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include "leitourgika.h"

//use shared memory, include the following
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include "list.h"
#include <sys/sem.h>
#define SHMKEY1 (key_t) 8844
#define SHMKEY2 (key_t) 8044
#define SEMKEY (key_t) 4488
#define PERM 0666

#define INSERTDELETEMUT 0			//
#define READCOUNTMUT 1				//
#define SEARCHDELETEMUT 2			//------>kanw define tis theseis sto sem array gia logoys eyxreistias
#define FULLSEM 3				//
#define EMPTYSEM 4				//
typedef enum { false, true } bool;

union senum{
  int val;
  struct semid_ds *buff;
  unsigned short * array;
};

//int sem_insert;		//insert semaphore
//int sem_delete;		//delete semaphore
int sem_id;

bool is_possible(float);
void up(int x,int num);		//synarthsh pou kanei down to semaphore sth thesh num
void down(int y,int num);	//synarthsh pou kanei up to semaphore sth thesh num

int main(int argc,char **argv){
//	srand(time(NULL));
	/*colors stin exodo*/
	const char *const red = "\033[0;40;31m";
	const char *const green = "\033[0;40;32m";
	const char *const lightblue = "\033[0;40;34m";
  	const char *const normal = "\033[0m";
	const char *const yellow = "\033[1;40;33m";
	int shm_id1;					//shared memory ids
	int shm_id2;					
	union senum arg0,arg1,arg2,arg3,arg4;
	Element ele;	
	int inserts;					//sunolo insert praxewn oxi processes				
	int deletes;					//sunolo delete	praxewn oxi processes
	pid_t  	pid,*pids;
	int i,l;
	int status;
	List *pmem;		//pointer to shared memory gia lista
	int *shreadcount;	//pointer to shared memory interger: readcount
	List lista;
	int search_no,insert_no,delete_no;		//plithos processes kathe typou pou tha dimiourgithoun
	int search_steps,insert_steps,delete_steps;
//	float p_search,p_insert,p_delete;
	bool is_search=false;				//
	bool is_insert=false;				//------------>boolean variable-in which kind of process i am.
	bool is_delete=false;				//
	if( argc != 7){
		printf("You gave me wrong arguments\n");
		printf("Give me 6 arguments: <search number> <insert number> <delete number> <search_steps> <insert_steps> <delete_steps>\n");
		return -1;
	} 						
	search_no=atoi(argv[1]);
	insert_no=atoi(argv[2]);
	delete_no=atoi(argv[3]);
	search_steps=atoi(argv[4]);
	insert_steps=atoi(argv[5]);
	delete_steps=atoi(argv[6]);
	inserts=insert_no*insert_steps;				//to synolo twn inserts = (plithos insert processes)*(vimata kathe mias)
	deletes=delete_no*delete_steps;				//to synolo twn delete = (plithos delete processes)*(vimata kathe mias)			
	//pids' array initialized to zero
	if ((pids=calloc((search_no+insert_no+delete_no),sizeof(pid_t)))==NULL ){	
		perror("calloc");
		return -1;
	}
	shm_id1=shmget (SHMKEY1,sizeof(List),PERM|IPC_CREAT);
	if(shm_id1==-1)
                printf ("\nError in shared memory about list region setup.\n");
        else{
                printf ("\nShared memory for list succesfully created\n");
                pmem= shmat(shm_id1, (List*)0, 0);	//attaches the shared mem to pmem
        }
	create(pmem);	
	pmem->insert_count=inserts;			
	pmem->delete_count=deletes;

	shm_id2=shmget (SHMKEY2,sizeof(int),PERM|IPC_CREAT);
	if(shm_id2==-1)
                printf ("\nError in shared memory about readcount region setup.\n");
        else{
                printf ("\nShared memory for readcount succesfully created\n");
                shreadcount= shmat(shm_id2, (int*)0, 0);	//attaches the shared mem to shreadcount
        }
	*shreadcount=0;
	/**--------CREATING SEMAPHORES-------------**/
	
	sem_id=semget(SEMKEY,5,PERM | IPC_CREAT);		//dhmiourgia array of 5 semaphores 
	if (sem_id < 0) {
		printf("ta kana xaliaaaaaaaaa\n");
                perror("semget");
                exit(1);
        }
        else{
                printf("Creating a semaphore with ID: %d \n",sem_id);
        }
	arg0.val=1;
	arg1.val=1;
	arg2.val=1;
	arg3.val=0;
	arg4.val=PLITHOS;
	printf("PLIThOS !!!! %d\n",PLITHOS);
	if (semctl(sem_id,0, SETVAL, arg0) <0) {			//
                perror("semctl");
                return -1;
        }
	if (semctl(sem_id,1, SETVAL, arg1) <0) {			//
                perror("semctl");
                return -1;
        }
	if (semctl(sem_id,2, SETVAL, arg2) <0) {			//--------semaphores' initialization
                perror("semctl");
                return -1;
        }
	if (semctl(sem_id,3, SETVAL, arg3) <0) {			//
                perror("semctl");
                return -1;
        }
	if (semctl(sem_id,4, SETVAL, arg4) <0) {			//
                perror("semctl");
                return -1;
        }
	

	/**Loops for creating processes*/
	for(i=0;i<search_no;i++){			//loop for creating search processes
		pid=fork();
		pids[i]=pid;			//save pid in array with pids...
    		if ( pid < 0 ){
           		perror("fork about search");
              		return -1;
          	}
		else if (!pid){		//eimai se paidi
			is_search=true;				
			break;
		}	
	}
	for(i=0;i<insert_no && pid!=0;i++){		//loop efoson den eimai se child...an eimai se child den mpainei...
		pid=fork();
		pids[i+search_no]=pid;
		if( pid<0 ){
			perror("fork about insert");
			return -1;
		}
		else if(!pid){
			is_insert=true;			//boolean metavliti deixnei metepeita se ti eidos process vriskomai
			break;
		}	
	}
	for(i=0;i<delete_no && pid!=0;i++){		//loop efoson den eimai se child...an eimai se child den mpainei...
		pid=fork();
		pids[i+search_no+insert_no]=pid;
		if( pid<0 ){
			perror("fork about insert");
			return -1;
		}
		else if(!pid){
			is_delete=true;
			break;
		}	
	}

	if(!pid){					//an eimai se child process....
		srand(getpid());
		sleep(1);
					
		if(is_search==true){
			for(i=0; i<search_steps; i++){			//eimai se child process SEARCH
				down(sem_id,READCOUNTMUT);				//leitoyrgia tupou readers writers
				*shreadcount=*shreadcount+1;
				if(*shreadcount==1)
					down(sem_id,SEARCHDELETEMUT);
				up(sem_id,READCOUNTMUT);
				printf("I am a %ssearch%s child process with %spid%s....%d\n",lightblue,normal,yellow,normal,getpid());
				search_elem(*pmem);			//kalw synarthsh Search
				down(sem_id,READCOUNTMUT);		//kanw down to sem READCOUNT gia na grapsw sto 
				*shreadcount=*shreadcount-1;
				if(*shreadcount==0)
					up(sem_id,SEARCHDELETEMUT);
				up(sem_id,READCOUNTMUT);
				sleep(2);				//adranopoiish meta apo kathe vima
			}	
			return 0;		
		}		
		else if(is_insert==true){			//eimai se child process INSERT
			for(i=0; i<insert_steps; i++){
				down(sem_id,EMPTYSEM);
				down(sem_id,INSERTDELETEMUT);
				if(full(*pmem)==0 || ( full(*pmem)==1 && pmem->delete_count!=0 )  ){			//an h lista den einai gemati
					printf("I am an %sinsert%s child process with %spid%s....%d\n",green,normal,yellow,normal,getpid());	
					printf("insert count is %i\n", pmem->insert_count);
				//	printf("I am an %sinsert%s child process with %spid%s....%d\n",green,normal,yellow,normal,getpid());			
					insert_elem(pmem);			//eisagwgh stoixeiou
					(pmem->insert_count)--;
					if(pmem->delete_count==0 && full(*pmem)==1 ){
						up(sem_id,EMPTYSEM);
					}

					up(sem_id,INSERTDELETEMUT);				//up to semaphore full.....+1 sta gemata
					up(sem_id,FULLSEM);
				
				}
				else {			//an h lista einai gemati kai den exw delete na ginoun
					printf("CANNOT INSERT !!!!!!!!!!!!!!!!!!\n");
					up(sem_id,FULLSEM);
					up(sem_id,EMPTYSEM);
					up(sem_id,INSERTDELETEMUT);
					break;
				}		
				sleep(1);			//adranopoiish meta apo kathe vima
			}
			return 0;
		}

		else if(is_delete==true){		//eimai se child process DELETE
		
			for(i=0; i<delete_steps; i++){

				down(sem_id,FULLSEM);			//an einai 0 dld kanena gemato h delete blockaretai
				down(sem_id,SEARCHDELETEMUT);		//an kapoia search to ekane down h delete blockaretai 
				down(sem_id,INSERTDELETEMUT);		//an kapoia insert to ekane down h delete blockarete
				
				if(empty(*pmem)==0 || ( empty(*pmem)==1 && pmem->insert_count!=0 )){						
					
					printf("delete count is %i\n", pmem->delete_count);
					printf("I am a %sdelete%s child process with %spid%s....%d\n",red,normal,yellow,normal,getpid());
					delete_elem(pmem,&ele);	
					(pmem->delete_count)--;
					if(pmem->insert_count==0 && empty(*pmem)==1 ){		/*den exw alles insert kai einai adeia tote kane up to full etsi wste
												na mpoun mesa oi alles delete kai na mpoun sto else kai na termatisoun */
						up(sem_id,FULLSEM);
					}
					up(sem_id,INSERTDELETEMUT);
					up(sem_id,EMPTYSEM);					/*ayxise arithmo eleftherwn keliwn kata 1*/
					up(sem_id,SEARCHDELETEMUT);
				}
				else{
					printf("CANNOT DELETE!!!!\n");
					up(sem_id,INSERTDELETEMUT);
					up(sem_id,EMPTYSEM);
					up(sem_id,FULLSEM);
					up(sem_id,SEARCHDELETEMUT);
			
					break;
		
				}
				sleep(2);			//adranopoiish meta apo kathe vima
			}
			return 0;
		}
	}
	else if(pid!=0){						//eimai sto patera
		printf("%sI am the father%s...\n",yellow,normal);

		for(l=0; l<(search_no+insert_no+delete_no); l++){
		
			pids[l] = wait(&status); // Perimenei o pateras na termatisoun ta paidia 
			if(pids[l]!=-1)
			printf("The process %d just finished\n",pids[l]);
		}
		free(pids);				//apodesmeyse xwro pou desmeyses dynamika me calloc gia ta pids
		printf("KANW TWRA DOULEIA>>>>\n");
		//detaches the shared memory segment before destroying it
		if ( shmdt(pmem) == -1  ){
               		perror("shmdt");
               		return -1;
        	}
                if ( shmdt(shreadcount) == -1  ){
                        perror("shmdt");
                        return -1;
                }
		//To destroy a shared memory segment use:
		if ( shmctl(shm_id1,IPC_RMID,NULL) == -1  ){
               		perror("shmctl");
               		return -1;
        	}
                if ( shmctl(shm_id2,IPC_RMID,NULL) ==-1){
                        perror("shmctl");
                        return -1;
                }

		if ( semctl(sem_id,1, SETVAL,NULL) ==-1){			//
                perror("semctl");
                	return -1;
		}
		if ( semctl(sem_id,2, SETVAL,NULL)==-1) {			//--------semaphores' Delete
		        perror("semctl");
		        return -1;
		}
		if ( semctl(sem_id,3, SETVAL,NULL)==-1) {			//
		        perror("semctl");
		        return -1;
		}
		if ( semctl(sem_id,4, SETVAL,NULL)==-1) {			//
		        perror("semctl");
		        return -1;
		}
	printf("End of work\n");	
	}
	//printf("End of work\n");
	return 0;

}
void up(int x,int num){
  struct sembuf oper[1]={num,1,0};
  semop(x,&oper[0],1);
}

void down(int y,int num){
  struct sembuf oper[1]={num,-1,0};
  semop(y,&oper[0],1);
}

bool is_possible(float p_lim){			/*h synarthsh telika den xrhsimopoihthike sta plaisia tis askisis*/
	float x,y;
	x=rand()%10;
	y=x/10;
	if(y<p_lim)
		return true;
	else
		return false;
}


