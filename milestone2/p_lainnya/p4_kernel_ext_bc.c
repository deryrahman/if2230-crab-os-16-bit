#define PRINT_STRING 0
#define READ_STRING 1
#define READ_SECTOR 2
#define READ_FILE 3
#define EXECUTE 4
#define TERMINATE 5
#define WRITE_SECTOR 6
#define DELETE_FILE 7
#define WRITE_FILE 8
#define GET_DIR 9
#define COPY_FILE 10

void printString(char*);
int readString(char*);
void readSector(char* buffer, int sector);
void writeSector(char* buffer, int sector);
int mod(int a, int b);
int div(int a, int b);
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void readFile(int* fileName,int* buffer);
void printChar(char ch);
void prtInt(int i);
int strComp(int* buffer, char* fileName);
void executeProgram(char* name, int segment); 
void terminate();
void deleteFile(char* name);
void writeFile(int* name, int* buffer, int numberOfSectors);
void getDirectory();
void clear(char*,int);
void copyFile(char* src, char* dest);
int strcmp(char* a, char* b, int len);
int getFileDesc(char* filename, char* buffer);

main(){
	char buffer[13312];
    char buff[10000];

    makeInterrupt21();
    while(1){
    	interrupt(0x21, PRINT_STRING, "\rshell> ", 0, 0);
    	interrupt(0x21, READ_STRING, buffer, 0, 0);
    	if(buffer[0]=='t' && buffer[1]=='y' && buffer[2]=='p' && buffer[3]=='e' && buffer[4]==' '){
    		interrupt(0x21, READ_FILE, buffer+5, buff, 0);
    		interrupt(0x21, PRINT_STRING, buff, 0, 0);
    	} else if(buffer[0]=='e' && buffer[1]=='x' && buffer[2]=='e' && buffer[3]=='c' && buffer[4]=='u' && buffer[5]=='t' && buffer[6]=='e' && buffer[7]==' '){
    		interrupt(0x21, EXECUTE, buffer+8, 0x2000, 0);
    	} else if(buffer[0]=='d' && buffer[1]=='e' && buffer[2]=='l' && buffer[3]=='e' && buffer[4]=='t' && buffer[5]=='e' && buffer[6]==' '){
    		interrupt(0x21, DELETE_FILE, buffer+7, 0, 0);
    	} else if(buffer[0]=='d' && buffer[1]=='i' && buffer[2]=='r'){
    		interrupt(0x21, GET_DIR, 0, 0, 0);
    	} else if(buffer[0]=='c' && buffer[1]=='r' && buffer[2]=='e' && buffer[3]=='a' && buffer[4]=='t' && buffer[5]=='e' && buffer[6]==' '){
    		interrupt(0x21, WRITE_FILE, buffer+7, 0, 1);
    	} else if(buffer[0]=='c' && buffer[1]=='o' && buffer[2]=='p' && buffer[3]=='y' && buffer[4]==' '){
    		int i = 5;
			int j = i;
			while (buffer[j] != ' ' && buffer[j] != '\t')
				j++;
			buffer[j++] = 0;
			while (buffer[j] == ' ' || buffer[j] == '\t')
				j++;
    		interrupt(0x21, COPY_FILE, buffer+i, buffer+j, 0);
    	} else {
    		interrupt(0x21, PRINT_STRING, "Invalid command!\n\r", 0, 0);
    	}
    }
}

int strcmp(char* a, char* b, int len) {
	int i;
	for (i = 0; i < len; i++) {
		if (a[i] != b[i])
			return 0;
		if (a[i] == 0)
			break;
	}
	return 1;
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){	
        if (AX == 0) printString(BX);
        else if (AX ==1) readString(BX);
        else if(AX == 2) readSector(BX,30);
		else if (AX == 3) readFile(BX, CX);
		else if (AX == 4) executeProgram(BX,CX);
		else if (AX == 5) terminate();
		else if (AX == 6) writeSector(BX,CX);
		else if (AX == 7) deleteFile(BX);
		else if (AX == 8) writeFile(BX, CX, DX);
		else if (AX == 9) getDirectory();
		else if (AX == 10) copyFile(BX,CX);
		else printString("Invalid interrupt!\0");
}

void copyFile(char* src, char* dest) {
	char map[512],dir[512],free[512],content[10240],*cp;
	int i,j,found,fc,fpos;
	
	readSector(map, 1);
	readSector(dir, 2);
	
	fc = 0;
	for (i = 3; i < 512; i++)
		if (map[i] == 0)
			free[fc++] = i;
	
	if (fc == 0) {
		printString("Not enough space!\n\r\0");
		return;
	}
	
	for (fpos = 0; fpos < 16; fpos++) {
		found = 1;
		for (i = 0; i < 32 && found; i++)
			if (dir[fpos*32+i] != 0)
				found = 0;
		if (found)
			break;
	}
	if (!found) {
		printString("Maximum file number!\n\r\0");
		return;
	}
	if (strComp(dir, dest) > -1) {
		printString("File already exists!\n\r\0");
		return;
	}
	for (i = 0; i < 6; i++) {
		dir[fpos*32+i] = dest[i];
		if (dest[i] == 0)
			break;
	}
	
	readFile(src, content);
	for (i = 0; content[i] != 0; i++);
	
	j = 0;
	cp = content;
	while (i > 0) {
		writeSector(cp, free[j]);
		dir[fpos*32+6+j] = free[j];
		map[free[j]] = 0xff;
		
		cp += 512;
		j++;
		i-=512;
	}
	writeSector(map, 1);
	writeSector(dir, 2);
}

