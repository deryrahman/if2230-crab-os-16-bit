#include "pcb.h"

/*
 * This function initializes all of the global variables and 
 * structures defined above.  All entries in the memory map should be marked as FREE.
 * All of the PCBs in the pcbPool should have the first character of their 
 * names set to null, their state set to DEFUNCT and their segment and stack
 * pointers set to 0x0000. The PCB for the idle process should be initialized 
 * with the name IDLE, the state READY, and both the segment and stack pointers
 * set to 0x0000.  running should refer to the PCB for the idle process and the ready queue
 * should be empty.
 */  
void initializeProcStructures(){
	int i;
	readyHead = readyTail = 0;
	for(i = 0; i < 8; i++){
		memoryMap[i]=FREE;
		pcbPool[i].name[0]=NULL;
		pcbPool[i].state=DEFUNCT;
		pcbPool[i].segment=0x0000;
		pcbPool[i].stackPointer=0x0000;
	}
}

/*
 * Returns the index of the first free memory segment or -1 if 
 * all of the memory segments are used.
 */
int getFreeMemorySegment(){
	int i;
	for(i = 0; i < 8; i++){
		if(memoryMap[i]==FREE) return i;
	}
	return -1;
}

/*
 * Release the indicated memory segment.
 */
void releaseMemorySegment(int seg){
	seg = div(seg,0x1000)-2;
	pcbPool[seg].state=DEFUNCT;
	memoryMap[seg]=FREE;
}

/*
 * Return a pointer to an available PCB from the PCB pool.  All of the
 * available PCBs should be marked as DEFUNCT.  Return NULL if there are
 * no PCBs available.
 */
struct PCB *getFreePCB(){
	int i;
	for(i = 0; i < 8; i++){
		if(pcbPool[i].state==DEFUNCT)
			return pcbPool + i;
	}
	return NULL;
}

/*
 * Release the provided PCB by setting its state to DEFUNCT, setting
 * its next and prev pointers to NULL and setting the first character
 * of its name to 0x00.
 */
void releasePCB(struct PCB *pcb){
	pcb->name[0]=NULL;
	pcb->state=DEFUNCT;
}

/*
 * Add the provided PCB to the tail of the ready queue.
 */
void addToReady(struct PCB *pcb){
	if(readyTail==NULL)
		readyTail=readyHead=pcb;
	else{
		readyTail->next=pcb;
		pcb->next=NULL;
		pcb->prev=readyTail;
		readyTail=pcb;
	}
}

/*
 * Remove the PCB at the head of the ready queue and return a 
 * pointer to it.
 */
struct PCB *removeFromReady(){
	struct PCB* ret;
	ret = readyHead;
	readyHead = readyHead->next;
	if (readyHead == 0)
		readyTail = 0;
	return ret;
}