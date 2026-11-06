#include <unistd.h>
int main(void){execl("/bin/sh","sh","-lc","exec coma",NULL);}
