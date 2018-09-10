/*======================================================================*
                            Minesweeper Game
 *======================================================================*/
unsigned int _seed2 = 0xDEADBEEF;

void srand(unsigned int seed){
	_seed2 = seed;
}

int rand() {
	int next = _seed2;
	int result;

	next *= 1103515245;
	next += 12345;
	result = (next / 65536) ;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (next / 65536) ;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (next / 65536) ;

	_seed2 = next;

	return result>0 ? result : -result;
}

void show_mat(int *mat,int *mat_state, int touch_x,int touch_y,int display){
	int x, y;
	for (x = 0; x < 10; x++){
		printf("  %d", x);
	}
	printf("\n");
	for (x = 0; x < 10; x++){
		printf("---");
	}
	for (x = 0; x < 10; x++){
		printf("\n%d|", x);
		for (y = 0; y < 10; y++){
			if (mat_state[x * 10 + y] == 0){				
					if (x == touch_x && y == touch_y)
						printf("*  ");
					else if (display!=0 && mat[x * 10 + y] == 1)
						printf("#  ");
					else
						printf("-  ");
				
			}
			else if (mat_state[x * 10 + y] == -1){
				printf("O  ");
			}
			else{
				printf("%d  ", mat_state[x * 10 + y]);
			}
			
		}
	}
	printf("\n");
}

void init_game(int *mat, int mat_state[100]){
	int sum = 0;
	int x, y;
	for (x = 0; x < 100; x++)
		mat[x] = mat_state[x] = 0;
	while (sum < 15){
		x = rand() % 10;
		y = rand() % 10;
		if (mat[x * 10 + y] == 0){
			sum++;
			mat[x * 10 + y] = 1;
		}
	}
	show_mat(mat,mat_state,-1,-1,0);
}

int check(int x, int y, int *mat){
	int i, j,now_x,now_y,result = 0;
	for (i = -1; i <= 1; i++){
		for (j = -1; j <= 1; j++){
			if (i == 0 && j == 0)
				continue;
			now_x = x + i;
			now_y = y + j;
			if (now_x >= 0 && now_x < 10 && now_y >= 0 && now_y <= 9){
				if (mat[now_x * 10 + now_y] == 1)
					result++;
			}
		}
	}
	return result;
}

void dfs(int x, int y, int *mat, int *mat_state,int *left_coin){
	int i, j, now_x, now_y,temp;
	if (mat_state[x*10+y] != 0)
		return;
	(*left_coin)--;
	temp = check(x, y,mat);
	if (temp != 0){
		mat_state[x * 10 + y] = temp;
	}
	else{
		mat_state[x * 10 + y] = -1;
		for (i = -1; i <= 1; i++){
			for (j = -1; j <= 1; j++){
				if (i == 0 && j == 0)
					continue;
				now_x = x + i;
				now_y = y + j;
				if (now_x >= 0 && now_x < 10 && now_y >= 0 && now_y <= 9){				
					dfs(now_x, now_y,mat,mat_state,left_coin);
				}
			}
		}
	}
}

void game(int fd_stdin){
	int mat[100] = { 0 };
	int mat_state[100] = { 0 };
	char keys[128];
	int x, y, left_coin = 100,temp;
	int flag = 1;

	while (flag == 1){
		init_game(mat, mat_state);
		left_coin = 100;
		printf("-------------------------------------------\n\n");
		printf("Input next x and y: ");

		while (left_coin != 15){

		clearArr(keys, 128);
                int r = read(fd_stdin, keys, 128);
		if(keys[0] == 'q'){
			break;
		}else if(keys[0]>'9'||keys[0]<'0'||keys[1]!=' '||keys[2]>'9'||keys[2]<'0'||keys[3]!=0){
                	printf("Please input again!\n");
    			continue;
            	}
            x = keys[0]-'0';
            y = keys[2]-'0';
			if (x < 0 || x>9 || y < 0 || y>9){
				printf("Please input again!\n");
				continue;
			}

			if (mat[x * 10 + y] == 1){
				break;
			}
			else{
				dfs(x, y, mat, mat_state, &left_coin);
				if (left_coin <= 15)
					break;
				show_mat(mat, mat_state, -1, -1, 0);
				printf("-------------------------------------------\n\n");
				printf("Input next x and y: ");
			}
		}
		if (mat[x * 10 + y] == 1){
			printf("\n\nFAIL!\n");
			show_mat(mat, mat_state, x, y, 1);
		}
		else{
			printf("\n\nSUCCESS!\n");
			show_mat(mat, mat_state, -1, -1, 1);
		}

		printf("Do you want to continue?(yes ot no)\n");
		clearArr(keys, 128);
        int r = read(fd_stdin, keys, 128);
        //  printf("%s\n",keys);
        if (keys[0]=='n' && keys[1]=='o' && keys[2]==0)
        {
        	flag = 0;
        //	printf("%s\n",keys);
            break;
        }
	}	
}
