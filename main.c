#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

#define xSize 10
#define ySize 10

// Queue
typedef struct vector2 Vector2;
struct vector2
{
	int x;
	int y;
	Vector2* next;
};

Vector2* first;
Vector2* last;
int queueLen = 0;
void enqueue(int y, int x)
{
	Vector2* newVector = (Vector2*) malloc(sizeof(Vector2));
	newVector->y = y;
	newVector->x = x;
	newVector->next = NULL;
	if(first == NULL)
	{
		first = newVector;
		last = first;

	}
	else
	{
		last->next = newVector;
		last = newVector;
	}
	queueLen++;	
}

// This will cause memory leaks? and also in 298-299?
void dequeue()
{
	if(first == NULL || last == NULL)
		return;
	if(first == last)
	{
		free(first);
		first = NULL;
		last = NULL;
	}
	else
	{
		Vector2* next = first->next;
		free(first);
		first = next;
	}
	queueLen--;
}
// !Queue

enum
{
	PLAY     = 1<<0,
	MENU     = 1<<1,
	GAMEOVER = 1<<2
};

enum 
{
	BLANK = 1 << 0,
	HEAD  = 1 << 1,
	TAIL  = 1 << 2,
	FOOD  = 1 << 3
};

int board[xSize][ySize] = {};

int tailLength = 3;
int xHead = xSize / 2;
int yHead = ySize / 2;

int xDirection = 1;
int yDirection = 0;

int xInput = 1;
int yInput = 0;

int xFood;
int yFood;
int isGameOver = 0;

float speed = 1.0f/6;

int menu = MENU;

int isCollided(int y, int x, int head)
{
	if(xHead == x && yHead == y && head)
		return 1;
	Vector2* c = first;
	while(c != NULL)
	{
		if(c->x == x && c->y == y)
			return 1;
		c = c->next;
	}	
	if(y >= ySize ||
	   x >= xSize || 
	   y < 0 ||
	   x < 0)
	{
		return 1;
	}
	return 0;
}

void genFood()
{
	do
	{
		xFood = rand() % xSize;
		yFood = rand() % ySize;
	}
	while(isCollided(yFood, xFood, 1));
}
void eatFood()
{
	if(xHead == xFood && yHead == yFood)
	{
		tailLength++;
		genFood();
	}
}

void setInput(int y, int x)
{
	yInput = y;
	xInput = x;
}

void clearBoard()
{
	for(int y = 0; y < ySize; ++y)
	{
		for(int x = 0; x < xSize; ++x)
		{
			board[y][x] = BLANK; 
		}
	}
}

void handleInput()
{
	if(_kbhit())
	{
		int key = _getch();
		switch (key) {
			case 'w':
				setInput(-1, 0);
				break;
			case 's':
				setInput(1, 0);
				break;
			case 'a':
				setInput(0, -1);
				break;
			case 'd':
				setInput(0, 1);
				break;
		}
	}
}

void update()
{
	enqueue(yHead, xHead);
	while(queueLen > tailLength)
		dequeue();
	
	if(!(xDirection == 1  && xInput == -1 ||
	   xDirection == -1 && xInput == 1 ||
	   yDirection == 1  && yInput == -1 ||
	   yDirection == -1 && yInput == 1)) 
	{
		xDirection = xInput;
		yDirection = yInput;
	}

	yHead += yDirection;
	xHead += xDirection;

	// Checking the head pos
	//if(yHead  < 0)
	//	yHead = ySize - 1;
	//else if(yHead >= ySize)
	//	yHead = 0;
	//if(xHead < 0)
	//	xHead = xSize - 1;
	//else if(xHead >= xSize)
	//	xHead = 0;
	
	if(isCollided(yHead, xHead, 0))
	{
		isGameOver = 1;
		menu = GAMEOVER;
		return;
	}

	eatFood();

	// Tail
	Vector2* current = first;
	while(current != NULL)
	{
		board[current->y][current->x] = TAIL;
		current = current->next;
	}
	
	// Head
	board[yHead][xHead] = HEAD;
	board[yFood][xFood] = FOOD;
}

