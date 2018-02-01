#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

typedef struct {
    int absis, ordinat;
} point;

typedef struct{
    int r,g,b,a;
} color;

// INISIALISASI VARIABEL
int layarx = 1366;
int layary = 700;

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;

color white = {
			 255,
			 255,
			 255,
			 0
	 };

color green = {
	0,255,255,0
};

int min(int y1, int y2){
	if (y1 < y2) {
		return y1;
	} else {
		return y2;
	}
}

void draw_dot(int x, int y, color* c) {
	if((x<1) || (x>layarx) || (y<1) || (y>layary)){
		return ;
	}
    long int position = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
       (y + vinfo.yoffset) * finfo.line_length;
    if(vinfo.bits_per_pixel == 32){
        *(fbp + position) = c->b;
        *(fbp + position + 1) = c->g;
        *(fbp + position + 2) = c->r;
        *(fbp + position + 3) = c->a;
    }
    else
    { //assume 16 bit color
        int b = c->b;
        int g = c->g;
        int r = c->r;
        unsigned short int t = r<<11 | g << 5 | b;
        *((unsigned short int*)(fbp + position)) = t;
    }
}

int draw_line(int x1, int y1, int x2, int y2, color* c) {
	if (x2 < x1) {
		int temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
	}
   
	int grad = (y2-y1)*(x2-x1);
	int y = y1;
	int x = x1;
	int dx = x2 - x1;
	int dy = y2 - y1;
	if (grad > 0) {
		int dxdy = y2 - y1 + x1 - x2;
		int F = y2 - y1 + x1 - x2;
		for (x = x1; x <= x2; x++) {
			draw_dot(x, y, c);
			if (F < 0) {
				F += dy;
			} else {
				y++;
				F += dxdy;
			}
		}
   	} else {
        int dxdy = y2 - y1 + x1 - x2;
		int dydx = x2 - x1 + y1 - y2;
        int F;
		if (dx >= (-1)*dy) {
			F = y2 - y1 + x1 - x2;
			for (x = x1; x <= x2; x++) {
				draw_dot(x, y, c);
				if (F > 0) {
					F += dy;
				} else {
					y--;
					F -= dxdy;
				}
			}
		} else {
			// draw_line(0,100,50,0,&white); dx positif, f = dx-dy
			printf("X");
			F = x2 - x1 + y1 - y2;
			for (y = y1; y >= y2; y--) {
				draw_dot(x, y, c);
				if (F > 0) {
					F -= dx;
				} else {
					x++;
					F += dydx;
				}
			}
		}
   	}
}


void clear_screen(int width, int height) {
    int x = 0;
    int y = 0;

    for(x=0; x<width; x++)
    {
        for(y=0; y<height; y++)
        {
            long int position = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
               (y + vinfo.yoffset) * finfo.line_length;
            *(fbp + position) = 0;
            *(fbp + position + 1) = 0;
            *(fbp + position + 2) = 0;
            *(fbp + position + 3) = 0;
        }
    }
}

void get_char_points(point* charpoints, char* nama_file, int current_x, int current_y) {
	FILE* charmap;
	int i = 0;
	for (int j = 0; j < 20; j++) {
		charpoints[j].absis = 0;
		charpoints[j].ordinat = 0;
	}

	charmap = fopen(nama_file, "r");
	while (!feof (charmap)) {
		int x,y;
		fscanf(charmap, "%d", &x);
		fscanf(charmap, " ");
		fscanf(charmap, "%d", &y);
		charpoints[i].absis = x + current_x;
		charpoints[i].ordinat = y + current_y;
		
		i++;
	}

	color white = {
	  255,
	  255,
	  255,
	  0
	};


	int j = 0;
	while (charpoints[j].absis != 0) {
		if (j==20) {
			draw_line(charpoints[j].absis, charpoints[j].ordinat, charpoints[0].absis, charpoints[0].ordinat, &white);
		} else {
			if (charpoints[j+1].absis == 0) {
				draw_line(charpoints[j].absis, charpoints[j].ordinat, charpoints[0].absis, charpoints[0].ordinat, &white);
				//printf("drawing from point %d %d to %d %d\n", charpoints[j].absis, charpoints[j].ordinat, charpoints[0].absis, charpoints[0].ordinat);
			} else {
				draw_line(charpoints[j].absis, charpoints[j].ordinat, charpoints[j+1].absis, charpoints[j+1].ordinat, &white);
				//printf("drawing from point %d %d to %d %d\n", charpoints[j].absis, charpoints[j].ordinat, charpoints[j+1].absis, charpoints[j+1].ordinat);
			}
		}
		char c;
		j++;
		//scanf("%c",&c);
		//printf("%d %d\n", charpoints[j].absis, charpoints[j].ordinat);
		usleep(500000);
	}

	fclose;
}

int main() {
	int fbfd = 0;

	long int screensize = 0;

  	int x = 0, y = 0;
  	long int location = 0;

  	// Open the file for reading and writing
  	fbfd = open("/dev/fb0", O_RDWR);
  	if (fbfd == -1) {
  		perror("Error: cannot open framebuffer device");
		exit(1);
	}

	printf("The framebuffer device was opened successfully.\n");

	// Get fixed screen information
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
		perror("Error reading fixed information");
		exit(2);
	}

	// Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		perror("Error reading variable information");
		exit(3);
	}

	printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

	// Figure out the size of the screen in bytes
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	// Map the device to memory
	fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
			fbfd, 0);
	if ((int)fbp == -1) {
		perror("Error: failed to map framebuffer device to memory");
		exit(4);
	}
	printf("The framebuffer device was mapped to memory successfully.\n");

	// menerima string untuk ditulis ulang
	/* unsigned int len_max = 128;
  	unsigned int current_size = 0;   
  	char *pStr = malloc(len_max);
  	current_size = len_max;

	printf("\nEnter a very very very long String value:");

	int length = 0;
  	if(pStr != NULL) {
		int c = EOF;
		unsigned int i =0;
    	
    	//accept user input until hit enter or end of file
		while (( c = getchar() ) != '\n') {
			pStr[i++]=(char)c;
			length++;
			
			//if i reached maximize size then realloc size
			if(i == current_size) {
				current_size = i+len_max;
				pStr = realloc(pStr, current_size);
			}
		}
		pStr[i] = '\0';
		printf("\nLong String value: %s \n\n",pStr);
  }

  clear_screen(vinfo.xres, vinfo.yres);

	// Figure out where in memory to put the pixel
	int first_y = 100; //y awal;
	int first_x = 100;
	int current_y = first_y; //y untuk karakter sementara
	int current_x = first_x; //x untuk karakter sementara
	for (int i = 0; i < length; i++) {
		
		point charpoints[20];

		//baca map untuk pixel karakter
		if (pStr[i] == 'A') {
			get_char_points(charpoints, "A.txt", current_x, current_y);
		}

	}
*/
	clear_screen(1366, 600);
	point charpoints[20];
	//get_char_points(charpoints, "A.txt", 100, 100);
	draw_line(0,100,80,0,&white);
	
	munmap(fbp, screensize);

	close(fbfd);

	return 0;

}
