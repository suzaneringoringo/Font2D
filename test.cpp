#include <iostream>

using namespace std;

char gambar[32][32];

int main() {
   int x1 = 10;  int y1 = 10;
   int x2 = 20;  int y2 = 30;

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
            gambar[y][x1] = '*';
        }
    }

    //kasus_horizontal 
    else if (y1 == y2) {
        for(int x = x1; x <= x2; x++) {
            gambar[y][x] = '*';
        }
    }

    else {
        float grad = (float)(y2-y1)/(float)(x2-x1);

        //gradien > 0
        if (grad > 0) {
                //gradien <= 1
                if (grad <= 1) {       
                    int dxdy = y2 - y1 + x1 - x2;
                    int F = y2 - y1 + x1 - x2; 
                    for (int x = x1; x <= x2; x++) {
                        gambar[y][x] = '*';
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
                        cout << x << " " << y << endl;
                        gambar[y][x] = '*';
                        if (F < 0) {
                            F += dx;
                        } else {
                            x++;
                            F += dxdy;
                        }
                    }
                }
    
        } else {
                //gradien >= -1
                if (grad >= -1) {
                    y = y1;
                    int dxdy = y2 - y1 + x1 - x2;
                    int F = y2 - y1 + x1 - x2; 
                    for (int x = x1; x <= x2; x++) {
                        cout << x << " " << y << endl;
                        gambar[y][x] = '*';
                        if (F > 0) {
                            F += dy;
                        } else {
                            y--;
                            F += dxdy;
                        }
                    
                    }
                }

                //gradien < -1
                else {
                    x = x2;
                    int dxdy = (y2 - y1 + x1 - x2);
                    int F = -(y2 - y1 + x1 - x2);
                    for (int y = y2; y <= y1; y++) {
                        cout << x << " " << y << endl;
                        gambar[y][x] = '*';
                        if (F > 0) {
                            F += dx;
                        } else {
                            x--;
                        
                            F += dxdy;
                        }
                    } 
                }
        }
   }


   //COBA RUN
   for(int i = 0; i < 32; i++) {
       for(int j = 0; j < 32; j++) {
           cout << gambar[i][j] << " ";
       }
       cout << endl;
   }
}