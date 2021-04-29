/**
 * @file proj2.c
 * @author Tereza Burianova, xburia28
 * @date 24 Nov 2019
 * @brief A diode voltage and current calculator.
 *
 * The program calculates the voltage and current of a diode in a diode-resistor circuit.
 * The arguments (double values) are the source voltage in volts (u0), the resistor resistance in ohms (r), the needed accuracy in the bisection method (eps)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#define I0 1e-12
#define Ut 25.8563e-3

int argumentsValidity(char *conversionErr1, char *conversionErr2, char *conversionErr3, double u0, double r, double eps);
double diode(double u0, double r, double eps);
double equation(double u0, double r, double x);

int main(int argc, char *argv[]) {
    // check the amount of arguments
    if (argc != 4) {
        fprintf(stderr, "3 arguments are needed to run the program (u0, r, eps).\n");
        return 1;
    }
    // convert arguments to double data type
    char *err1, *err2, *err3;
    double u0 = strtod(argv[1], &err1);
    double r = strtod(argv[2], &err2);
    double eps = strtod(argv[3], &err3);
    // error handling for entered arguments
    if (argumentsValidity(err1, err2, err3, u0, r, eps) == 1)
        return 1;
    double Up = diode(u0, r, eps);
    // Shockley diode equation used to calculate the diode current
    double Ip = I0*(exp(Up/Ut)-1);
    fprintf(stdout, "Up=%g V\nIp=%g A\n", Up, Ip);
    return 0;
}

/* Function argumentsValidity:
 * Arguments: int numOfArguments (number of entered arguments), char *conversionErr1, char *conversionErr2, char *conversionErr3 (strtod error handling), double u0 (source voltage), double r (resistor resistance), double eps (needed accuracy)
 * Return value: 1 for error, 0 for success
 * Function: Checks the validity of arguments entered by a user.
 */
int argumentsValidity(char *conversionErr1, char *conversionErr2, char *conversionErr3, double u0, double r, double eps) {
    if (*conversionErr1 != '\0' || *conversionErr2 != '\0' || *conversionErr3 != '\0') {
        fprintf(stderr, "The arguments are not valid! (type double)\n");
        return 1;
    }
    if (errno == ERANGE) {
        fprintf(stderr, "One of the arguments is too long for the used data type (double)!\n");
        return 1;
    }
    if (u0 < 0 || r <= 0 || eps <= 0) {
        fprintf(stderr, "The values must not be negative (or 0 for R and eps)!\n");
        return 1;
    }
    return 0;
}

/* Function equation:
 * Arguments: double u0 (source voltage), double r (resistor resistance), double x (estimated value of possible Up)
 * Return value: double
 * Function: Calculates the difference between Ip (diode current) and Ir (resistor current) used in the bisection method.
 */
double equation(double u0, double r, double x) {
    return (I0*(exp(x/Ut)-1)-(u0-x)/r);
}

/* Function diode:
 * Arguments: double u0 (source voltage), double r (resistor resistance), double eps (needed accuracy)
 * Return value: double
 * Function: Calculates the estimated value of Up (diode voltage) using the bisection method.
 */
double diode(double u0, double r, double eps) {
    double a = 0;                                   // left boundary
    double b = u0;                                  // right boundary
    double middle = (a+b)/2;
    double prev_middle = middle;
    int iterations = 0;
    while(fabs(b-a) > eps) {
        if((equation(u0, r, a) * equation(u0, r, middle)) < 0) {    // negative value - solution is located in the left half of the interval
            b = middle;
        } else {                                                    // solution is located in the right half of the interval
            a = middle;
        }
        if (fabs(b-a) > eps) {
            prev_middle = middle;
            middle = (a + b) / 2;
        }
        iterations++;
        if (prev_middle == middle)
            break;
    }
    return middle;                                  // estimated value of Up
}