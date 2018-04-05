#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_X   20
#define MAX_Y   20

#define TRAJECTORY_TOLERANCE 0.25

#define PI 3.14159

struct player{
    uint8_t X;
    uint8_t Y;
}p1, p2, p3;

void print_map(struct player p1, struct player p2, struct player p3){
    for(int i=MAX_X; i>-1; i--){
        for(int j=0; j<MAX_Y + 1; j++){
            if((i == 0) || (j == 0) || (i == MAX_X) || (j == MAX_Y)){
                printf(" X");
            }

            else if((p1.X - 1 == j) && (p1.Y -1 == i))
                printf(" 1");
            
            else if((p2.X - 1 == j) && (p2.Y -1 == i))
                printf(" 2");
            
            else if((p3.X - 1 == j) && (p3.Y -1 == i))
                printf(" 3");
            
            else
                printf("  ");
        }
        printf("\n");
    }
    printf("\n\n");
}


uint8_t is_in_trajectory(int id1, int id2, struct player me, float my_theta, struct player you){
    printf("%i: %i,%i firing at %i: %i,%i with trajectory: %3.3f\n", id1, me.X, me.Y, id2, you.X, you.Y, my_theta);
    float temp_theta = atan2(you.Y - me.Y, you.X - me.X) * 180 / PI;
    printf("calcd theta: %3.3f\n", temp_theta);
    float temp_distance = sqrt((you.Y-me.Y)*(you.Y-me.Y) + (you.X-me.X)*(you.Y-me.Y));
    printf("distance: %3.3f tolerance: %3.3f\n", temp_distance, temp_distance * TRAJECTORY_TOLERANCE);
    float theta_tolerance = temp_distance*TRAJECTORY_TOLERANCE;


    if((temp_theta < my_theta + theta_tolerance) && (temp_theta > my_theta - theta_tolerance))
        return 1;
    else
        return 0;

}

int main(){

    p1.X = 2;
    p1.Y = 2;

    p2.X = 17;
    p2.Y = 4;

    p3.X = 13;
    p3.Y = 18;

    print_map(p1, p2, p3);

    uint8_t ret = is_in_trajectory(1, 3, p1, 55.0, p3);
    if(ret)
        printf("HIT!\n");
    else
        printf("MISS!\n");

    ret = is_in_trajectory(3, 1, p3, -200.0, p1);
    if(ret)
        printf("HIT!\n");
    else
        printf("MISS!\n");

    return 0;
}