void executeProgram(char* name, int segment){ 
	int i;
	int address;
	char readingBuffer[13312];
	readFile(name, readingBuffer);
	for (i=0; i<13312;i++)
		putInMemory(segment,i,readingBuffer[i]);
	launchProgram(segment);
}

void getDirectory(){
	char buffer[512];
	char file[7];
	int i,j;
	
	file[6] = 0;
	readSector(buffer, 2);
	for (i = 0; i < 16; i++)
		if (buffer[i*32] != 0) {
			for (j = 0; j < 6; j++) {
				file[j] = buffer[i*32+j] != 0 ? buffer[32*i+j] : ' ';
			}
			printString(file);
			printString("\n\r\0");
		}
}

/* BX = address of character array holding the file name, CX = address of character array holding the file to be written, DX = number of sectors */
void writeFile(char* name,char* buffer, int numberOfSectors) {
	char map[512], directory[512], content[10240], subBuff[512];
	int directoryLine,j,k, index, diff;
	int nameLen, sectorNum;
	int iterator = 0;
	int foundFree = 0;
	int nameInts[7];
	int i,h;
	int kVal;

	readSector(map,1);
	readSector(directory,2);
	
	// mencari directory yang kosong
    for (directoryLine = 0; directoryLine < 16; directoryLine++){
		if (directory[32*directoryLine] == 0x00){
			foundFree = 1;
			break;
		}
	}
	if (foundFree == 0){
		printString("Didn't find empty location for file.");
		return;
	}

	if (strComp(directory, name) > -1) {
		printString("File already exists!\n\r\0");
		return;
	}

	// memasukan nama file ke 6bit pertama pada directory yang kosong
	nameLen = 0;
	while(name[nameLen] != 0x0) nameLen++;
	for (j=0;j<6;j++){
		directory[32*directoryLine+j] = (j<nameLen) ? name[j] : 0x0;
	}

	// membuat file
	for (k = 0; k < numberOfSectors; k++){

		// mencari sector yang kosong dalam map
		sectorNum = 0;
		while(map[sectorNum] != 0x0){
			sectorNum++;
		}
		if (sectorNum==26)
		{
			printString("Not enough space in directory entry for file\n");
			return;
		}
		// set 0xFF kedalam map

		map[sectorNum] = 0xFF;
		
		// tambahkan nomor sector kedalam directory
		directory[32*directoryLine+6+k] = sectorNum;

		i = 0;
		while ((j = readString(content+i)) > 0) {
			content[j+i] = '\n';
			content[j+i+1] = '\r';
			i += (j + 2);
		}
		content[i] = 0;

		writeSector(content,sectorNum);
	}
	// masukan map dan directory kedalam disk
	writeSector(map,1);
	writeSector(directory,2);
}

int getFileDesc(char* filename, char* buffer) {
	char dir[512];
	int i,j;
	
	readSector(dir, 2);
	for (i = 0; i < 16; i++)
		if (strcmp(dir+32*i,filename,6)) {
			for (j = 0; j < 32; j++)
				buffer[j] = dir[32*i+j];
			return i;
		}
	return -1;
}

void deleteFile(char* name){
	char map[512], directory[512];
	int i, j, k, fileFound, index;
	
	readSector(map,1);
	readSector(directory,2);

	fileFound = strComp(directory,name);

	if (fileFound){
		// set 6 bytes pertama menjadi 0x00
		for(i=0;i<6;i++){
			directory[fileFound*32+i] = 0x00;
		}
		// set 26 bytes selanjutnya menjadi 0x00 dan map
		for (j=0;j<26;j++){
			index = directory[fileFound*32+6+j];
			if(index != 0) map[index] = 0x00;
			directory[fileFound*32+6+j] = 0x00;
		}
	}
	else{
		printString("File not found!");
	}
	
	writeSector(map,1);
	writeSector(directory,2);
}

