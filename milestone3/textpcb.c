#define MAIN
#include <stdio.h>
#include "pcb.h"

int main(){
	struct PCB *pcb;
	initializeProcStructures();
	int i;
	for(i=0;i<8;i++){
		int segment = getFreeMemorySegment();
		int segmentAddr = (segment + 2) * 0x1000;
		pcbPool[segment].state = STARTING;
		pcbPool[segment].segment = segmentAddr;
		pcbPool[segment].stackPointer = 0xFF00;
		addToReady(pcbPool + segment);
	}

	for(i=0;i<8;i++){
		running=removeFromReady();
		printf("%04x\n", running->segment);
		addToReady(running);
	}
	return 0;
}