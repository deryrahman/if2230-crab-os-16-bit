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
#define CREATE_DIR 11
#define PWD 12
#define CHANGE_DIR 13

void printString(char*);
int readString(char*);
void readSector(char* buffer, int sector);
void writeSector(char* buffer, int sector);
int mod(int a, int b);
int div(int a, int b);
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void readFile(int* fileName,int* buffer, char* wdir);
void printChar(char ch);
void prtInt(int i);
int getFileLoc(int* buffer, char* fileName);
void executeProgram(char* name, int segment); 
void terminate();
void deleteFile(char* name, char* wdir);
void writeFile(char* filename, char* wdir);
void getDirectory(char* wdir);
void clear(char*,int);
void copyFile(char* src, char* dest);
int strcmp(char* a, char* b, int len);
void createDir(char* dirname, char* wdir);
void getCurrDir(char* filename);
void changeDir(char* dirname, char* wdir);
int isDir(char* dirname);

main(){
	char buffer[13312];
    char buff[10000];
    char wdir[10];
    int i;

    for(i = 0; i < 10; i++) wdir[i] = 0;
    makeInterrupt21();
    while(1){
    	interrupt(0x21, PRINT_STRING, "\rshell> ", 0, 0);
    	interrupt(0x21, READ_STRING, buffer, 0, 0);
    	if(strcmp(buffer,"type ",5)){
    		interrupt(0x21, READ_FILE, buffer+5, buff, wdir);
    		interrupt(0x21, PRINT_STRING, buff, 0, 0);
    	} else if(strcmp(buffer,"execute ",7)){
    		interrupt(0x21, EXECUTE, buffer+8, 0x2000, 0);
    	} else if(strcmp(buffer,"delete ",7)){
    		interrupt(0x21, DELETE_FILE, buffer+7, 0, 0);
    	} else if(strcmp(buffer,"dir",3)){
    		interrupt(0x21, GET_DIR, wdir, 0, 0);
    	} else if(strcmp(buffer,"create ",7)){
    		interrupt(0x21, WRITE_FILE, buffer+7, wdir, 0);
    	} else if(strcmp(buffer,"copy ",5)){
    		int i = 5;
			int j = i;
			while (buffer[j] != ' ' && buffer[j] != '\t')
				j++;
			buffer[j++] = 0;
			while (buffer[j] == ' ' || buffer[j] == '\t')
				j++;
    		interrupt(0x21, COPY_FILE, buffer+i, buffer+j, 0);
    	} else if (strcmp(buffer,"mkdir ",6)){
    		interrupt(0x21, CREATE_DIR, buffer+6, wdir, 0);
    	} else if (strcmp(buffer,"pwd",3)){
    		interrupt(0x21, PWD, wdir, 0, 0);
    	} else if (strcmp(buffer,"cd ",3)){
    		interrupt(0x21, CHANGE_DIR, buffer+3, wdir, 0);
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
		else if (AX == 3) readFile(BX, CX, DX);
		else if (AX == 4) executeProgram(BX,CX);
		else if (AX == 5) terminate();
		else if (AX == 6) writeSector(BX,CX);
		else if (AX == 7) deleteFile(BX);
		else if (AX == 8) writeFile(BX, CX);
		else if (AX == 9) getDirectory(BX);
		else if (AX == 10) copyFile(BX,CX);
		else if (AX == 11) createDir(BX,CX);
		else if (AX == 12) getCurrDir(BX);
		else if (AX == 13) changeDir(BX,CX);
		else printString("Invalid interrupt!\0");
}

int isDir(char* dirname){
	int i;
	char buffer[32];
	getFileLoc(buffer, dirname);
	for (i = 12; i < 32; i++)
		if (buffer[i] != 0)
			return 0;
	return 1;
}

void changeDir(char* dirname, char* wdir){
	char buffer[32];
	int i;
	
	buffer[0] = 0;
	if(strcmp(dirname,"..",2)){
		getFileLoc(buffer, wdir);
		for (i = 0; i < 6; i++)
			wdir[i] = buffer[6+i];
		return;
	} else {
		getFileLoc(buffer, dirname);
		if(isDir(dirname) == 0){
			printString("It's file!\n\r\0");
			return;
		}
		if (buffer[0] == 0 || strcmp(buffer+6, wdir, 6) == 0) {
			printString("Folder not exists!\n\r\0");
			return;
		}
		for (i = 0; i < 6; i++)
			wdir[i] = dirname[i];
	}
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
	if (getFileLoc(dir, dest) > -1) {
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

void getCurrDir(char* filename){
	char temp[16][7];
	char buffer[32];
	int i, j;
	i = 0;
	while(filename[0]!=0){
		getFileLoc(buffer,filename);
		for(j = 0; j < 6; j++){
			temp[i][j] = buffer[j];
			filename[j] = buffer[6+j];
		}
		temp[i][7] = 0;
		i++;
	}
	printString("/\0");
	while(i--){
		printString(temp[i]);
	}
	printString("\n\r\0");
}

void getDirectory(char* wdir){
	char buffer[512];
	char name[7];
	int i,j,isDir;

	name[6] = 0;
	readSector(buffer, 2);
	for (i = 0; i < 16; i++)
		if (buffer[i*32] != 0 && strcmp(buffer+32*i+6,wdir,6) != 0) {
			for (j = 0; j < 6; j++) {
				name[j] = buffer[i*32+j] != 0 ? buffer[32*i+j] : ' ';
			}

			isDir = 1;
			for (j = 12; j < 32 && isDir; j++)
				if (buffer[32*i+j] != 0) isDir = 0;
			if(isDir){
				printString("d ");
			} else {
				printString("f ");
			}
			printString(name);
			printString("\n\r\0");
		}
}

/* BX = address of character array holding the file name, CX = address of character array holding the file to be written, DX = number of sectors */
void writeFile(char* filename,char* wdir) {
	char map[512],dir[512],free[512],content[10240],*cp;
	int i,j,found,fcek,directoryLine;
	
	readSector(map, 1);
	readSector(dir, 2);
	
	fcek = 0;
	for (i = 3; i < 512; i++)
		if (map[i] == 0)
			free[fcek++] = i;
	
	if (fcek == 0) {
		printString("Not enough space!\n\r\0");
		return;
	}
	
	for (directoryLine = 0; directoryLine < 16; directoryLine++) {
		found = 1;
		for (i = 0; i < 32 && found; i++)
			if (dir[directoryLine*32+i] != 0) found = 0;
		if (found) break;
	}
	if (!found) {
		printString("Didn't find empty location for file.\n\r\0");
		return;
	}
	if (getFileLoc(dir, filename) > -1) {
		printString("File already exists!\n\r\0");
		return;
	}
	
	for (i = 0; i < 6; i++)
		dir[directoryLine*32+i] = filename[i];
	for (i = 6; i < 12; i++)
		dir[directoryLine*32+i] = wdir[i-6];
	
	i = 0;
	while ((j = readString(content+i)) > 0) {
		content[j+i] = '\n';
		content[j+i+1] = '\r';
		i += j + 2;
	}
	content[i] = 0;
	
	j = 0;
	cp = content;
	while (i > 0) {
		writeSector(cp, free[j]);
		dir[directoryLine*32+12+j] = free[j];
		map[free[j]] = 0xff;
		
		cp += 512;
		j++;
		i-=512;
	}
	writeSector(map, 1);
	writeSector(dir, 2);
}

void createDir(char* dirname, char* wdir){
	char directory[512];
	int directoryLine, found, i;
	readSector(directory,2);
	
	// mencari directory yang kosong
    for (directoryLine = 0; directoryLine < 16; directoryLine++){
    	found = 1;
    	for(i = 0; i < 32 && found; i++){
    		if(directory[directoryLine*32+i] != 0) found = 0;
    	}
    	if(found) break;
	}
	if (!found){
		printString("Didn't find empty location for directory.");
		return;
	}
	for (i = 0; i < 6; i++) {
		directory[directoryLine*32+i] = dirname[i];
		if (dirname[i] == 0) break;
	}
	for (i = 6; i < 12; i++) {
		directory[directoryLine*32+i] = wdir[i];
		if (wdir[i] == 0) break;
	}
	for (i = 12; i < 32; i++) directory[directoryLine*32+i] = 0;

	writeSector(directory,2);
}


void deleteFile(char* name, char* wdir){
	char map[512], directory[512];
	int i, j, k, fileFound, index;
	
	readSector(map,1);
	readSector(directory,2);

	fileFound = getFileLoc(directory,name);

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

void readFile(char* fileName,char* buffer, char* wdir){ 
	int fileFound;
	int nameCt = 0;
	int index, k,h;
	int sectors[21];
	int j = 0;
	int i;
	int buffAddress = 0;

	readSector(buffer, 2);
	fileFound = getFileLoc(buffer, fileName);
	if (fileFound >= 0 && strcmp(buffer + 6, wdir,6)){
		index = fileFound*32+12;
		for (j=0;j<20;j++)
			sectors[j] = buffer[index+j];

		sectors[20] = 0;
		k = 0;
		while(sectors[k]!=0x0) {
			readSector(buffer+buffAddress,sectors[k]);
			buffAddress += 512;
			k++;
		}
	} else {
		printString("File Not Found!\n\r\0");
		buffer[0] = 0;
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

int getFileLoc(char* buffer, char* filename){ 
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