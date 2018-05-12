#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#define ARG_SIZE 20
#define ARGS_NUMBER 20
#define PATHS_NUMBER 20

void cleanTable(char* table[], int elementsNumber)
{
	int i;
	for(i = 0; i < elementsNumber; i++)
        table[i] = NULL;
}

void getEnvPath(char* paths[])
{
	char* allPaths = getenv("PATH");
	char* path = strtok(allPaths,":");
	int i = 0;
	cleanTable(paths,PATHS_NUMBER);
	while(path != NULL && i < PATHS_NUMBER - 1)
	{
    	paths[i] = path;
    	i++;
    	path = strtok(NULL,":");
	}
}

int main(int argc, char* argv[])
{   	
   	char* paths[PATHS_NUMBER];
   	getEnvPath(paths);
	
	if(argc < 4)
	{
		printf("You need to enter name of file with commands, available processor time and memory size.\n");
		return 0;
	}
	

	FILE* file  = fopen(argv[1],"r");
    if(file == NULL)
        return 0;
     
    //---------------resources
    int processorTime = (int)strtol(argv[2],NULL,10);
    int memorySize = (int)strtol(argv[3],NULL,10) * 1024 * 1024;
     	
    struct rusage currentUsage;
	struct rusage previousUsage;
     	
    previousUsage.ru_utime.tv_sec = 0;
	previousUsage.ru_utime.tv_usec = 0;
	previousUsage.ru_stime.tv_sec = 0;
	previousUsage.ru_stime.tv_usec = 0;
   //---------------resources	
   
	char* args[ARGS_NUMBER];
    size_t bufferSize = 80;
    char *buffer = malloc(bufferSize * sizeof(char));
    int lineNumber = 0;
    
    while(-1 != getline(&buffer, &bufferSize, file))
    {
    	printf("\n%d: %s", ++lineNumber, buffer);
    	char* separatedData = strtok(buffer,"\t \n");
    	if(separatedData == NULL) continue;
    	
    	cleanTable(args,ARGS_NUMBER);
                
    	int i = 0;	
    	while(separatedData != NULL && i < ARGS_NUMBER - 1)
    	{
    		args[i] = separatedData;
    		i++;
    		separatedData = strtok(NULL,"\t \n");
    	}
    	
    	int res;
    	pid_t pid;
    	pid = fork();
    	if(pid == 0) //proces dziecka
    	{
    		
    		//---------------resources
    		struct rlimit limit;
    		limit.rlim_cur = memorySize;
			limit.rlim_max = memorySize;
    		setrlimit(RLIMIT_AS,&limit); 
    		
    		limit.rlim_cur = processorTime;
			limit.rlim_max = processorTime;
    		setrlimit(RLIMIT_CPU,&limit);
    		//---------------resources
    		
    		int wyn;
    		wyn = execve(args[0], args,paths);
    		if(wyn == -1)
    		{
    			printf("Error occured, trying to execute command in line %d: ",lineNumber);
					int j = 0;
					while(args[j] != NULL)
					{
						printf("%s ",args[j]);
						j++;
					}
					printf("\n");
					return(-1);
    		}
    	}
    	else
    	{
    		wait(&res);
			if(WEXITSTATUS(res) != 0) return -1;
			
			//---------------resources
			getrusage(RUSAGE_CHILDREN,&currentUsage);
			
			int j = 0;
			while(args[j] != NULL)
			{
				printf("%s ",args[j]);
				j++;
			}
			printf("\n");
			
			printf("user\t%ds %dus\nsys\t%ds %dus\n",
				(int)(currentUsage.ru_utime.tv_sec - previousUsage.ru_utime.tv_sec),
				(int)(currentUsage.ru_utime.tv_usec - previousUsage.ru_utime.tv_usec),
				(int)(currentUsage.ru_stime.tv_sec - previousUsage.ru_stime.tv_sec),
				(int)(currentUsage.ru_stime.tv_usec - previousUsage.ru_stime.tv_usec));

			previousUsage = currentUsage;
			//---------------resources
    	}	
    }

	fclose(file);
	return 0;
}
