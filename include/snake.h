/*======================================================================*
                            snake game
 *======================================================================*/

/**
 *  listen the keybord
 * @param key
 * @return
 */
PUBLIC void judgeInpt(u32 key) {
    char output[2] = {'\0', '\0'};
    if (!(key & FLAG_EXT)) {
        output[0] = key & 0xFF;
        if(output[0] == 'a') changeToLeft();
        if(output[0] == 's') changeToDown();
        if(output[0] == 'd') changeToRight();
        if(output[0] == 'w') changeToUp();
    }
}

int listenerStart = 0;

struct Snake{   //every node of the snake 
    int x, y;  
    int now;   //0,1,2,3 means left right up down   
}Snake[15*35];  //Snake[0] is the head，and the other nodes are recorded in inverted order，eg: Snake[1] is the tail


//change the direction of circle
void changeToLeft(){
    if(listenerStart == 1){
        Snake[0].now = 0;
        listenerStart = 0;
    }
}
void changeToDown(){
    if(listenerStart == 1){
        Snake[0].now = 3;
        listenerStart = 0;
    }
}
void changeToRight(){
    if(listenerStart == 1){
        Snake[0].now = 1;
        listenerStart = 0;
    }
}
void changeToUp(){
    if(listenerStart == 1){
        Snake[0].now = 2;
        listenerStart = 0;
    }
}

const int mapH = 15;
const int mapW = 35;
char sHead = '@';
char sBody = 'O';
char sFood = '$';
char sNode = '.';    
char Map[15][35]; // the map of snake
int food[15][2] = {{5, 13},{6, 10}, {17, 15}, {8, 9}, {3, 4}, {1,12}, {0, 2}, {5, 23},
                   {15, 13},{16, 10}, {7, 15}, {8, 19}, {3, 14}, {11,12}, {10, 2}};
int foodNum = 0;
int eat = -1;
int win = 15;  // the length of win
 
int sLength = 1;
int overOrNot = 0;
int dx[4] = {0, 0, -1, 1};  
int dy[4] = {-1, 1, 0, 0}; 

void initGame(); 
void initFood();
void show();
void move();
void checkBorder();
void checkHead(int x, int y);
void action();
void showGameSuccess();
void showGameSuccess();
/**
 * enter the snake game
 */
void snakeGame(){
    clear();
    initGame();  
    show(); 
}
/**
 * init game
 */
void initGame() {
    int i, j;  
    int headx = 5;
    int heady = 10;
    memset(Map, '.', sizeof(Map));   //init map with '.'
    Map[headx][heady] = sHead;  
    Snake[0].x = headx;  
    Snake[0].y = heady;  
    Snake[0].now = -1;
    initFood();   //init target 
    for(i = 0; i < mapH; i++){
        for(j = 0; j < mapW; j++)  
            printf("%c", Map[i][j]);  
        printf("\n");  
    } 
    printf("press 'a''s''d''w' key and start the game\n");
    listenerStart = 1;
    while(listenerStart);
}

/**
 * the food location
 */
void initFood(){
    int fx, fy;
    int tick;  
    while(1){
        tick = get_ticks();
        fx = tick%mapH;
        fy = tick%mapW;     
        if(Map[fx][fy] == '.'){
            eat++;
            Map[fx][fy] = sFood;  
            break;  
        }
        foodNum++;
    }
}

/**
 * show game situation
 */
void show(){
    int i, j; 
    printf("Load snake game ...");
    while(1){
        listenerStart = 1;

        if(eat < 5){
            milli_delay(8000);
        }else if(eat < 10){
            milli_delay(6000);
        }else{
            milli_delay(5000);
        }
        move();  
        if(overOrNot) 
        {
            showGameOver();
            milli_delay(50000);
            clear();
            break;  
        } 
        if(eat == win)
        {
            showGameSuccess();
            milli_delay(50000);
            clear();
            break;
        }
        clear();
        for(i = 0; i < mapH; i++)   
        {   
            for(j = 0; j < mapW; j++)  
            printf("%c", Map[i][j]);  
            printf("\n");  
        }  

        printf("           Have fun!\n");
        printf("       You have ate:%d\n",eat);
    }  
}
/**
 * snake move function
 */
void move(){
    int i, x, y;  
    int t = sLength;
    x = Snake[0].x;  
    y = Snake[0].y;  
    Snake[0].x = Snake[0].x + dx[Snake[0].now]; 
    Snake[0].y = Snake[0].y + dy[Snake[0].now];  

    Map[x][y] = '.'; 
    checkBorder(); 
    checkHead(x, y);   
    if(sLength == t)  //did not eat
        for(i = 1; i < sLength; i++){
            if(i == 1)   //tail  
                Map[Snake[i].x][Snake[i].y] = '.';  
     
            if(i == sLength-1)  //the node after the head 
            {  
                Snake[i].x = x;  
                Snake[i].y = y;  
                Snake[i].now = Snake[0].now;  
            }  
            else 
            {  
                Snake[i].x = Snake[i+1].x;  
                Snake[i].y = Snake[i+1].y;  
                Snake[i].now = Snake[i+1].now;  
            }  
            Map[Snake[i].x][Snake[i].y] = sBody;  
        }  
}
/**
 *
 */
void checkBorder(){
    if(Snake[0].x < 0 || Snake[0].x >= mapH || Snake[0].y < 0 || Snake[0].y >= mapW){
        printl("game over!");
        overOrNot = 1;  
    }
    
}
/**
 *
 * @param x
 * @param y
 */
void checkHead(int x, int y){
    if(Map[Snake[0].x][Snake[0].y] == '.')
        Map[Snake[0].x][Snake[0].y] = sHead ;  
    else if(Map[Snake[0].x][Snake[0].y] == sFood) {
        Map[Snake[0].x][Snake[0].y] = sHead ;    
        Snake[sLength].x = x;                //new node
        Snake[sLength].y = y;  
        Snake[sLength].now = Snake[0].now;  
        Map[Snake[sLength].x][Snake[sLength].y] = sBody;   
        sLength++;  
        initFood();  
    }  
    else{ 
        overOrNot = 1; 
    }
}
/**
 *
 */
void showGameOver(){
    printf("=======================================================================\n");
    printf("==============================Game Over================================\n");
    printf("=======================will exit in 3 seconds...=======================\n");
}

/**
 *
 */
void showGameSuccess(){
    printf("=======================================================================\n");
    printf("============================Congratulation!============================\n");
    printf("=======================will exit in 3 seconds...=======================\n");
}