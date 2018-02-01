#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

typedef struct {
    int r,g,b,a;
} color;

typedef struct {
    int absis, ordinat;
} point;

void draw_dot(int x, int y, color* c) {
	if((x<1) || (x>layarx) || (y<1) || (y>layary)) {
		return ;
	}
  long int position = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
     (y + vinfo.yoffset) * finfo.line_length;
  if(vinfo.bits_per_pixel == 32){
    *(fbp + position) = c->b;
    *(fbp + position + 1) = c->g;
    *(fbp + position + 2) = c->r;
    *(fbp + position + 3) = c->a;
  } else { //assume 16 bit color
    int b = c->b;
    int g = c->g;
    int r = c->r;
    unsigned short int t = r<<11 | g << 5 | b;
    *((unsigned short int*)(fbp + position)) = t;
  }
}

int draw_line(int x1, int y1, int x2, int y2, color* c) {
   int grad = (y2-y1)*(x2-x1);
	 int y = y1;
	 int dy = y2 - y1;
   if (grad > 0) {
    int dxdy = y2 - y1 + x1 - x2;
    int F = y2 - y1 + x1 - x2;
    for (int x = x1; x <= x2; x++) {
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
    int F = y2 - y1 + x1 - x2;
    for (int x = x1; x <= x2; x++) {
      draw_dot(x, y, c);
      if (F > 0) {
        F += dy;
      } else {
        y--;
        F += dxdy;
      }
    }
  }
}

point* get_char_points(char* nama_file) {
	FILE* charmap;
	char ch, buffer[32];
	int i = 0, arr[100];
	point charpoints[20];

	charpoints = fopen(nama_file, "r");
	while (!feof (charmap)) {
		fscanf(charmap, "%d %d", charpoints[i]->absis, charpoints[i]->ordinat);
		i++;
	}
}

int main() {
	int fbfd = 0;
  struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long int screensize = 0;
  char *fbp = 0;
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

	//read char
	int charlength = 60;
	int charheight = 60;

	char **pixelmap = (char **)malloc(charheight * sizeof(char *));
	for (int i=0; i<charheight; i++)
		pixelmap[i] = (char *)malloc(charlength * sizeof(char));

	// menerima string untuk ditulis ulang
	unsigned int len_max = 128;
  unsigned int current_size = 0;   
  char *pStr = malloc(len_max);
  current_size = len_max;

	printf("\nEnter a very very very long String value:");

	int length = 0;
  if(pStr != NULL) {
		int c = EOF;
		unsigned int i = 0;
    	
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

  //menghitamkan layar
	for (y = 0; y < vinfo.yres; y++) {
		for (x = 0; x < vinfo.xres; x++) {
			location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
					(y+vinfo.yoffset) * finfo.line_length;
			*(fbp + location) = 0;        // hitam
			*(fbp + location + 1) = 0;     // hitam
			*(fbp + location + 2) = 0;    // hitam
			*(fbp + location + 3) = 0;      // No transparency
		}
	}

	// Figure out where in memory to put the pixel
	int first_y = 100; //y awal;
	int first_x = 100;
	int current_y = first_y; //y untuk karakter sementara
	int current_x = first_x; //x untuk karakter sementara
	for (int i = 0; i < length; i++) {
		
		//baca map untuk pixel karakter
		if (pStr[i] == 'A') {
			charmap = fopen("A.txt", "r");
		} else  if (pStr[i] == 'B') {
			charmap = fopen("B.txt", "r");
		} else  if (pStr[i] == 'C') {
			charmap = fopen("C.txt", "r");
		} else  if (pStr[i] == 'D') {
			charmap = fopen("D.txt", "r");
		} else  if (pStr[i] == 'E') {
			charmap = fopen("E.txt", "r");
		} else  if (pStr[i] == 'F') {
			charmap = fopen("F.txt", "r");
		} else  if (pStr[i] == 'G') {
			charmap = fopen("G.txt", "r");
		} else  if (pStr[i] == 'H') {
			charmap = fopen("H.txt", "r");
		} else  if (pStr[i] == 'I') {
			charmap = fopen("I.txt", "r");
		} else  if (pStr[i] == 'J') {
			charmap = fopen("J.txt", "r");
		} else  if (pStr[i] == 'K') {
			charmap = fopen("K.txt", "r");
		} else  if (pStr[i] == 'L') {
			charmap = fopen("L.txt", "r");
		} else  if (pStr[i] == 'M') {
			charmap = fopen("M.txt", "r");
		} else  if (pStr[i] == 'N') {
			charmap = fopen("N.txt", "r");
		} else  if (pStr[i] == 'O') {
			charmap = fopen("O.txt", "r");
		} else  if (pStr[i] == 'P') {
			charmap = fopen("P.txt", "r");
		} else  if (pStr[i] == 'Q') {
			charmap = fopen("Q.txt", "r");
		} else  if (pStr[i] == 'R') {
			charmap = fopen("R.txt", "r");
		} else  if (pStr[i] == 'S') {
			charmap = fopen("S.txt", "r");
		} else  if (pStr[i] == 'T') {
			charmap = fopen("T.txt", "r");
		} else  if (pStr[i] == 'U') {
			charmap = fopen("U.txt", "r");
		} else  if (pStr[i] == 'V') {
			charmap = fopen("V.txt", "r");
		} else  if (pStr[i] == 'W') {
			charmap = fopen("W.txt", "r");
		} else  if (pStr[i] == 'X') {
			charmap = fopen("X.txt", "r");
		} else  if (pStr[i] == 'Y') {
			charmap = fopen("Y.txt", "r");
		} else  if (pStr[i] == 'Z') {
			charmap = fopen("Z.txt", "r");
		} else  if (pStr[i] == ' ') {
			charmap = fopen("Spasi.txt", "r");
		}
		
		for (int i = 0; i < charheight; i++) {
			fscanf (charmap, "%s", pixelmap[i]);
		}
		fclose;

		//menulis ke framebuffer
		int max_length = (int)(vinfo.xres);
		for (y = current_y; y < current_y+charheight; y++) {
			for (x = current_x; x < current_x+charlength; x++) {
				location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
						(y+vinfo.yoffset) * finfo.line_length;

				if (vinfo.bits_per_pixel == 32) {
					if (pixelmap[y-current_y][x-current_x] == '*') {
						*(fbp + location) = 255;        // putih
						*(fbp + location + 1) = 255;     // putih
						*(fbp + location + 2) = 255;    // putih
						*(fbp + location + 3) = 0;      // No transparency
					} else {
						*(fbp + location) = 0;        // hitam
						*(fbp + location + 1) = 0;     // hitam
						*(fbp + location + 2) = 0;    // hitam
						*(fbp + location + 3) = 0;      // No transparency
					}
				}
			}
		}
		if (current_x > max_length-first_x-charlength) {
			current_y += 100;
			current_x = 100;
		} else {
			current_x += 60;
		}
		usleep(1000000);
	}

	munmap(fbp, screensize);

	free(pStr);
	pStr = NULL;
	close(fbfd);
	
	return 0;

 }
