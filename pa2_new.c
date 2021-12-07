#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	
	//validating user input
	if(argc <= 1){
		printf("Missing filename\n");
		exit(0);
	}
	if(argc > 2){
		printf("Too many parameters\n");
	}

	//opening file
	int fd = open(argv[1], O_WRONLY); //returns index into file descriptors table
	if(fd < 0){
		printf("Could not open file\n");
		exit(0);
	}


	//loop until user exits or aborts program
	while(1){
		char user_option;
		printf("Option (r for read, w for write, s for seek):");
		user_option = getchar();
		
		if(user_option > 0){ // will return null on EOF or Error, Ctrl+d sends EOF	
			//read from file
			if(user_option  == 'r'){
				int num_bytes;
				printf("Enter the number of bytes you want to read: ");
				scanf("%d", &num_bytes);
				printf("\n");

				//inserting characters into buffer to read from (includes \n)
				char* buff = (char*)malloc(num_bytes + 1);
				if(read(fd, buff, num_bytes + 1) >= 0){
					if(write(1, buff, num_bytes+1) < 0){
						printf("Error displaying on output\n");
					}
				}
				else{
					printf("Could not read from file\n");
				}			
				free(buff);

			}
			/*
			//write string to file
			else if(user_option == 'w'){

				//max number of chars is 256
				char str_to_write[256];
				printf("Enter the string you want to write (max 256 chars, input will be cut off): ");
				fgets(str_to_write, 256, stdin);

				//read until null terminator
				if(fgets(str_to_write, 256, stdin) == NULL){
					printf("Could not read input string\n");
				}
				else{
					str_to_write[strlen(str_to_write) - 1] = '\0';
				}
				//write str_to_write to file
				fputs(str_to_write, fp);
					
			}

			//seek in file
			else if(user_option == 's'){
				long int offset_val;
				printf("Enter an offset value: ");
				scanf("%ld", &offset_val);

				printf("\n");
				int whence_val;
				printf("Enter a value for whence (O for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END): ");
				scanf("%d", &whence_val);

				//seeks
				if(fseek(fp, offset_val, whence_val) != 0){
					printf("Could not seek in the file\n");
				}
		
			}
			*/
		}
		
		else{
			exit(0);
		}
	}


	close(fd);
	return 0;
}
