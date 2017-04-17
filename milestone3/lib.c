void printString(char* chars){
    int i = 0;
    while(chars[i] != '\0'){
            int ch = chars[i];
		        interrupt(0x10, 0xe*256+ch, 0, 0, 0);
            i++;
    }
}

int div(int a, int b){
    int q = 0;
    while(q*b <=a){
        q = q+1;
    }
    return q-1;
}