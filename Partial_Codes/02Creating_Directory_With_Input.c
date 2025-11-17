//take user's name (student ID, eg 25K0729) as an input and create a folder with that name in the directory: D:/PF_Project/Client/Chats/
//This directory would store the chats of the client 25K0729

#include <direct.h>
#include <stdio.h>
#include <string.h>

int main() {
	
	char Client_A_UserName[50];
	char buffer;
	printf("Please enter your name: ");
	scanf("%s", Client_A_UserName);
	
	scanf("%c", &buffer);
	
	char location[100] = "D:/PF_Project/Client/Chats/";
	strcat(location, Client_A_UserName);
    int status = _mkdir(location);

    if (status == 0) {
        printf("Directory created successfully.\n");
    } else {
        perror("Error creating directory");
    }

    return 0;
}
