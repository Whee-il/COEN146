#include <stdio.h>
#include <stdlib.h>

int main(){
    int RTT0 = 3;
    int RTT1 = 20;
    int RTT2 = 26;
    int RTTHTTP = 47;


    int a = RTT0 + RTT1 + RTT2 + 2*RTTHTTP;
    int b = a + 12*RTTHTTP;

    int n = 5;

    int c1,c2;
    if(6%n == 0){
        c1 = a + ((6/n))*RTTHTTP;
        c2 = a + 2*((6/n))*RTTHTTP;
    }
    else{
        c1 = a + ((6/n)+1)*RTTHTTP;
        c2 = a + 2*((6/n)+1)*RTTHTTP;
    }

    printf("A is %i\n", a);
    printf("B is %i\n", b);
    printf("C1 is %i\n", c1);
    printf("C2 is %i\n", c2);
}