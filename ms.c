//cs335 Spring 2013
//program: ms.c
//author:  gordon griesel
//purpose: midterm sampler
//
//This program contains functions you can use
//in your midterm and final project.
//
//1. draw a game board in middle of screen
//2. draw grid lines
//3. find the center of each grid square
//4. draw smaller squares inside each grid square
//5. track mouse movement over each square
//6. highlight grid squares
//7. grids textured with images
//
//
//notes:
//This code assumes a 4x4 grid on the game board.
//Try to make the grid dimensions generic.
//
//There are many places in the code that can be made more efficient and more
//generic. Part of the exercise for the midterm/final projects is to improve
//the code samples you are given, based upon the features you include in your
//project.
//
//
#include <stdio.h>
#include <stdlib.h>
#include <GL/glfw.h>
//macros
#define rnd() (double)rand()/(double)RAND_MAX
//prototypes
void init(void);
int init_glfw(void);
void init_opengl(void);
void render(void);
void GLFWCALL mouse_click(int button, int action);
void check_mouse(void);
void get_grid_center(const int i, const int j, int cent[2]);
int xres=640;
int yres=480;
int size;           //The dimension of the board


typedef struct t_grid {
	int status;
	int over;
	float color[4];
	struct t_grid *prev;
	struct t_grid *next;
} Grid;
Grid grid[25][25];
int grid_dim=4;
int board_dim;
int qsize;
//
int lbutton=0;
int rbutton=0;
//
GLuint Htexture;
GLuint Vtexture;
GLuint Background;
GLuint loadBMP(const char *imagepath);


int main(int argc, const char *argv[])
{
    int i, j;
    //Prompts user for dimensions
    printf("Game Functions: Press 'r' to reset the grid.\n");
    printf("Press 'Middle Button' to clear individual grid square.\n");
    printf("Enter the dimensions of the grid. Cap is 20x20.\n");
    scanf("%d",&size);
    if(size>20) size = 20;
	
    if (init_glfw()) {
		exit(EXIT_FAILURE);
	}
	init_opengl();
	init();
	srand((unsigned int)time(NULL));
	while(1) {
		check_mouse();
		render();
		glfwSwapBuffers();
        if (glfwGetKey('R') == GLFW_PRESS){
            for (i=0; i<size; i++) {
                for (j=0; j<size; j++) {
                    grid[i][j].status = 0;
                }
            }
        }
                 
		
		if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS) break;
		if (!glfwGetWindowParam(GLFW_OPENED)) break;
	}
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

int init_glfw(void)
{
	int nmodes;
	GLFWvidmode glist[256];
	if (!glfwInit()){
		printf("Failed to initialize GLFW\n");
		return 1;
	}
	//get the monitor native full-screen resolution
	nmodes = glfwGetVideoModes(glist, 250);
	xres = glist[nmodes-1].Width;
	yres = glist[nmodes-1].Height;
	//create a window
	//if (!glfwOpenWindow(xres, yres, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
	if (!glfwOpenWindow(xres,yres,8,8,8,0,32,0,GLFW_FULLSCREEN)) {
		glfwTerminate();
		return 1;
	}
	glfwSetWindowTitle("Game Board and Grid");
	glfwSetWindowPos(0, 0);
	//make sure we see the escape key pressed
	glfwEnable(GLFW_STICKY_KEYS);
	glfwSetMouseButtonCallback(mouse_click);
	glfwEnable( GLFW_MOUSE_CURSOR );
	//enable vertical sync (on cards that support it)
	glfwSwapInterval(1);
	return 0;
}

void init_opengl(void)
{
	//OpenGL initialization
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_COLOR_MATERIAL);
	//
	//choose one of these
	//glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_TEXTURE_2D);
	Htexture = loadBMP("H.bmp");
	Vtexture = loadBMP("V.bmp");
    Background = loadBMP("space.bmp");
	glBindTexture(GL_TEXTURE_2D, 0);
	printf("tex: %i %i\n",Htexture,Vtexture);
}

void init(void)
{
	board_dim = yres - 150;
	//make board dim divisible by 4
    board_dim >>= 2;
    board_dim <<= 2;
	int half_board_dim = board_dim/2;
	int one_grid_sec, bp;
	//quad upper-left corner
	//one_grid_sec is the width of one grid section
	one_grid_sec = board_dim/size;
	qsize = (one_grid_sec-10) / 2;
	//
	//notes:
	//This code is not generic.
	//A goal for this project is to make your board-size generic.
	//Allow the user to select the grid dimensions.
	//For instance, 8x8 board, or 20x20 board.
	//Maybe get this parameter at the command-line.
}

