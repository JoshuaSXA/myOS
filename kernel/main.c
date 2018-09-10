
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "keyboard.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

#include "calculator.h"
#include "2048Game.h"
#include "finger_guessing.h"
#include "minesweeper.h"
#include "snake.h"


/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main() {
    disp_str("-----\"kernel_main\" begins-----\n");
    struct task* p_task;
    struct proc* p_proc= proc_table;
    char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
    u16   selector_ldt = SELECTOR_LDT_FIRST;
    u8    privilege;
    u8    rpl;
    int   eflags;
    int   i, j;
    int   prio;

    // start the process
    for (i = 0; i < NR_TASKS+NR_PROCS; i++) {
        if (i < NR_TASKS) {
            /* task */
            p_task    = task_table + i;
            privilege = PRIVILEGE_TASK;
            rpl       = RPL_TASK;
            eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1   1 0010 0000 0010(2)*/
            prio      = 20;     //set the priority to 15
        }
        else {
            /* user process */
            p_task    = user_proc_table + (i - NR_TASKS);
            privilege = PRIVILEGE_USER;
            rpl       = RPL_USER;
            eflags    = 0x202; /* IF=1, bit 2 is always 1              0010 0000 0010(2)*/
            prio      = 10;     //set the priority to 5
        }

        strcpy(p_proc->name, p_task->name); /* set prio name */
        p_proc->pid = i;            /* set pid */

        p_proc->run_count = 0;

        p_proc->ldt_sel = selector_ldt;

        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
               sizeof(struct descriptor));
        p_proc->ldts[0].attr1 = DA_C | privilege << 5;
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
               sizeof(struct descriptor));
        p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
        p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

        p_proc->regs.eip = (u32)p_task->initial_eip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = eflags;

        p_proc->p_flags = 0;
        p_proc->p_msg = 0;
        p_proc->p_recvfrom = NO_TASK;
        p_proc->p_sendto = NO_TASK;
        p_proc->has_int_msg = 0;
        p_proc->q_sending = 0;
        p_proc->next_sending = 0;

        for (j = 0; j < NR_FILES; j++)
            p_proc->filp[j] = 0;

        p_proc->ticks = p_proc->priority = prio;
        p_proc->run_state = 1;
        p_task_stack -= p_task->stacksize;
        p_proc++;
        p_task++;
        selector_ldt += 1 << 3;
    }

    proc_table[4].run_state = 0;
    proc_table[5].run_state = 0;

    // init process
    k_reenter = 0;
    ticks = 0;

    p_proc_ready = proc_table;

    init_clock();
    init_keyboard();

    restart();

    while(1){}
}


/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
    MESSAGE msg;
    reset_msg(&msg);
    msg.type = GET_TICKS;
    send_recv(BOTH, TASK_SYS, &msg);
    return msg.RETVAL;
}

PUBLIC void addTwoString(char *to_str,char *from_str1,char *from_str2){
    int i=0,j=0;
    while(from_str1[i]!=0)
        to_str[j++]=from_str1[i++];
    i=0;
    while(from_str2[i]!=0)
        to_str[j++]=from_str2[i++];
    to_str[j]=0;
}


char users[3][128] = {"root", "admin","chen"};
char passwords[3][128]= {"root","super","super"};

