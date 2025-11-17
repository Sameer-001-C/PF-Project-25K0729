#include <direct.h>
#include <io.h>       // <-- needed for _access()
#include <stdio.h>
#include <string.h>

int main() {
	char clientAUsername[50]; //25K0729
	char password[50];
	char buffer;
	
	printf("Please enter username: ");
	scanf("%s", clientAUsername);
    scanf("%c", &buffer); // consume leftover newline	
	printf("Please enter password: ");
	scanf("%s", password);
    scanf("%c", &buffer); // consume leftover newline	
    char Client_A_UserName[50]; //Client A is our current client using this client program
    
    strcpy(Client_A_UserName, clientAUsername);

    //printf("Please enter your name: ");
    //scanf("%s", Client_A_UserName);


    char location[100] = "D:/PF_Project/Client/Chats/";
    strcat(location, Client_A_UserName); //this becomes: D:/PF_Project/Client/Chats/25K0729

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
            perror("Error creating directory\n"); //could also use printf
        }
    }
    
    char Friend_UserID[50];
    printf("Please enter userID of friend: "); //25K0660
    scanf("%s", Friend_UserID);
    scanf("%c", &buffer);
    
    char Friend_Location[50];
	strcpy(Friend_Location, location); //D:/PF_Project/Client/Chats/25K0729
	strcat(Friend_Location, "/"); //D:/PF_Project/Client/Chats/25K0729/
	strcat(Friend_Location, Friend_UserID); //D:/PF_Project/Client/Chats/25K0729/25K0660
	strcat(Friend_Location, ".txt"); //D:/PF_Project/Client/Chats/25K0729/25K0660.txt
	
	FILE *fptr;

	fptr = fopen(Friend_Location,"a");// open file for writing at the location: D:/PF_Project/Client/Chats/25K0729/25K0660.txt
	
	char message[256];
	
	printf("You: ");
	scanf("%s", message);
	scanf("%c", &buffer);
	
	fprintf(fptr, "You: %s\n", message);
	
	fclose(fptr);
	
    return 0;
}