void readFile(char* fileName,char* buffer){ 
    int fileFound;
    int nameCt = 0;
	int index, k,h;
	int sectors[27];
	int j = 0;
	int i;
	int buffAddress = 0;
	

    /* Read in the directory sector */
    readSector(buffer, 2);  

    /* Try to find the file name */
	fileFound = strComp(buffer,fileName);

	if (fileFound!=0){

		/* 1.Using the sector numbers in the directory, load the file, sector by sector, into the buffer array.
			You should add 512 to the buffer address every time you call readSector*/
		/* Get the line that the file info is on from the buffer */
		index = fileFound*32+12;
		for (j=0;j<21;j++){
			sectors[j] = buffer[index+j];
			
		}

		sectors[21] = 0;
		k = 0;
		while(sectors[k]!=0x0){
			readSector(buffer+buffAddress,sectors[k]);
			buffAddress += 512;
			k++;
		}
		
	}
	else{
		printString("File Not Found!");
		return;
	}		

}
/* BX - address of char array, CX- sector number*/
void writeSector(char* buffer, int sector){
	
       /* relative sector = ( sector MOD 18 ) + 1*/
        int relSector = mod(sector,18) + 1;
        /*head = ( sector / 18 ) MOD 2		(this is integer division, so the result should be rounded down)*/
        int op = div(sector,18);
        int head = mod(op,2);
        /*track = ( sector / 36 )*/
        int track = div(sector,36);
        /* call interrupt 0x13 */
        interrupt(0x13,3*256+1,buffer,track*256+relSector,head*256+0);
}

void readSector(char* buffer, int sector){ 

       /* relative sector = ( sector MOD 18 ) + 1*/
        int relSector = mod(sector,18) + 1;
        /*head = ( sector / 18 ) MOD 2		(this is integer division, so the result should be rounded down)*/
        int op = div(sector,18);
        int head = mod(op,2);
        /*track = ( sector / 36 )*/
        int track = div(sector,36);
        /* call interrupt 0x13 */
        interrupt(0x13,2*256+1,buffer,track*256+relSector,head*256+0);


}

int strComp(char* buffer, char* fileName){ 
	int i, j;

	int checkFound = 0;


     for (i = 0; i < 16; i++)
 		{
		 /* If there is a file at the location */
		if (buffer[32*i] != 0x0){
			/* Check the first characters */
			for (j=0; j < 6; j++){
				/* This is a match */
				/*printString("Checking: ");
				printChar(buffer[j+32*i]);
				printString(" with ");
				printChar(fileName[j]);*/

				if (buffer[j+32*1] == 0x0 || buffer[j+32*1] == '\r' || buffer[j+32*1] == '\n' || fileName[j] == 0x0 || fileName[j] == '\r' || fileName[j] == '\n'){
					break;
				}
				else if (buffer[j+32*i] == fileName[j]){
					checkFound = 1;	
				}
				else {
					checkFound = 0;
					break;
				}
				
			}
		 	
			if (checkFound == 1){

				 return i;
			}
			else{
				/*printString("Next check");*/
			}
		}
	}
	return -1;
}

int mod(int a, int b){
    while(a >= b){
        a = a - b;
    }
    return a;
}

int div(int a, int b){
    int q = 0;
    while(q*b <=a){
        q = q+1;
    }
    return q-1;

}

int readString(char* buff){
    int dashn = 0xa;
    int endStr = 0x0;
    int enter = 0xd;
    int backsp = 0x8;
    int dashr = 0xd;
    int loop = 1;
    int count = 0;
    buff[0] = dashr;
    buff[1] = dashn;
    while(loop){
            /* Call interrupt 0x16 */
            /* interrupt #, AX, BX, CX, DX */
            int ascii = interrupt(0x16,0,0,0,0);
            if (ascii == enter){              
				buff[count] = 0x0;
                buff[count+1] = dashr;
                interrupt(0x10, 0xe*256+buff[count+1], 0, 0, 0);
                buff[count+2] = dashn;
                interrupt(0x10, 0xe*256+buff[count+2], 0, 0, 0);
                return count;
            }
            else if (ascii == backsp){
                if (count > 0){
                    buff[count] = 0x0;
                    count--;
                    interrupt(0x10,0xe*256+0x8,0,0,0);
                    count++;
                    interrupt(0x10,0xe*256+0x0,0,0,0);
                    count--;
                    interrupt(0x10,0xe*256+0x8,0,0,0);
                    
               }
            }
            else{
                buff[count] = ascii;
                interrupt(0x10, 0xe*256+ascii, 0, 0, 0);
                count++;
            }     
    }
    return 0;
}
    
void printString(char* chars){

   
    int i = 0;
    while(chars[i] != '\0'){
            int ch = chars[i];
            /* interrupt #, AX, BX, CX, DX */
		        interrupt(0x10, 0xe*256+ch, 0, 0, 0);
            i++;
    }


}
void clear(char* buff, int len){
	int i;
	for(i=0;i<len;i++){
		buff[i] = 0x0;
	}
}
void printChar(char ch){
	char* chars[2];
	chars[0] = ch;
	chars[1] = '\0';
	printString(chars);
}
void prtInt(int i){
	int* chars[2];
	chars[0] = i;
	chars[1] = '\0';
	printString(chars);
	
}