void check_mouse(void)
{
	static int sx=0,sy=0;	
	int x,y;
	int i,j;
	int cent[2];
	lbutton=0;
	rbutton=0;
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		//left mouse button is pressed.
		lbutton=1;
	}
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		//right mouse button is pressed.
		rbutton=1;
	}
	glfwGetMousePos(&x, &y);
	//reverse the y position
	y = yres - y;
	if (x == sx && y == sy) return;
	sx=x;
	sy=y;	
	//
	//is the mouse over any grid squares?
	//
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			grid[i][j].over=0;
		}
	}
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			get_grid_center(i,j,cent);
			if (x >= cent[0]-qsize &&
				x <= cent[0]+qsize &&
				y >= cent[1]-qsize &&
				y <= cent[1]+qsize) {
				grid[i][j].over=1;
				//break;
                return;
				//You could do a return here.
				//If more code is added below, a return
				//would cause you to exit too early.
			}
		}
		if (grid[i][j].over) break;
	}
}

void GLFWCALL mouse_click(int button, int action)
{
	int x,y;
	if (action == GLFW_PRESS) {
		int i,j,k=0;
		//center of a grid
		int cent[2];
		glfwGetMousePos(&x, &y);
		//reverse the y position
		y = yres - y;
		for (i=0; i<size; i++) {
			for (j=0; j<size; j++) {
				get_grid_center(i,j,cent);
				if (x >= cent[0]-qsize &&
					x <= cent[0]+qsize &&
					y >= cent[1]-qsize &&
					y <= cent[1]+qsize) {
					if (button == GLFW_MOUSE_BUTTON_LEFT)  grid[i][j].status=1;
					if (button == GLFW_MOUSE_BUTTON_RIGHT) grid[i][j].status=2;
                    //middle button resets square
                    if (button == GLFW_MOUSE_BUTTON_MIDDLE) grid[i][j].status=0;
                    k=1;
					break;
				}
			}
			if (k) break;
		}
	}
}

void get_grid_center(const int i, const int j, int cent[2])
{
	//This function can be optimized, and made more generic.
	int half_board_dim = board_dim/2;
	int screen_center[2] = {xres/2, yres/2};
	int x_screen_center = screen_center[0];
	int y_screen_center = screen_center[1];
	int one_grid_sec, bp;
	//quad upper-left corner
	int quad[2];
	//make board dim divisible by 4
    //board_dim >>= 2;
	//board_dim <<= 2;
	//one_grid_sec is the width of one grid section
	one_grid_sec = board_dim/size;
    //-------------------------------------
	quad[0] = x_screen_center-half_board_dim;
	quad[1] = y_screen_center-half_board_dim;
	cent[0] = quad[0] + one_grid_sec/2;
	cent[1] = quad[1] + one_grid_sec/2;
	cent[0] += (one_grid_sec * j);
	cent[1] += (one_grid_sec * i);
}

