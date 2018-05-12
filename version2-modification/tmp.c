#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	if(argc < 3) 
	{
		printf("\nWrong arguments\n");
		return 0;
	}
	printf("\n\nHello\n");
	printf("%s\n",argv[0]);
	printf("%s\n",argv[1]);
	printf("%s\n",argv[2]);
	
	//char *buffer = malloc(10000 * sizeof(char));
	
	while(1)
	{
		printf("\nk");
	}
	return 0;
}
