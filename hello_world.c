#include <stdio.h>
#include <math.h>

int add_together(int x, int y) {
   int result = x + y;
   return result;
}

typedef struct {
    float x;
	float y;
} point;

void double_digit_checker(int x) {
	if (x > 9 && x < 100) {
		puts("x is two digit number!");
	} else {
		puts("x is not a two digit number!");
	}
}

void while_looper(int i) {
	while(i > 0) {
		puts("While loop iteration");
		i = i - 1;
    }
}

void for_looper(int i) {
	for(int j = 0; j < i; j++) {
		puts("For loop iteration");
	}
}
 
int main (int argc, char** argv) {
    puts("Hello lispy world!");
	int added = add_together(3, 5);
    printf("%d\n", added);
    point p;
    p.x = 0.1;
    p.y = 10.0;
    float length = sqrt(p.x * p.x + p.y * p.y);
    printf("Length: %.6f\n", length);
    double_digit_checker(15);
    while_looper(5);
	for_looper(4);
    return 0;
}

