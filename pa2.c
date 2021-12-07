#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	FILE* fp;
	int curr_file_position = 0;
	
	fp = fopen(argv[1], "r+"); //opening file for r/w
	if(fp == NULL){
		printf("File could not be opened\n");
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
				int i = 0;
				while(i < num_bytes){
					char c = fgetc(fp);
					if(c == EOF){
						break;
					}
					*(buff+i) = c;
					i = i+1;
				}

				//buffer needs terminating character 
				*(buff+num_bytes) = '\0';
				printf("%s\n", buff);
				free(buff);

			}

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
		}

		else{
			exit(0);
		}
	}


	fclose(fp);
	return 0;
}
