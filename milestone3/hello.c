void printString(char*);

main() {
	int i=0;
	int j=0;
	int k=0;

	enableInterrupts();

	for(i=0; i<1000; i++) {
		printString("Hello\n\r\0");
		for(j=0; j<1000; j++) {
			for(k=0; k<1000; k++) {
			}
		}
	}
}