void render(void)
{
	int i,j;
	//--------------------------------------------------------
	//This code is repeated several times in this program, so
	//it can be made more generic and cleaner with some work.
	int half_board_dim = board_dim/2;
	int screen_center[2] = {xres/2, yres/2};
	int x_screen_center = screen_center[0];
	int y_screen_center = screen_center[1];
	int one_grid_sec, bp;
	//quad upper-left corner
	int quad[2], saveq0;
	//center of a grid
	int cent[2];
	//one_grid_sec is the width of one grid section
	one_grid_sec = board_dim/size;
	//--------------------------------------------------------
	//start the opengl stuff
	//set the viewing area on screen
	glViewport(0, 0, xres, yres);
	//clear color buffer
	//glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//init matrices
	glMatrixMode (GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//this sets to 2D mode (no perspective)
	glOrtho(0, xres, 0, yres, -1, 1);
	glColor3f(1.0f, 1.0f, 1.0f);

    //Background Texture
    glBindTexture(GL_TEXTURE_2D,Background);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f); glVertex2i(0,0);
        glTexCoord2f(0.0f,1.0f); glVertex2i(0,yres);
        glTexCoord2f(1.0f,1.0f); glVertex2i(xres,yres);
        glTexCoord2f(1.0f,0.0f); glVertex2i(xres,0);
    glEnd();
    glBindTexture(GL_TEXTURE_2D,0);

	/*draw stuff
	//draw the main game board in middle of screen
	glBegin(GL_QUADS);
		glVertex2i(x_screen_center-half_board_dim, y_screen_center-half_board_dim);
		glVertex2i(x_screen_center-half_board_dim, y_screen_center+half_board_dim);
		glVertex2i(x_screen_center+half_board_dim, y_screen_center+half_board_dim);
		glVertex2i(x_screen_center+half_board_dim, y_screen_center-half_board_dim);
	glEnd();
    */
	//draw grid lines
	//vertical
	glColor3f(0.0f, 0.0f, 1.0f);
	bp = x_screen_center-half_board_dim;
	glLineWidth(2);
	glBegin(GL_LINES);

    int it;
    for(it=0;it<(size-1);it++){
		bp += one_grid_sec;
		glVertex2i(bp, y_screen_center-half_board_dim);
		glVertex2i(bp, y_screen_center+half_board_dim);
    }
	
	glEnd();
	//horizontal
	glColor3f(0.0f, 0.0f, 1.0f);
	bp = y_screen_center-half_board_dim;
	glBegin(GL_LINES);
    for(it=0;it<(size-1);it++){
		bp += one_grid_sec;
		glVertex2i(x_screen_center-half_board_dim, bp);
		glVertex2i(x_screen_center+half_board_dim, bp);
    }
	glEnd();
	glLineWidth(1);
	//
	//draw a new square in center of each grid
	//squares are slightly smaller than grid
	//
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			get_grid_center(i,j,cent);
			glColor3f(0.0f, 1.0f, 1.0f);
			if (grid[i][j].over) {
				glColor3f(0.0f, 1.0f, 0.0f);
			}
            //Highlights Path
            if (grid[i][j].path){
                glColor3f(1.0f, 0.0f, 0.0f);
			glBindTexture(GL_TEXTURE_2D, 0);
			if (grid[i][j].status==1) glBindTexture(GL_TEXTURE_2D, Vtexture);
			if (grid[i][j].status==2) glBindTexture(GL_TEXTURE_2D, Htexture);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex2i(cent[0]-qsize,cent[1]-qsize);
				glTexCoord2f(0.0f, 1.0f); glVertex2i(cent[0]-qsize,cent[1]+qsize);
				glTexCoord2f(1.0f, 1.0f); glVertex2i(cent[0]+qsize,cent[1]+qsize);
				glTexCoord2f(1.0f, 0.0f); glVertex2i(cent[0]+qsize,cent[1]-qsize);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

GLuint loadBMP(const char *imagepath)
{
	//When you create your texture files, please specify
	//type: BMP
	//color depth: 24-bit
	unsigned int retval;
	unsigned char header[54];
	//Each BMP file begins by a 54-bytes header
	//Position in the file where the actual data begins
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int imageSize;
	// = width*height*3
	//RGB data will go in this
	unsigned char *data; 
	//
	printf("loadBMP(%s)...\n",imagepath);
	//Log("opening file **%s**\n",imagepath);
	FILE * file = fopen(imagepath,"r");
	if (!file) {
		printf("Image could not be opened\n");
		return 0;
	} 
	if (fread(header, 1, 54, file)!=54) {
		// If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0]!='B' || header[1]!='M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	dataPos   = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width     = *(int*)&(header[0x12]);
	height    = *(int*)&(header[0x16]); 
	//Some BMP files are misformatted, guess missing information
	if (imageSize==0) imageSize=width*height*3;
	if (dataPos==0) dataPos=54; 
	data = (unsigned char *)malloc(imageSize+1);
	//Read the actual data from the file into the buffer
	retval = fread(data,1,imageSize,file);
	fclose(file); 
	//In glTexImage2D, the GL_RGB indicates that we are talking
	//about a 3-component color, and GL_BGR says how exactly
	//it is represented in RAM. As a matter of fact, BMP does
	//not store Red->Green->Blue but Blue->Green->Red, so we
	//have to tell it to OpenGL.
	#define GL_BGR 0x80E0
	//Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	free(data);
	return textureID;
}
