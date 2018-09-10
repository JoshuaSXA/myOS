/********************************************
 * @file   finger_guessing.h
 * @brief  finger-guessing game
 * @author Shen Xiaoang
 *******************************************/

#include "stdio.h"

PUBLIC void fingerGuessingGame(int fd_stdin, int fd_stdout){
    char gamer[2];
    int computer;
    int result;
    clear();
    while (1){
        printf ("=========================================================\n");
        printf ("This is a finger-guessing game. Please make your choice:\n");
        printf ("A:Scissor\nB:Stone\nC:Cloth\nD:Quit\n");
        read(fd_stdin, gamer, 2);
        switch (gamer[0]){
            case 65:
            case 97:
                gamer[0] = 4;
                break;
            case 66:
            case 98:
                gamer[0] = 7;
                break;
            case 67:
            case 99:
                gamer[0] = 10;
                break;
            case 68:
            case 100:
		clear();
		return;
            default:
                printf ("You have made a wrong choice!\n");
                continue;
        }
        
        computer = rand2048() % 3;          //The computer's choice    
        result = (int)gamer[0]+computer;    
        printf ("The computer's choice is: ");
        switch (computer){
            case 0:printf ("Scissor\n");break;
            case 1:printf ("Stone\n");break;
            case 2:printf ("Cloth\n");break;
        }
        printf ("Your choice is: ");
        switch (gamer[0]){
            case 4:printf ("Scissor\n");break;
            case 7:printf ("Stone\n");break;
            case 10:printf ("Cloth\n");break;
        }
        if (result==6||result==7||result==11){
            printf ("You win!\n\n");
        }
        else if (result==5||result==9||result==10){
            printf ("You lose!\n\n");
        }
        else{
            printf ("Draw!\n\n");
        }
    }
    clear();
    return;
}
