/*
   Author   : Dery (13515097) | Yano (13515100) | Farhan (13515043)
   Date     : 21/02/2017
*/

#define VIDEO_SEGMENT 0xB000
#define BASE  0x8000
#define SCREEN_SIZE 25*80*2

#define BLACK           0x00
#define BLUE            0x01
#define GREEN           0x02
#define CYAN            0x03
#define RED             0x04
#define MAGENTA         0x05
#define BROWN           0x06
#define LIGHT_GRAY      0x07
#define LIGHT_BLACK     0x08
#define LIGHT_BLUE      0x09
#define LIGHT_GREEN     0x0A
#define LIGHT_CYAN      0x0B
#define LIGHT_RED       0x0C
#define LIGHT_MAGENTA   0x0D
#define YELLOW          0x0E
#define WHITE           0x0F

void clearScreen(int bg);
void printHelloWorld(int row, int col, int color, int bg);
void printString(int row, int col, int color, int bg, char* s);
void printStringCenter(int row, int ln, int color, int bg, char* s);
void printLogo(int color, int bg);
void delay(unsigned long time );

int main() {
   clearScreen(CYAN);
   //printHelloWorld(12, 34, WHITE, BLUE);
   //printString(12, 27, WHITE, CYAN, "Lorem ipsum dolor sit amet");
   printLogo(WHITE, CYAN);
   while(1);
   return 0;
}

void clearScreen(int bg){
   int i = 0;

   while (i < SCREEN_SIZE){
      putInMemory(VIDEO_SEGMENT, BASE + i*2, '\0');
      putInMemory(VIDEO_SEGMENT, BASE + i*2 + 1, bg<<4);
      i++;
   }
}
void printHelloWorld(int row, int col, int color, int bg){
   int offset=BASE+((row-1)*80*2)+col*2;
   putInMemory(VIDEO_SEGMENT, offset + 0, 'H');
   putInMemory(VIDEO_SEGMENT, offset + 1, color + (bg<<4));
   putInMemory(VIDEO_SEGMENT, offset + 2, 'e');
   putInMemory(VIDEO_SEGMENT, offset + 3, color + (bg<<4));
   putInMemory(VIDEO_SEGMENT, offset + 4, 'l');
   putInMemory(VIDEO_SEGMENT, offset + 5, color + (bg<<4));
   putInMemory(VIDEO_SEGMENT, offset + 6, 'l');
   putInMemory(VIDEO_SEGMENT, offset + 7, color + (bg<<4));
   putInMemory(VIDEO_SEGMENT, offset + 8, 'o');
   putInMemory(VIDEO_SEGMENT, offset + 9, color + (bg<<4));
   putInMemory(VIDEO_SEGMENT, offset + 10, ' ');
   putInMemory(VIDEO_SEGMENT, offset + 11, color + (bg<<4));
   putInMemory(VIDEO_SEGMENT, offset + 12, 'W');
   putInMemory(VIDEO_SEGMENT, offset + 13, color + (bg<<4));
   putInMemory(VIDEO_SEGMENT, offset + 14, 'o');
   putInMemory(VIDEO_SEGMENT, offset + 15, color + (bg<<4));
   putInMemory(VIDEO_SEGMENT, offset + 16, 'r');
   putInMemory(VIDEO_SEGMENT, offset + 17, color + (bg<<4));
   putInMemory(VIDEO_SEGMENT, offset + 18, 'l');
   putInMemory(VIDEO_SEGMENT, offset + 19, color + (bg<<4));
   putInMemory(VIDEO_SEGMENT, offset + 20, 'd');
   putInMemory(VIDEO_SEGMENT, offset + 21, color + (bg<<4));
   putInMemory(VIDEO_SEGMENT, offset + 22, '!');
   putInMemory(VIDEO_SEGMENT, offset + 23, color + (bg<<4));
}
void printString(int row, int col, int color, int bg, char* s){
   int i=0;
   int offset=BASE+((row-1)*80*2)+col*2;
   while(s[i]!='\0'){
      putInMemory(VIDEO_SEGMENT, offset + i*2, s[i]);
      putInMemory(VIDEO_SEGMENT, offset + i*2+1, color + (bg<<4));
      i++;
   }
}
void printStringCenter(int row, int ln, int color, int bg, char* s){
   int i=0;
   int offset=BASE+((row-1)*80*2)+(40-ln/2)*2;
   while(s[i]!='\0'){
      putInMemory(VIDEO_SEGMENT, offset + i*2, s[i]);
      putInMemory(VIDEO_SEGMENT, offset + i*2+1, color + (bg<<4));
      i++;
   }
}
void printLogo(int color, int bg){
   printStringCenter(7, 20, color, bg,"    __       __");
   printStringCenter(8, 20, color, bg,"   / <`     '> \\");
   printStringCenter(9, 20, color, bg,"  (  / @   @ \\  )");
   printStringCenter(10, 20, color, bg,"   \\(_ _\\_/_ _)/");
   printStringCenter(11, 20, color, bg," (\\ `-/     \\-' /)");
   printStringCenter(12, 20, color, bg,"  \"===\\     /===\"");
   printStringCenter(13, 20, color, bg,"   .==')___(`==.");
   printStringCenter(14, 20, color, bg,"  ' .='     `=.");
   printStringCenter(15, 20, color, bg,"       CrabOS");
   printStringCenter(16, 20, color, bg,"Dery | Yano | Farhan");
}