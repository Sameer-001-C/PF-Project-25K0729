//This code must run on client side upon logging in.
//User input for Client_A_UserName would change to the variable that is the current client's name.
//In the directory that is created, will the chats of that particular client be stored in.

#include <direct.h>
#include <io.h>       // <-- needed for _access()
#include <stdio.h>
#include <string.h>

int main() {
    char Client_A_UserName[50];
    char buffer;

    printf("Please enter your name: ");
    scanf("%s", Client_A_UserName);

    scanf("%c", &buffer); // consume leftover newline

    char location[100] = "D:/PF_Project/Client/Chats/";
    strcat(location, Client_A_UserName);

    // ----------------------------
    // Check if the directory exists
    // ----------------------------
    if (_access(location, 0) == 0) {
        // directory exists
        printf("Directory already exists.\n");
    } 
    else {
        // directory does not exist ? create it
        int status = _mkdir(location);

        if (status == 0) {
            printf("Directory created successfully.\n");
        } else {
            perror("Error creating directory"); //could also use printf
        }
    }

    return 0;
}