void shell(char *tty_name){
    int fd;
    //int isLogin = 0;
    char rdbuf[512];
    char cmd[512];
    char arg1[512];
    char arg2[512];
    char buf[1024];
    char temp[512];

    int fd_stdin  = open(tty_name, O_RDWR);
    assert(fd_stdin  == 0);
    int fd_stdout = open(tty_name, O_RDWR);
    assert(fd_stdout == 1);
    animation();  // the start animation
    char current_dirr[512] = "/";
    
    while(1){
        printf("please login first)\n");
        clearArr(rdbuf, 512);
        clearArr(cmd, 512);
        clearArr(arg1, 512);
        clearArr(arg2, 512);
        clearArr(buf, 1024);
        clearArr(temp, 512);

        printf("username: ");
        int r = read(fd_stdin, rdbuf, 512);
        if (strcmp(rdbuf, "") == 0)
            continue;
        int i = 0;
        int j = 0;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0){
            cmd[i] = rdbuf[i];
            i++;
        }
        i++;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0){
            arg1[j] = rdbuf[i];
            i++;
            j++;
        }
        i++;
        j = 0;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0){
            arg2[j] = rdbuf[i];
            i++;
            j++;
        }
        // clear
        rdbuf[r] = 0;
        char old_cmd[512];
        strcpy(old_cmd,cmd);
        int cnt = 0,flag = 0;
        for(cnt = 0; cnt < 3; cnt++){
            if(strcmp(old_cmd, users[cnt])==0){
                printf("password: ");
                clearArr(rdbuf, 512);
                clearArr(cmd, 512);
                clearArr(arg1, 512);
                clearArr(arg2, 512);
                clearArr(buf, 1024);
                clearArr(temp, 512);
                int r = read(fd_stdin, rdbuf, 512);
                if (strcmp(rdbuf, "") == 0)
                    continue;
                int i = 0;
                int j = 0;
                while(rdbuf[i] != ' ' && rdbuf[i] != 0){
                    cmd[i] = rdbuf[i];
                    i++;
                }
                i++;
                while(rdbuf[i] != ' ' && rdbuf[i] != 0){
                    arg1[j] = rdbuf[i];
                    i++;
                    j++;
                }
                i++;
                j = 0;
                while(rdbuf[i] != ' ' && rdbuf[i] != 0){
                    arg2[j] = rdbuf[i];
                    i++;
                    j++;
                }
                // clear
                rdbuf[r] = 0;
                if (strcmp(cmd, passwords[cnt]) == 0){
                    flag = 1;
                }
                else{
                    printf("invalid password!\n");
                }
            }
        }
        if(flag == 0){
            printf("no valid username!\n");
        }
        else{
            printf("login sucessfully~\n");
            break;
        }
    }

    while (1) {
        // clear the array ！
        clearArr(rdbuf, 512);
        clearArr(cmd, 512);
        clearArr(arg1, 512);
        clearArr(arg2, 512);
        clearArr(buf, 1024);
        clearArr(temp, 512);

        printf("root@user:~%s$ ", current_dirr);
        int r = read(fd_stdin, rdbuf, 512);
        if (strcmp(rdbuf, "") == 0)
            continue;
        int i = 0;
        int j = 0;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0){
            cmd[i] = rdbuf[i];
            i++;
        }
        i++;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0){
            arg1[j] = rdbuf[i];
            i++;
            j++;
        }
        i++;
        j = 0;
        while(rdbuf[i] != ' ' && rdbuf[i] != 0){
            arg2[j] = rdbuf[i];
            i++;
            j++;
        }
        // clear
        rdbuf[r] = 0;

        if (strcmp(cmd, "process") == 0){
            ProcessManage();
        }
        else if (strcmp(cmd, "help") == 0){
            help();
        }      
        else if (strcmp(cmd, "clear") == 0){
            clear(); 
        }
        else if (strcmp(cmd, "ls") == 0) {
            ls(current_dirr);
        }
        else if (strcmp(cmd, "about") == 0) {
            printAbout();
        }
        else if(strcmp(rdbuf,"pause 4") == 0 ){
            proc_table[4].run_state = 0 ;
            ProcessManage();
        }
        else if(strcmp(rdbuf,"pause 5") == 0 ){
            proc_table[5].run_state = 0 ;
            ProcessManage();
        }
        else if(strcmp(rdbuf,"pause 6") == 0 ){
            proc_table[6].run_state = 0 ;
            ProcessManage();
        }
        else if(strcmp(rdbuf,"kill 4") == 0 ){
            proc_table[4].p_flags = 1;
            ProcessManage();
            printf("cant kill this process!");
        }
        else if(strcmp(rdbuf,"kill 5") == 0 ){
            proc_table[5].p_flags = 1;
            ProcessManage();
        }
        else if(strcmp(rdbuf,"kill 6") == 0 ){
            proc_table[6].p_flags = 1;
            ProcessManage();
        }
        else if(strcmp(rdbuf,"resume 4") == 0 ){
            proc_table[4].run_state = 1 ;
            ProcessManage();
        }
        else if(strcmp(rdbuf,"resume 5") == 0 ){
            proc_table[5].run_state = 1 ;
            ProcessManage();
        }
        else if(strcmp(rdbuf,"resume 6") == 0 ){
            proc_table[6].run_state = 1 ;
            ProcessManage();
        }
        else if(strcmp(rdbuf,"up 4") == 0 ){
            proc_table[4].priority = proc_table[4].priority*2;
            ProcessManage();
        }
        else if(strcmp(rdbuf,"up 5") == 0 ){
            proc_table[5].priority = proc_table[5].priority*2;
            ProcessManage();
        }
        else if(strcmp(rdbuf,"up 6") == 0 ){
            proc_table[6].priority = proc_table[6].priority*2;
            ProcessManage();
        }
        else if (strcmp(cmd, "touch") == 0){
            if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }

            fd = open(arg1, O_CREAT | O_RDWR);
            if (fd == -1){
                printf("Failed to create file! Please check the filename!\n");
                continue ;
            }
            write(fd, buf, 1);
            printf("File created: %s (fd %d)\n", arg1, fd);
            close(fd);
        }
        else if (strcmp(cmd, "cat") == 0){
            if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }

            fd = open(arg1, O_RDWR);
            if (fd == -1){
                printf("Failed to open file! Please check the filename!\n");
                continue ;
            }
            if (!verifyFilePass(arg1, fd_stdin)){
                printf("Authorization failed\n");
                continue;
            }
            read(fd, buf, 1024);
            close(fd);
            printf("%s\n", buf);
        }
        else if (strcmp(cmd, "vi") == 0){
            if(arg1[0]==0){
                printf("Failed to open file! Please input the filename!\n");
                continue;
            }
            if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }

            fd = open(arg1, O_RDWR);
            if (fd == -1){
                printf("Please check the filename!\n");
                continue ;
            }
            if (!verifyFilePass(arg1, fd_stdin)){
                printf("Authorization failed\n");
                continue;
            }
            int tail = read(fd_stdin, rdbuf, 512);
            rdbuf[tail] = 0;

            write(fd, rdbuf, tail+1);
            close(fd);
        }
        else if (strcmp(cmd, "rm") == 0) {
            if ((strcmp(arg1, "-r") == 0) || (strcmp(arg1, "-R") == 0)) {
                char files[512] = "";
                files[0] = 0;
                int len = 0;
                if (strlen(arg2) == 0) {
                    printl("input error!");
                    continue;
                }
                if (arg2[0] != '/') {
                    addTwoString(temp, current_dirr, arg2);
                    memcpy(arg2, temp, 512);
                }
                //printl("%s %s\n", arg1, arg2);
                find_all_path(files, arg2, &len);

                //printl("%d\n", len);
                
                // Unlink all the paths in the array files
                int index = 0;
                char current_path[32];
                char current_char = 0;
                char path_index = 0;
                while (current_char = files[index]) {
                    if (current_char != ' ') {
                        current_path[path_index] = current_char;
                        path_index++;
                    } else {
                        current_path[path_index] = 0;
                        path_index = 0;

                        // Unlink the current path or file
                        unlink(current_path);
                    }

                    index++;
                }

                // Unlink the current directory
                unlink(arg2);
                
                continue;
            }
            if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }
            
            int result;
            result = unlink(arg1);
            if (result == 0){
                printf("%s File deleted!\n",arg1);
                continue;
            }
            else{
                printf("Failed to delete file! Please check the filename!\n");
                continue;
            }
        }
        else if (strcmp(cmd, "cp") == 0){
            //get the content of file
            if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }
            fd = open(arg1, O_RDWR);
            if (fd == -1){
                printf("File not exists! Please check the filename!\n");
                continue ;
            }
            int tail = read(fd, buf, 1024);
            close(fd);

            if(arg2[0]!='/'){
                addTwoString(temp,current_dirr,arg2);
                memcpy(arg2,temp,512);                
            }
            /*create the file*/
            fd = open(arg2, O_CREAT | O_RDWR);
            if (fd == -1){
                //printl("file already exists\n");
            }
            else{
                //file not exist or create file
                buf[tail] = 0;
                write(fd, buf, tail+1);
                close(fd);
            }
            /*
            //set file to value
            fd = open(arg2, O_RDWR);
            write(fd, buf, tail+1);
            close(fd);
            */
        }
        else if (strcmp(cmd, "mv") == 0){
             if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }
            //get the content of file first
            fd = open(arg1, O_RDWR);
            if (fd == -1)
            {
                printf("File not exists! Please check the filename!\n");
                continue ;
            }
           
            int tail = read(fd, buf, 1024);
            close(fd);

            if(arg2[0]!='/'){
                addTwoString(temp,current_dirr,arg2);
                memcpy(arg2,temp,512);                
            }
            /*create the file*/
            fd = open(arg2, O_CREAT | O_RDWR);
            if (fd == -1){
                //file exist
            }
            else{
                //file not exist or create file
                buf[tail]=0;
                write(fd, buf, tail+1);
                close(fd);
            }
            
            // delete the file
            unlink(arg1);
        }   
        else if (strcmp(cmd, "encrypt") == 0)
        {

            if(arg1[0]==0){
                printf("Failed to open file! Please input the filename!\n");
                continue;
            }
            if(arg1[0]!='/'){
                addTwoString(temp,current_dirr,arg1);
                memcpy(arg1,temp,512);                
            }

            fd = open(arg1, O_RDWR);
            if (fd == -1){
                printf("Please check the filename!\n");
                continue ;
            }
            printl("%s\n",arg1);
            doEncrypt(arg1, fd_stdin);
        }
        else if (strcmp(cmd, "test") == 0)
        {
            doTest(arg1);
        }
        else if (strcmp(cmd, "game") == 0){
        	   game(fd_stdin);
        }
        else if (strcmp(cmd, "mkdir") == 0){
            i = j =0;
            while(current_dirr[i]!=0){
                arg2[j++] = current_dirr[i++];
            }
            i = 0;
            
            while(arg1[i]!=0){
                arg2[j++]=arg1[i++];
            }
            arg2[j]=0;
            printf("%s\n", arg2);
            fd = mkdir(arg2);            
        }
        else if (strcmp(cmd, "cd") == 0){
            if(strcmp(arg1, "..")==0){
                memcpy(arg2, current_dirr, 512);
                j =0;
                int k=0;
                int count =0;
                while(arg2[k] !=0){
                    if(arg2[k++]=='/'){
                        count++;
                    }
                }
                int index=0;
                for(i=0;arg2[i]!=0;i++){
                    if(arg2[i] == '/'){
                        index++;
                    }
                    if(index < count-1){
                        arg1[j++] = arg2[i];
                    }
                }
                arg1[j++] ='/';
                arg1[j]=0;
            }
            else if(arg1[0]==0){
                arg1[0] ='/';
                arg1[1]=0;
            }
            else{ // not absolute path from root
                i = j =0;
                while(current_dirr[i]!=0){
                    arg2[j++] = current_dirr[i++];
                }
                i = 0;
                while(arg1[i]!=0){
                    arg2[j++]=arg1[i++];
                }
                arg2[j++] = '/';
                arg2[j]=0;
                memcpy(arg1, arg2, 512);
            }
            //printf("%s\n", arg1);
            fd = open(arg1, O_RDWR);

            if(fd == -1){
                printf("The path not exists! Please check the pathname!\n");
            }
            else{
                memcpy(current_dirr, arg1, 512);
                //printf("Change dir %s success!\n", current_dirr);
            }
        }else if( strcmp(cmd, "calc") == 0){
	    calculator(fd_stdin, fd_stdout);
	}else if( strcmp(cmd, "minesweeper") == 0){
	    game(fd_stdin);
        }else if( strcmp(cmd, "snake")  == 0){
            snakeGame();
        }else if( strcmp(cmd, "2048") == 0){
	    start2048Game(fd_stdin, fd_stdout);
	}else if( strcmp(cmd, "guess") == 0){
	    fingerGuessingGame(fd_stdin, fd_stdout);
	}else
            printf("Command not found, please check!\n");
    }
}

