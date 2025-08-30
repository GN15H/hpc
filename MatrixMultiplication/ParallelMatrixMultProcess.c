#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    pid_t pid;
    int pito = 6;
    pid = fork();
		if(pid){
            sleep(0.5);
			printf("Papito pa %i\n", pito);
		}else{
            sleep(0.5);
			printf("Hijito %i\n", pito);
		}
    return 0;
}
