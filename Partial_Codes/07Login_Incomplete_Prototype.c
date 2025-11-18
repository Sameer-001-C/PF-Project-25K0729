/*At this point, the below code is still missing some few elements. It works by taking the username and password from the user,
making necessary directories for the user. As soon as
*/
#include <direct.h>
#include <io.h>       // <-- needed for _access()
#include <stdio.h>
#include <string.h>

//D:/PF_Project/Client/25K0729/Chats

int main() {
	char clientAUsername[50]; //25K0729
	char password[50];
	char buffer;
	
	printf("Please enter username: "); //25K0729
	scanf("%s", clientAUsername);
    scanf(" %c", &buffer); // consume leftover newline
	printf("Please enter password: ");
	scanf("%s", password);
    scanf(" %c", &buffer); // consume leftover newline	
    char Client_A_UserName[50]; //Client A is our current client, who is currently using this client program
    
    strcpy(Client_A_UserName, clientAUsername);

    //printf("Please enter your name: ");
    //scanf("%s", Client_A_UserName);


    char location[200] = "D:/PF_Project/Client/"; //Location of the current client folder, also the parent directory
    
    strcat(location, Client_A_UserName); //this becomes: D:/PF_Project/Client/25K0729

    // ----------------------------
    // Check if the directory exists
    // ----------------------------
    if (_access(location, 0) == 0) {
        // directory exists
        printf("Directory already exists.\n");
    } 
    else {
        // directory does not exist ? create it
        int status = _mkdir(location); //D:/PF_Project/Client/25K0729

        if (status == 0) {
            printf("Directory created successfully.\n");
        } else {
            perror("Error creating directory\n"); //could also use printf
        }
    }
    
    
    
    //Creating Chats folder for the user:
    
    char path_Chats[200];
    strcpy(path_Chats, location); // D:/PF_Project/Client/25K0729
    strcat(path_Chats, "/Chats"); // D:/PF_Project/Client/25K0729/Chats
    
    if (_access(path_Chats, 0) == 0) { //checking if D:/PF_Project/Client/25K0729/Chats exists
    // directory exists
    printf("Directory already exists.\n");
    } 
    else {
        // directory does not exist ? create it
        int status = _mkdir(path_Chats); // D:/PF_Project/Client/25K0729/Chats

        if (status == 0) {
            printf("Directory created successfully.\n");
        } else {
            perror("Error creating directory\n"); //could also use printf
        }
    }    
    
    /*
    char Contact_Path[50];
    strcpy(Contact_Path, location); //D:/PF_Project/Client/Chats/25K0729
    strcpy(Contact_Path, "/All_Contacts"); //D:/PF_Project/Client/Chats/25K0729/All_Contacts
    
    if (_access(Contact_Path, 0) == 0) { //checking if D:/PF_Project/Client/Chats/25K0729/All_Contacts exists
        // directory exists
        printf("Directory already exists.\n");
    } 
    else {
        // directory does not exist ? create it
        int status = _mkdir(Contact_Path); //D:/PF_Project/Client/Chats/25K0729/All_Contacts

        if (status == 0) {
            printf("Directory created successfully.\n");
        } else {
            perror("Error creating directory\n"); //could also use printf
        }
    }    
    */
    

	//At this point, D:/PF_Project/Client/25K0729/Chats is present/exists, stored in path_Chats
	
	char path_Contacts[200];
	strcpy(path_Contacts, path_Chats); // D:/PF_Project/Client/25K0729/Chats
	strcat(path_Contacts, "/Address.txt"); // D:/PF_Project/Client/25K0729/Chats/Address.txt

	FILE *fptr;
	
	fptr = fopen(path_Contacts, "a"); //just to create a file named Address.txt in the Chats section
	fclose(fptr);
	

	//All of the above is part of logging in
	
	
	
	
	
//The Code below should probably be in a loop:
	
    char Friend_UserID[50];
    printf("Please enter userID of friend: "); //25K0660
    scanf("%s", Friend_UserID);
    scanf(" %c", &buffer);
    
    //First create a new textfile if it doesn't already exists:
    char Friend_Location[200];
	strcpy(Friend_Location, path_Chats); // D:/PF_Project/Client/25K0729/Chats
	strcat(Friend_Location, "/"); // D:/PF_Project/Client/25K0729/Chats/
	strcat(Friend_Location, Friend_UserID); // D:/PF_Project/Client/25K0729/Chats/25K0660
	strcat(Friend_Location, ".txt"); // D:/PF_Project/Client/25K0729/Chats/25K0660.txt

	fptr = fopen(Friend_Location, "a"); //immediately create, if not already existing
	fclose(fptr);
	
	fptr = fopen(path_Contacts, "r");
	
	char line[200];
	int found = 0;
	
	// --- Check if username already exists in address.txt---
	if (fptr != NULL) {
	    while (fgets(line, sizeof(line), fptr)) {
	        // remove newline from line if present
	        line[strcspn(line, "\n")] = '\0';
	
	        if (strcmp(line, Friend_UserID) == 0) {
	            found = 1;
	            break;
	        }
	    }
	    fclose(fptr);
	}
	
	// --- If not found, append Address.txt. Add the username as the Contact Name, after adding him as a new friend ---
	if (!found) {
	    fptr = fopen(path_Contacts, "a");
	    if (fptr != NULL) {
	        fprintf(fptr, "%s\n", Friend_UserID);
	        fclose(fptr);    
	    }
	} 
	

//The loop should probably end here maybe?	
	//FILE *fptr;

	fptr = fopen(Friend_Location,"a");// open file for writing at the location: D:/PF_Project/Client/Chats/25K0729/25K0660.txt
	
	char message[256];
	
	printf("You: ");
	fgets(message, sizeof(message), stdin);

	// remove newline
	message[strcspn(message, "\n")] = '\0';
	
	fprintf(fptr, "You: %s\n", message); //maybe use fputs()?
	
	fclose(fptr);
	
    return 0;
}
