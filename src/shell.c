#include "common.h"
#include "monitor.h"
#include "ds.h"

char STDOUT[500]="";
char STDIN[500]="";
extern struct kernel_queue stdin_queue;


void duplicate_shell() {
    boot_message("Starting duplicate shell...\n");
    monitor_clear();
    print_welcome();

    int stdin_len=0;
    char ch;
    
    boot_message("System booted...\n");
    monitor_clear();
    print_welcome();

    while(1) {

        monitor_write("\nCoc $ ");

        do {
            while(stdin_queue.is_kq_empty(&stdin_queue));
            ch = (stdin_queue.kq_pop(&stdin_queue));
            STDIN[stdin_len++] = ch;
        }while(ch != 10);

        STDIN[--stdin_len] = '\0';

        if(!strcmp(STDIN, "help")) {   
            monitor_write("help\nauthor\nversion");
        } else if(!strcmp(STDIN, "author")) {   
            monitor_write("Dcalsky & nemoremold");
        } else if(!strcmp(STDIN, "version")) {   
            monitor_write("0.2.1");
        } else if(!strcmp(STDIN, "cat file1.txt")) {
            monitor_write("file1");
        } else if(!strcmp(STDIN, "cat file2.txt")) {
            monitor_write("file2");
        } else if(strstr(STDIN, "cat") != -1) {
            monitor_write("Look for who?");
        } else {
            monitor_write("Invalid Command.");
        }
/*
        switch(strlen(STDIN))
        {
            case 6 : monitor_write("Vishnu KS"); break;
            case 4 : monitor_write("help\nauthor\nversion"); break;
            case 7 : monitor_write("Not named version!!!!");break;

            default : monitor_write("Invalid Command!!!");
        }*/



        strcpy("", STDIN);
        stdin_len=0;


    }




}
