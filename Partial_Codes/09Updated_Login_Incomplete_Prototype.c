/*At this point, the below code is still missing some few elements. It works by taking the username and password from the user,
making necessary directories for the user. As soon as you add a new friend, the username of that friend is added to address.txt.

address.txt basically stores the friends Usernames/UserID inside it, for the particular client.
*/

// Same code as 07Login_Incomplete_Prototype.c, just with more consistent variable names and flow.
// Added creation of directories if not already existing

#include <direct.h>
#include <io.h>       // for _access()
#include <stdio.h>
#include <string.h>

// Program: Simple chat system prototype
// Folder structure example: D:/PF_Project/Client/25K0729/Chats

int main() {
    // ============================
    // LOGIN INFORMATION
    // ============================
    char client_username[50];   // Current client username (e.g., 25K0729)
    char password[50];          // Password input
    char temp_char;             // Temporary char to consume leftover input

    printf("Please enter username: ");
    scanf("%s", client_username);
    scanf("%c", &temp_char);  // Consume leftover newline

    printf("Please enter password: ");
    scanf("%s", password);
    scanf("%c", &temp_char);  // Consume leftover newline
    
    
    
    
    // Wait for Server's Reply
    // If Server disapproves login, reject from doing all the other things below, or re ask for login, or end by printing unsuccess message
    // If Server approves login, do the following:




    // ============================
    // CLIENT FOLDER SETUP
    // ============================
    if (_access("D:/PF_Project", 0) != 0) { //If directory does not exist, make it : D:/PF_Project
        int status = _mkdir("D:/PF_Project");
        if (status == 0) {
            printf("Directory created successfully.\n");
        } else {
            perror("Error creating directory\n");
        }
    }
    if (_access("D:/PF_Project/Client", 0) != 0) { //If directory does not exist, make it : D:/PF_Project/Client
        int status = _mkdir("D:/PF_Project/Client");
        if (status == 0) {
            printf("Directory created successfully.\n");
        } else {
            perror("Error creating directory\n");
        }
    }    
    
	// At this stage, we are sure that the directory "D:/PF_Project/Client" is present
    char client_dir_path[200] = "D:/PF_Project/Client/";
    strcat(client_dir_path, client_username);  // Full path to client folder (eg: D:/PF_Project/Client/25K0729)

    // Check if client directory exists
    if (_access(client_dir_path, 0) == 0) {
        printf("Directory already exists.\n");
    } else {
        int status = _mkdir(client_dir_path);
        if (status == 0) {
            printf("Directory created successfully.\n");
        } else {
            perror("Error creating directory\n");
        }
    }

    // ============================
    // CHATS FOLDER SETUP
    // ============================
    char chats_dir_path[200];
    strcpy(chats_dir_path, client_dir_path); // D:/PF_Project/Client/25K0729
    strcat(chats_dir_path, "/Chats"); // D:/PF_Project/Client/25K0729/Chats

    // Check if Chats folder exists
    if (_access(chats_dir_path, 0) == 0) { // D:/PF_Project/Client/25K0729
        printf("Directory already exists.\n");
    } else {
        int status = _mkdir(chats_dir_path); // D:/PF_Project/Client/25K0729
        if (status == 0) {
            printf("Directory created successfully.\n");
        } else {
            perror("Error creating directory\n");
        }
    }

    // ============================
    // ADDRESS BOOK FILE SETUP
    // ============================
    char contacts_file_path[200];
    strcpy(contacts_file_path, chats_dir_path); // D:/PF_Project/Client/25K0729
    strcat(contacts_file_path, "/Address.txt"); // D:/PF_Project/Client/25K0729/Address.txt

    // Create Address.txt if it does not exist
    FILE *fptr = fopen(contacts_file_path, "a");
    fclose(fptr);
    // -------------------- LOGIN SETUP COMPLETE --------------------




    // ============================
    // ADD FRIEND + CHAT
    // ============================
    
    
    // ALSO, ADD A FRIEND WHEN SOMEONE SENDS A MESSAGE TO OUR CLIENT
    char friend_user_id[50];   // Friend's username / ID
    printf("Please enter userID of friend: ");
    scanf("%s", friend_user_id); // example: 25K0660

    // Clear leftover input
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    // Build path for friend's chat file
    char friend_chat_file_path[200];
    strcpy(friend_chat_file_path, chats_dir_path); // D:/PF_Project/Client/25K0729
    strcat(friend_chat_file_path, "/"); // D:/PF_Project/Client/25K0729/
    strcat(friend_chat_file_path, friend_user_id); // D:/PF_Project/Client/25K0729/25K0660
    strcat(friend_chat_file_path, ".txt"); // D:/PF_Project/Client/25K0729/25K0660.txt

    // Create friend's chat file if it does not exist
    fptr = fopen(friend_chat_file_path, "a"); // D:/PF_Project/Client/25K0729/25K0660.txt
    fclose(fptr);

    // ============================
    // UPDATE ADDRESS BOOK
    // ============================
    
    // Checking If a Friend Already Exists In Contacts in Address.txt
    fptr = fopen(contacts_file_path, "r");

    char line_buffer[200];
    int found = 0;

    if (fptr != NULL) {
        while (fgets(line_buffer, sizeof(line_buffer), fptr)) {
            // Remove newline if present
            line_buffer[strcspn(line_buffer, "\n")] = '\0';

            if (strcmp(line_buffer, friend_user_id) == 0) {
                found = 1; //Reset to 0 if we want to reuse it
                break;
            }
        }
        fclose(fptr);
    }

    // Append friend to Address.txt if not already there
    if (!found) {
        fptr = fopen(contacts_file_path, "a");
        if (fptr != NULL) {
            fprintf(fptr, "%s\n", friend_user_id);
            fclose(fptr);
        }
    }

    // ============================
    // SEND MESSAGE TO FRIEND
    // ============================
    fptr = fopen(friend_chat_file_path, "a");

    char message[256];
    printf("You: ");
    fgets(message, sizeof(message), stdin);

    // Remove newline from message
    message[strcspn(message, "\n")] = '\0';

    fprintf(fptr, "You: %s\n", message);
    fclose(fptr);

    return 0;
}