/*======================================================================*
                               TestA
 *======================================================================*/

//A process
void TestA(){
    shell("/dev_tty0");
    assert(0);
}

/*======================================================================*
                               TestB
 *======================================================================
*/
//B process
void TestB(){
	shell("/dev_tty1");
	assert(0); /* never arrive here */
}
/*======================================================================*
                               TestC
 *======================================================================*/
//C process
void TestC(){
	//shell("/dev_tty2");
	assert(0);
}

/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...) {
    int i;
    char buf[256];
    /* 4 is the size of fmt in the stack */
    va_list arg = (va_list)((char*)&fmt + 4);
    i = vsprintf(buf, fmt, arg);
    printl("%c !!panic!! %s", MAG_CH_PANIC, buf);
    /* should never arrive here */
    __asm__ __volatile__("ud2");
}

/*****************************************************************************
 *                                Custom Command
 *****************************************************************************/
char* findpass(char *src){
    char pass[128];
    int flag = 0;
    char *p1, *p2;

    p1 = src;
    p2 = pass;

    while (p1 && *p1 != ' ')
    {
        if (*p1 == ':')
            flag = 1;

        if (flag && *p1 != ':')
        {
            *p2 = *p1;
            p2++;
        }
        p1++;
    }
    *p2 = '\0';

    return pass;
}

