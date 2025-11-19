//To create a directory for server
//Also, creates a text file named "User_Details.txt"
//"User_Details.txt" will contain the ID and Password of the clients, in a single string
//Example:
//  UserID: 25K0729
//  Password: sameer123
//so, in "User_Details.txt", 25K0729sameer123 will be added.

void Make_Initial_Directory(void){ //upon starting server
    if (_access("D:/PF_Project", 0) != 0) {
        // directory does not exist ? create it
        int status = _mkdir("D:/PF_Project");

        if (status == 0) {
            printf("Directory created successfully.\n");
        } else {
            perror("Error creating directory"); //could also use printf
        }
    }
    if (_access("D:/PF_Project/Server", 0) != 0) {
        // directory does not exist ? create it
        int status = _mkdir("D:/PF_Project/Server");

        if (status == 0) {
            printf("Directory created successfully.\n");
        } else {
            perror("Error creating directory"); //could also use printf
        }
    }
    if (_access("D:/PF_Project/Server/Users", 0) != 0) {
        // directory does not exist ? create it
        int status = _mkdir("D:/PF_Project/Server/Users");

        if (status == 0) {
            printf("Directory created successfully.\n");
        } else {
            perror("Error creating directory"); //could also use printf
        }
    }
    FILE *fptr;
    fptr = fopen("D:/PF_Project/Server/Users/User_Details.txt", "a");
    fclose(fptr);
}
//Use like this: Make_Initial_Directory();
