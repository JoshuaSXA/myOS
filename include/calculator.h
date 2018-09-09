#include "stdio.h"
char operand[2];
char optr;
char answer[2];

PUBLIC void calculator(int fd_stdin, int fd_stdout){
	//clear screen
	clear();
	printf("Welcome to the calculator!\n");
	printf("You may input operand and basic operator to calculate. Inputting Q indicates quit the calculate.\n\n");
	printf("Notice: Only two operand and one operator once are acceptable.\n");
	printf("For example: You may enter in the following sequence to calculator 8+5*2\n\n");
	printf("|*****************************************************************************|\n");
	printf("|                                                                             |\n");
	printf("|                   5 -> * -> 2 -> enter -> + -> 8 -> enter                   |\n");
	printf("|                                                                             |\n");
	printf("|*****************************************************************************|\n");
	printf("\nYou get it,right?\n");
	printf("Please input the first operand:\n");
	read(fd_stdin,answer,2);
	while(1){
		clear();
                if(answer[0] == 'q'){
			break;
		}
		printf("%d ",answer[0]);
		read(fd_stdin,optr,2);
		if(optr=='E'){
			break;
		}
		clear();
		printf("%d %c",answer[0],optr);
		read(fd_stdin,operand,2);
		clear();
		if(optr=='+'){
			answer[0]+=operand[0];
		}else if(optr=='-'){
			answer[0]-=operand[0];
		}else if(optr=='*'){
			answer[0]*=operand[0];
		}else if(optr=='/'){
			answer[0]/=operand[0];
		}
	}
}