void clearArr(char *arr, int length){
    int i;
    for (i = 0; i < length; i++)
        arr[i] = 0;
}

void printAbout(){
    clear(); 
    if(current_console==0){
        printf("=============================================================================\n");
        printf("                                  My OS v1.0                                 \n");
        printf("     Built by Wang Jiacheng & Shen Xiaoang & Chen Zehui on Aug 13, 2018      \n");
        printf("                      Compiled on Sep 10 2018 at 22:43:08                    \n");
        printf("=============================================================================\n");
    }
    else{
    	printf("[TTY #%d]\n", current_console);
    }
}

void clear(){	
    clear_screen(0,console_table[current_console].cursor);
    console_table[current_console].crtc_start = console_table[current_console].orig;
    console_table[current_console].cursor = console_table[current_console].orig;    
}

void doTest(char *path) {
    struct dir_entry *pde = find_entry(path);
    printl(pde->name);
    printl("\n");
    printl(pde->pass);
    printl("\n");
}

int verifyFilePass(char *path, int fd_stdin) {
    char pass[128];

    struct dir_entry *pde = find_entry(path);

    //printl("in the verifyfile pass function\n");
    //printl(pde->pass);
    //printl("pde->pass is %s\n",pde->pass);

    if (strcmp(pde->pass, "") == 0)
        return 1;

    printl("Please input the file password: ");
    read(fd_stdin, pass, 128);

    if (strcmp(pde->pass, pass) == 0)
        return 1;

    return 0;
}

