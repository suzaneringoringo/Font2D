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
int jumlah_maksimal_titik = 20;

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

int max(int y1, int y2) {
	if (y2 < y1) {
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

   int y = y1;
   int x = x1;
   int dy = y2 - y1;
   int dx = x2 - x1;


    //kasus vertikal 
    if (x1 == x2) {
        for (int y = min(y1, y2); y <= max(y1, y2); y++) {
            draw_dot(x,y,c);
        }
    }

    //kasus_horizontal 
    else if (y1 == y2) {
        for(int x = x1; x <= x2; x++) {
            draw_dot(x,y,c);
        }
    }

	//kasus miring
    else {
        float grad = (float)(y2-y1)/(float)(x2-x1);

        //gradien > 0
        if (grad > 0) {
                //gradien <= 1
                if (grad <= 1) {       
                    int dxdy = y2 - y1 + x1 - x2;
                    int F = y2 - y1 + x1 - x2; 
                    for (int x = x1; x <= x2; x++) {
                        draw_dot(x,y,c);
                        if (F < 0) {
                            F += dy;
                        } else {
                            y++;
                            F += dxdy;
                        }
                    }
                }

                //gradien > 1
                else {
                    int x = x1;
                    int dx = x2 -x1;
                    int dxdy = x2 - x1 + y1 - y2;
                    int F = x2 - x1 + y1 - y2;
                    for (int y = y1; y <= y2; y++) {
                        
                        draw_dot(x,y,c);
                        if (F < 0) {
                            F += dx;
                        } else {
                            x++;
                            F += dxdy;
                        }
                    }
                }
		//gradien < 0
        } else {
                //gradien >= -1
                if (grad >= -1) {
                    int dy = y2 - y1;
                    if (dy < 0) {
                        dy *= -1;
                    }
                    int dx = x2 - x1;
                    int F = 2*dy - dx;
                    int y = y1;

			        for (x = x1; x <= x2; x++) {
                        draw_dot(x,y,c);
                        if (F > 0) {
                            y--;
                            F = F - 2*dx;
                        } 
                        F = F + 2*dy;
			        }
                }
                //gradien < -1
                else {
                    int dx = x1 - x2;
                    if (dx < 0) {
                        dx *= -1;
                    }
                    int dy = y1 - y2;
                    int F = 2*dx - dy;
                    int x = x2;

			        for (y = y2; y <= y1; y++) {
                        draw_dot(x,y,c);
                        if (F > 0) {
                            x--;
                            F = F - 2*dy;
                        } 
                        F = F + 2*dx;
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

//I.S. Bentuk Polygon sudah ada
//F.S. Polygon telah diwarnai
//x dan y adalah titik didalam polygon
void fill(int x,int y){

	if((x<1)||(y<1)||(x>1366)||(y>700)){
		return;
	}
	
	long int position = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (y + vinfo.yoffset) * finfo.line_length;
	//Kalau White
	if((*(fbp + position) == -1)){
		return;
	}
	
	else{
		draw_dot(x,y,&white);
		fill(x,y+1);
		fill(x,y-1);
		fill(x+1,y);
		fill(x-1,y);
	}
}

void draw(point* charpoints, char* nama_file, int current_x, int current_y) {
	FILE* charmap;

	charmap = fopen(nama_file, "r");

	int jumlah_loop;
	fscanf(charmap, "%d", &jumlah_loop);
	//printf("Jumlah loop = %d\n", jumlah_loop);


	for (int current_loop = 0; current_loop < jumlah_loop; current_loop++) {
		
		int i = 0;

		for (int j = 0; j < jumlah_maksimal_titik; j++) {
			charpoints[j].absis = 0;
			charpoints[j].ordinat = 0;
		}
		int jumlah_titik;
		fscanf(charmap, "%d", &jumlah_titik);
		//printf("Jumlah titik pada loop %d = %d\n", current_loop, jumlah_titik);
		for (int k = 0; k < jumlah_titik; k++) {
			int x,y;
			fscanf(charmap, "%d  %d", &x, &y);
			//printf("%d %d ", x, y);
			charpoints[k].absis = x + current_x;
			charpoints[k].ordinat = y + current_y;
			//printf("%d %d\n", charpoints[k].absis, charpoints[k].ordinat);
		}
		color white = {
		  255,
		  255,
		  255,
		  0
		};
		int j = 0;
		while (charpoints[j].absis != 0) {
			if (j==jumlah_maksimal_titik) {
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
			j++;
		}
	}
	int x,y;
	fscanf(charmap, "%d %d", &x, &y);
	//printf("Mulai warna dari %d %d", x+current_x, y+current_y);
	fill(x+current_x, y+current_y);
	fclose;
}

int isValid(char* test,int len){
	int i;
	int count = 0;
	for(i=0;i<len;i++){
		if(test[i] == ' '){
			count = 0;
		}
		else{
			count++;
		}
		
		if(count>30){
			return 0;
		}
	}
	return 1;
}

int findSpace(char* test,int len, int start){
	int x = start + 29;
	if (x >= len){
		return len;
	}
	else{
		while(test[x] != ' '){
			x--;
		}
		return x;
	}
}

void draw_huruf(int x, int y, char c){
  // koreksi huruf kecil -> -32
  char c2;
  if (c > 90){
    c2 = c - 32;
  } else {
    c2 = c;
  }
  point charpoints[jumlah_maksimal_titik];
  char inpfile[] = {"A.txt"};
	//draw(charpoints, "F.txt", 0, 0);
	inpfile[0] = c2;
	printf("%s",inpfile);
	draw(charpoints, inpfile, x, y);
}

void draw_kata(int* x, int* y, char* kata, int len){
	int xx = *x;
	int yy = *y;
	int icrx = 60;
	int icry = 60;
	char curr = ' ';
	int i = 0;
    int spacepos = findSpace(kata,len,0);
	while(i < len){
		curr = kata[i];
		if(curr == '\0') break;
		draw_huruf(xx, yy, curr);
		xx += icrx;
		i++;
		
		if(i==spacepos){
			yy += icry;
			*y += icry;
			xx = *x;
			if (spacepos != len){
				i++;
				spacepos = findSpace(kata,len,i);
			}
		}
	}
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
		
		point charpoints[jumlah_maksimal_titik];
		//baca map untuk pixel karakter
		if (pStr[i] == 'A') {
			get_char_points(charpoints, "A.txt", current_x, current_y);
		}
	}
*/
	
	//get_char_points(charpoints, "C.txt", 100, 100);
	//get_char_points(charpoints, "D.txt", 200, 100);
	//draw_line(0,0,100,200,&white);

	point charpoints[jumlah_maksimal_titik];
	//draw(charpoints, "F.txt", 0, 0);
	//draw(charpoints, "A.txt", 100, 0);
	//draw(charpoints, "H.txt", 200, 0);
	//draw(charpoints, "I.txt", 300, 0);
	char input[256];
	scanf("%256[^\n]",&input);
	int y0 = 50; // 50 pixel dari atas biar ga mepet screen
	y = y0;
	//refresh(0, 360, 0, 700);
	if ((strlen(input) <= 256)&&(isValid(input,strlen(input)))){   
		clear_screen(1366, 700);
		draw_kata(&x, &y,input,strlen(input));
	}
	//draw_huruf(0,0,'a');
	munmap(fbp, screensize);

	close(fbfd);

	return 0;

}