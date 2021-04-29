/**
 * @file proj1.c
 * @author Tereza Burianova, xburia28
 * @date 10 Nov 2019
 */

#include <stdio.h>
#include <string.h>

#define MAXLINE 102
#define MAXCONTACT 204

int read_contact(char currentName[], char currentNumber[], char currentContact[], unsigned long *stringLength);
void uppercase_to_lowercase(char currentContact[], unsigned long *stringLength);
void convert_to_numbers(char currentContact[], char convertedContact[], unsigned long *stringLength);
void searchContacts(char convertedContact[], char argument[], int *foundContacts, char currentContact[]);

int main(int argc, char *argv[]) {
    char currentNumber[MAXLINE];
    char currentName[MAXLINE];
    char currentContact[MAXCONTACT];
    char convertedContact[MAXCONTACT];
    int foundContacts = 0;
    unsigned long stringLength;
    // Read all lines of the input
    while (fgets(currentName, MAXLINE, stdin) != NULL) {
        // Read second line of the contact, error handling
        if (read_contact(currentName, currentNumber, currentContact, &stringLength) == 1) {
            fprintf(stderr, "Function error!");
            return 1;
        }
        // Error handling for too long lines
        if((strchr(currentNumber, '\n') == NULL) || (strchr(currentName, '\n') == NULL)) {
            fprintf(stderr, "The contact is too long!\n");
            return 1;
        }
        // Contact search if an argument is available
        if(argc > 1) {
            uppercase_to_lowercase(currentContact, &stringLength);
            convert_to_numbers(currentContact, convertedContact, &stringLength);
            searchContacts(convertedContact, argv[1], &foundContacts, currentContact);
        } else {
            // No argument available, print all contacts
            foundContacts = 1;
            fprintf(stdout, "%s", currentContact);
        }
    }
    if(foundContacts == 0 ) {
        fprintf(stdout, "Not found\n");
    }
    return 0;
}

int read_contact(char currentName[], char currentNumber[], char currentContact[], unsigned long *stringLength) {
        if (fgets(currentNumber, MAXLINE, stdin) == NULL)
            return 1;
        // Copy name and number into one array, error handling
        if (sprintf(currentContact, "%.*s, %s",(int)strlen(currentName)-1 , currentName, currentNumber) < 0)
            return 1;
        *stringLength = strlen(currentContact);
	return 0;
    }

void uppercase_to_lowercase(char currentContact[], unsigned long *stringLength) {
    for (unsigned long i = 0; i < (*stringLength); i++) {
        if (((int)currentContact[i] >= 65) && ((int)currentContact[i] <= 90)) {
            currentContact[i] += 32;
        }
    }
}

void convert_to_numbers(char currentContact[], char convertedContact[], unsigned long *stringLength) {
    strcpy(convertedContact, currentContact);
    // Check every character and convert to corresponding number if necessary
    for(unsigned long i = 0; i < (*stringLength); i++) {
        switch (convertedContact[i]) {
            case 'a': case 'b': case 'c':
                convertedContact[i] = '2';
                break;
            case 'd': case 'e': case 'f':
                convertedContact[i] = '3';
                break;
            case 'g': case 'h': case 'i':
                convertedContact[i] = '4';
                break;
            case 'j': case 'k': case 'l':
                convertedContact[i] = '5';
                break;
            case 'm': case 'n': case 'o':
                convertedContact[i] = '6';
                break;
            case 'p': case 'q': case 'r': case 's':
                convertedContact[i] = '7';
                break;
            case 't': case 'u': case 'v':
                convertedContact[i] = '8';
                break;
            case 'w': case 'x': case 'y': case 'z':
                convertedContact[i] = '9';
                break;
            case '+':
                convertedContact[i] = '0';
                break;
        }
    }
}

void searchContacts(char convertedContact[], char argument[], int *foundContacts, char currentContact[]) {
    // Check if the argument is a substring of the contact
    if (strstr(convertedContact, argument) != NULL) {
        fprintf(stdout, "%s", currentContact);
        *foundContacts = *foundContacts + 1;
    }
}