void doEncrypt(char *path, int fd_stdin) {
    //search the file
    /*struct dir_entry *pde = find_entry(path);*/

    char pass[128] = {0};

    printl("Please input the new file password: ");
    read(fd_stdin, pass, 128);

    if (strcmp(pass, "") == 0) {
        printl("A blank password!\n");
        strcpy(pass, "");
    }
    

    //encrypt the file
    int i, j;
    //printl("%s \n",path);
    char filename[MAX_PATH];
    memset(filename, 0, MAX_FILENAME_LEN);
    struct inode * dir_inode;

    if (strip_path(filename, path, &dir_inode) != 0){
        return 0;
    }
    //printl("%d\n",dir_inode->i_num);
    //printl("%s\n",filename);
    if (filename[0] == 0)   /* path: "/" */{
        return dir_inode->i_num;
    }
    
    /**
     * Search the dir for the file.
     */
    int dir_blk0_nr = dir_inode->i_start_sect;
    int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
    int nr_dir_entries =
    dir_inode->i_size / DIR_ENTRY_SIZE;
    int m = 0;
    struct dir_entry * pde;
    for (i = 0; i < nr_dir_blks; i++) {
        RD_SECT(dir_inode->i_dev, dir_blk0_nr + i);
        pde = (struct dir_entry *)fsbuf;
        for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++)
        {
            if (memcmp(filename, pde->name, MAX_FILENAME_LEN) == 0)
            {
                // delete the file
                strcpy(pde->pass, pass);
                WR_SECT(dir_inode->i_dev, dir_blk0_nr + i);
                printl("your input password is %s\n",pde->pass);
                return;
                /*return pde->inode_nr;*/
            }
            if (++m > nr_dir_entries)
                break;
        }
        //printl("---\n");
        if (m > nr_dir_entries) /* all entries have been iterated */
            break;
    }
    
}


