void terminate();

int main(){
	interrupt(0x21, 0, "Hello World!\n\r\0", 0, 0);
	terminate();
}