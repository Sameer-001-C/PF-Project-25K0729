//This function checks whether or not a given message containing the username and password, is present inside a textfile named "User_Details.txt" or not
//Format: <Server_Code><UserName><Password> ---> This is what would have been stored inside the buffer
//Example: "125K0729sameer123" ---> stored in buffer, correctly null terminated at the end
//The "1" at the start here represents the Server_Code
//"bytes" basically represents the length of buffer, without the Null Terminator
//So, in this case, bytes would have a value of 17

int Logging_In(char *buffer, int bytes){ //return 0 if login unsuccessful, 1 if successful
    char UserName_And_Password[100]; //format eg: 25K0729sameer123
    strncpy(UserName_And_Password, buffer + 1, bytes); //leaves the first and copies the rest
    FILE *fptr;
    fptr = fopen("D:/PF_Project/Server/Users/User_Details.txt", "r"); //search for that in this file
    char line[256];
    while (fgets(line, sizeof(line), fptr) != NULL) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, UserName_And_Password) == 0) {
            fclose(fptr);
            return 1;   //Username was found
        }
    }
    fclose(fptr);
    return 0; // Did not find username
}
// Use it like this: int Login_Status = Logging_In(buffer, bytes);