void help() {
    printf("=============================================================================\n");
    printf("                                  My OS 1.0                                  \n");
    printf("     Built by Wang Jiacheng & Shen Xiaoang & Chen Zehui on Aug 13, 2018      \n");
    printf("                                   Welcome!                                  \n");
    printf("=============================================================================\n");
    printf("Usage: [command] [flags] [options]                                           \n");
    printf("    clear                         : clear the screen                         \n");
    printf("    ls                            : list files in current directory          \n");
    printf("    touch       [filename]        : create a new file                        \n");
    printf("    cat         [filename]        : display content of the file              \n");
    printf("    vi          [filename]        : modify the content of the file           \n");
    printf("    rm          [filename]        : delete a file                            \n");
    printf("    cp          [source] [dest]   : copy a file                              \n");
    printf("    mv          [source] [dest]   : move a file                              \n");
    printf("    encrypt     [filename]        : encrypt a file                           \n");
    printf("    cd          [pathname]        : change the directory                     \n");
    printf("    mkdir       [dirname]         : create a new directory                   \n");
    printf("    minesweeper                   : start the minesweeper game               \n");
    printf("    snake                         : start the snake game                     \n");  
    printf("    2048                          : start the 2048 game                      \n");
    printf("    cal                           : start the calculator                     \n");
    printf("    guess                         : start the finger-guessing game           \n");
    printf("    process                       : display all process-info and manage      \n");
    printf("    about                         : display the about of system              \n");
    printf("=============================================================================\n");
}

void ProcessManage()
{
    int i;
    printf("=============================================================================\n");
    printf("          myPID      |    name       |      priority    |     running        \n");
    printf("-----------------------------------------------------------------------------\n");
    for ( i = 0 ; i < NR_TASKS + NR_PROCS ; ++i ) {
        printf("%13d%17s%18d%18d\n",
               proc_table[i].pid, proc_table[i].name, proc_table[i].priority, proc_table[i].run_state);
    }
    printf("=============================================================================\n");
    printf("=          Usage: pause  [pid]  you can pause one process                   =\n");
    printf("=          	      resume [pid]  you can resume one process                  =\n");
    printf("=                 kill   [pid]  kill the process                            =\n");
    printf("=                 up     [pid]  improve the process priority                =\n");
    printf("=============================================================================\n");
}


/*======================================================================*
                            welcome animation
 *======================================================================*/
void animation() {
    clear();
	printf("_____________________________________________________\n");
	printf(".....................................................\n");
	printf(".mm...mmmmmmmm..............oooooooooo..sssssssss....\n");
	printf(".mm..mmmmmmmmmm.............oooooooooo..sssssssss....\n");
	printf(".mm.mmm..mm..mmm.yyy.....yyyoo......oo..ss...........\n");
	printf("..mmmm...mm...mmmyyy.....yyyoo......oo..ss...........\n");
	printf("...mmm...mm...mmmyyy.....yyyoo......oo..sssssssss....\n");
	printf("...mmm...mm...mmm.yyy....yyyoo......oo........sss....\n");
	printf("...mmm...mm...mmm..yyy..yyy.oooooooooosssssssssss....\n");
	printf("...mm....mm...mmm...yyyyyy..ooooooooossssssssssss....\n");
	printf("......................yyy............................\n");
	printf(".....................yyy.............................\n");
	printf("...................yyy...............................\n");
	printf("...............yyyy...........Version 1.0.0..........\n");
	printf(".............................WJC.&.SXA.&.CZH.........\n");
	printf(".....................................................\n");
	printf("_____________________________________________________\n");
    milli_delay(50000);	
    clear();
    help();
}