// if game is true draw the snake, else nothing
void draw(int game)
{
	printf("\033[0;0H");
	printf("\e[?25l");
	printf("+----------+\n");
	for(int y = 0; y < ySize; ++y)
	{
		printf("|");
		for(int x = 0; x < xSize; ++x)
		{
			int v = board[y][x];
			if(game)
			{
				if(v == HEAD)
					printf("O");
				else if(v == TAIL)
					printf("#");
				else if(v == BLANK)
					printf(" ");
				else if(v == FOOD)
					printf("@");
			}
			else
				printf(" ");

			if(x == xSize - 1)
			{
				printf("|");
				if(game && y == 1)
				{
					printf("Score : %d", tailLength - 3);
				}
				if(y == 3)
					printf("W, A, S, D to move");
				if(y == 4)
					printf("Space to select");
			}
		}
		printf("\n");
	}
	printf("+----------+\n");
}

void play()
{
	clock_t start = clock();
	while(!isGameOver)
	{
		handleInput();
		clock_t now = clock();
		if((float)(now - start) / CLOCKS_PER_SEC > speed)
		{
			clearBoard();
			update();
			start = now;
		}

		draw(1);
	}
}

void reset()
{
	clearBoard();
	isGameOver = 0;
	xHead = xSize / 2;
	yHead = ySize / 2;
	xDirection = 1;
	yDirection = 0;
	xInput = 1;
	yInput = 0;
	tailLength = 3;
	while(queueLen > 0)
		dequeue();
	first = NULL;
	last = NULL;
	genFood();
}

int main(int argc, char* argv[])
{
	while(1)
	{
		system("cls");
		if(menu == PLAY)
		{
			reset();
			play();
		}
		else if(menu == MENU)
		{
			int selected = 0;
			int exit = 0;
			while(!exit)
			{
				draw(0);
				printf("\033[4;4H");
				printf("-Menu-");

				printf("\033[5;3H");
				if(!selected)
					printf(">>");
				else
					printf("  ");
				printf("PLAY");
				if(!selected)
					printf("<<");

				printf("\033[6;3H");
				if(selected)
					printf(">>");
				else
					printf("  ");
				printf("EXIT");
				if(selected)
					printf("<<");
				
				int key = getch();
				switch(key)
				{
					case ' ':
						exit = 1;
						break;
					case 'w': case 's':
						selected = !selected;
						break;
				}
			}
			if(selected)
				return 0;
			else
				menu = PLAY;
		}
		else if(menu == GAMEOVER)
		{
			int selected = 0;
			int exit = 0;
			while(!exit)
			{
				draw(0);
				printf("\033[4;2H");
				printf("-GameOver-");

				printf("\033[5;2H");
				if(selected == 0)
					printf(">>");
				else
					printf("  ");
				printf("Retry");
				if(selected == 0)
					printf("<<");

				printf("\033[6;2H");
				if(selected == 1)
					printf(">>");
				else
					printf("  ");
				printf("Menu");
				if(selected == 1)
					printf("<<");

				printf("\033[7;2H");
				if(selected == 2)
					printf(">>");
				else
					printf("  ");
				printf("Exit");
				if(selected == 2)
					printf("<<");			

				int key = getch();
				switch(key)
				{
					case ' ':
						exit = 1;
						break;
					case 'w':
						selected -= 1;
						if(selected < 0)
							selected = 2;
						break;
					case 's':
						selected += 1;
						if(selected > 2)
							selected = 0;
						break;
				}
			}
			if(selected == 0)
				menu = PLAY;
			else if(selected == 1)
				menu = MENU;
			else if(selected == 2)
				return 0;
		}
	}
	
	while(first != NULL)
		dequeue();
	system("pause");
	return 0;
}
