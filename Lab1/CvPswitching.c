// Name: Will Cockrum 
// Date: 4/1/21
// Title: Lab1 – Circuit Switching vs Packet Switching 
// Description: Calculates various variables related to Circuit vs Packet switching

#include <stdio.h>      /* printf, stderr */
#include <math.h>       /* pow */

double factorial(int init){
    int i = 1;
    double val = 1;
    for (i = init; i > 1; --i){
        val *= i;
    }
    return val;
}

/* main function with command-line arguments to pass */
int main(int argc, char *argv[]) {
    int linkBandwidth, userBandwidth, nCSusers, nPSusers;
    double tPSuser, pPSusersBusy, pPSusersNotBusy;

    linkBandwidth = 200;
    userBandwidth = 20;
    tPSuser  = 0.10;
    nPSusers = 19;

    nCSusers = linkBandwidth/userBandwidth;
    printf("Number of CS Users: %d\n", nCSusers);

    pPSusersBusy = tPSuser;
    printf("Probability a PS user is busy transmitting: %f\n",pPSusersBusy);

    pPSusersNotBusy = 1 - pPSusersBusy;
    printf("Probability a PS user is not busy transmitting: %f\n",pPSusersNotBusy);

    double c = pow(pPSusersNotBusy, nPSusers - 1);
    printf("Probability 18 PS users are not busy transmitting: %f\n",c);

    double d = pPSusersBusy * c;
    printf("Probability one user is busy transmitting: %.3e\n", d);

    double e = nPSusers * d;
    printf("Probability any one user is busy transmitting: %.3e\n", e);

    double f = pow(pPSusersBusy,10) * pow(pPSusersNotBusy, nPSusers - 10);
    printf("Probability ten users are busy transmitting: %.3e\n", f);

    double g = f * (factorial(nPSusers)/ factorial(10) /factorial(nPSusers - 10));
    printf("Probability any ten users are busy transmitting: %.3e\n", g);

    int count = 11;
    double h = 0;
    for (count; count < 20; ++count){
        double countUsers = pow(pPSusersBusy, count) * pow(pPSusersNotBusy, nPSusers - count);
        double combos = factorial(nPSusers)/ factorial(count) /factorial(nPSusers - count);
        h += countUsers * combos;
    }
    printf("Probability more than ten users are busy transmitting: %.3e\n", h);

}