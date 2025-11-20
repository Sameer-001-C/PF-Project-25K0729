//This works! Finally.

/*Server Duties:

    Handle Requests From Clients:
        1. Server Codes:
            * If Server Code = 1, that means Client is trying to login to an existing account.
            * If Server Code = 2, that means Client is trying to create a new account.
            * If Server Code = 3, that means Client is trying to add new friend.
            * If Server Code = 4, that means Client is trying to send a message to someone.
            * If Server Code = 5, that means Client is trying to log out.

        2. Login:
            * Client requests login to existing account:

                - Server is in a listening state
                - Server recieves a message
                - Server decodes the message and extracts the server code
                - Server Code = 1
                - Check total online count
                - If total online count exceeds maximum users:
                    reject login by sending a client code
                    End connection ---> closesocket()
                - Else:
                - Recieve UserID and Password
                - Check UserID and Password in Server's DataBase (textfile):

                - If (Correct details):
                    1. Increment Online Count   (variable)
                    2. Add Usename to the structure variable
                    3. Assign a port to the user   (stored in a variable, structure)
                    4. Send a validation message to the client of login approval
                    5. End connection ---> closesocket()
                - Else:
                    1. Send a validation message to the client of login rejection
                    2. End connection ---> closesocket()

            * Client requests creation of new account
            
                - Server is in a listening state
                - Server recieves a message
                - Server decodes the message and extracts the server code
                - Server Code = 2
                - Recieve UserID and Password
                - Validate the format of the UserId (for directory creation purposes):
                - Wrong format also includes username exceeding 7 characters
                - If wrong format
                    reject login by sending a client code (wrong username format, please enter again)
                    End connection ---> closesocket()
                - If validation complete, check maximum online count
                - If maximum online count exceeds maximum clients:
                    reject login by sending a client code (maximum login exceeded)
                    End connection ---> closesocket()
                - Else:
                    1. Store UserID and Password in Server's database (textfile)
                    2. Increament online count
                    3. Add Username to the structure variable
                    4. Assign a port to the user   (stored in a variable, structure)
                    5. Send a validation message to the client of login approval
                    6. End connection ---> closesocket()

        3. Adding a new friend:

            - Server is in a listening state
            - Server recieves a message
            - Server decodes the message and extracts the server code
            - Server Code = 3
            - Server extract the Username of the friend
            - Server checks the Username of the friend in it's database (textfile)
            - IF (username found):
                Send a client code that the user has been found
                End connection ---> closesocket()
            - ELSE
                Send a client code that the user has not been found
                End connection ---> closesocket()

        4. Client wants to send a message to someone:

            - Server is in a listening state
            - Server recieves a message 
                -> first 15 characters are reserved for Server_Code, Sender_Username, and Reciever_Username
            - Server decodes the message and extracts the server code
            - Server Code = 4
            - Server extracts the "To" (UserID) and "From" (UserID) from the message, along with the message itself
            - Server checks if the "To" is online or not   ---> "To" = Client B, "From" = Client A
            - IF (Client B is not online):
                Send a client code to client A that the person is offline
                End connection ---> closesocket()
            - ELSE (Client B is online):
                Connect with client B
                Send a client code to client B
                Forward the message to client B
                End connection ---> closesocket()

        5. Logout

            - Server is in a listening state
            - Server recieves a message
            - Server decodes the message and extracts the server code
            - Server Code = 5
            - Server searches for the user by matching his username
            - Server romoves everything about the user
            - Server decreases the online count
            - End connection ---> closesocket()        


    Client Codes:
        1. If Login is successful, send Client Code = 1
        2. If Login is unsuccessful due to wrong credentials, send Client Code = 2
        3. If Login is unsuccessful due to maximum online users reached, send Client Code = 3
        4. If client wants to add a friend, and, friend exists, send Client Code = 4
        5. If client wants to add a friend, but, no such friend exists, send Client Code = 5
        6. If client wants to send a message to a friend, but, friend is offline, send Client Code = 6
        7. If client recieves a message from someone
        8. If creating a new account was unsuccessful due to username already existing
        9. If the sender's message was delivered to the reciever successfully.

*/


#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <direct.h>
#include <io.h>       // for _access()

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 4



//Add here only when loggin in or out
struct Client_Info{
    char UserID[8]; //example: 25K0729 ---> size is 8 cuz of Null Terminator in the end
    char IP_Address[16];
};

char Extract_Server_Code(char *buffer){
    char Server_Code = buffer[0];
    return Server_Code;
}
// Use it like this: Server_Code = Extract_Server_Code(buffer)



void Extract_Sender_Username(char *buffer, char *Sender_Username_Out)
{
    // Copy 7 characters starting from message[1]
    for (int i = 0; i < 7; i++) {
        Sender_Username_Out[i] = buffer[i + 1];
    }

    // Add null terminator at the end
    Sender_Username_Out[7] = '\0';
}
// Use it like this: Extract_Sender_Username(buffer, Sender_Username);   ---> automatically stores in Sender_Username



void Extract_Reciever_Username(char *buffer, char *Reciever_Username_Out)
{
    // Copy 7 characters starting from message[8]
    for (int i = 0; i < 7; i++) {
        Reciever_Username_Out[i] = buffer[i + 8];
    }

    // Add null terminator at the end
    Reciever_Username_Out[7] = '\0';
}
// Use it like this: Extract_Reciever_Username(buffer, Reciever_Username);   ---> automatically stores in Reciever_Username



void Extract_Chat_Text(char *buffer, char *Chat_Text_Out, int buffer_length) // Extracts the chat text from the buffer
{
    // Chat text starts from index 15
    int chat_start = 15;

    if (buffer_length <= chat_start) {
        // No chat text available
        Chat_Text_Out[0] = '\0';
        return;
    }

    // Copy the rest of the message into Chat_Text_Out
    int chat_length = buffer_length - chat_start;

    // Be careful: Chat_Text_Out should be large enough to hold chat_length + 1
    memcpy(Chat_Text_Out, buffer + chat_start, chat_length);

    // Null-terminate the string
    Chat_Text_Out[chat_length] = '\0';
}
// Use it like this: Extract_Chat_Text(buffer, message, bytes);



void Make_Initial_Server_Directory(void){ //upon starting server
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



int Logging_In(char *buffer, int bytes, int Online_Count){ //return 0 if login unsuccessful
    if(Online_Count > MAX_CLIENTS){
        return 3; //3 means max clients reached
    }
    char UserID_And_Password[24]; //format eg: 125K0729sameer123 ---> 7 chars for UserID and max 16 for password
    strncpy(UserID_And_Password, buffer + 1, bytes - 1); //leaves the first and copies the rest
    UserID_And_Password[bytes - 1] = '\0';
    FILE *fptr;
    fptr = fopen("D:/PF_Project/Server/Users/User_Details.txt", "r"); //search for that in this file
    char line[256];
    while (fgets(line, sizeof(line), fptr) != NULL) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, UserID_And_Password) == 0) {
            fclose(fptr);
            return 1;   //Username was found, means 1, successful
        }
    }
    fclose(fptr);
    return 2; // Did not find username
}
// Use it like this: int Login_Status = Logging_In(buffer, bytes, Online_Count);



int Creating_New_Account(char *buffer, int bytes, int Online_Count){ //Buffer format: <Server_Code><UserID><Passoword>
    if(Online_Count > MAX_CLIENTS){
        return 3; //3 means max clients reached
    }
    char UserID_And_Password[24]; //format eg: 125K0729sameer123 ---> 7 chars for UserID and max 16 for password
    strncpy(UserID_And_Password, buffer + 1, bytes - 1); //leaves the first and copies the rest
    UserID_And_Password[bytes - 1] = '\0';
    char UserID[8];
    strncpy(UserID, buffer + 1, 7); //Fixed 7 length value
    UserID[7] = '\0';
    FILE *fptr;
    fptr = fopen("D:/PF_Project/Server/Users/User_Details.txt", "r"); //search for that in this file
    char line[256];
    char First_Seven_Characters[8];
    while (fgets(line, sizeof(line), fptr) != NULL) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        strncpy(First_Seven_Characters, line, 7); //Format for User_Details.txt: <UserID><Passoword>
        First_Seven_Characters[7] = '\0';
        if (strcmp(First_Seven_Characters, UserID) == 0) {
            fclose(fptr);
            return 8;   //Username was found, means username was taken and already exists
        }
    }
    fclose(fptr);
    fptr =  fopen("D:/PF_Project/Server/Users/User_Details.txt", "a"); //UserName was not found in User_Details.txt, so add a new account
    fprintf(fptr, "%s", UserID_And_Password);
    fprintf(fptr, "\n");
    fclose(fptr);
    return 1; //successful login by creating a new account
}



int Find_Online_User_Index(char *Reciever_Username, struct Client_Info *Client, int Online_Count){ //return -1 if offline
    int i;
    for(i = 0; i < Online_Count; i++){
        if(strcmp(Reciever_Username, Client[i].UserID) == 0){
            return i; //if found
        }
    }
    return -1; //if not found
}



int Add_New_Friend(char *Friend_User_ID){
    char Friend_ID[8];
    strcpy(Friend_ID, Friend_User_ID);
    FILE *fptr;
    fptr = fopen("D:/PF_Project/Server/Users/User_Details.txt", "r");
    char line[256];
    char First_Seven_Characters[8];
    while (fgets(line, sizeof(line), fptr) != NULL) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        strncpy(First_Seven_Characters, line, 7); //Format for User_Details.txt: <UserID><Passoword>
        First_Seven_Characters[7] = '\0';
        if (strcmp(First_Seven_Characters, Friend_ID) == 0) {
            fclose(fptr);
            return 4;   //Username was found, means client can add him as a friend
        }
    }
    fclose(fptr);
    return 5; //client wants to add a friend but no such friend exists
}








int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char Server_Code;
    char reply[BUFFER_SIZE];

    char Sender_Username[8]; //Client A   ---> always 7 characters long, plus null terminator (25K0729)
    char Reciever_Username[8]; //Client B   ---> always 7 characters long, plus null terminator (25K0729)

    int bytes_sent = 0;

    int Online_Count = 0;

    struct Client_Info Client[MAX_CLIENTS];

    Make_Initial_Server_Directory();

    //1. Initializing Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    //2. Creating a server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Could not create socket.\n");
        WSACleanup();
        return 1;
    }

    //3. Binding for server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    //4. Starting to listen
    listen(server_socket, 5);
    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_len = sizeof(client_addr);

        //accepting a connection and establishing the link
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len); //keep opening and closing this socket to keep serving clients
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed.\n");
            continue;
        }

        //Getting Client IP
        char *client_ip = inet_ntoa(client_addr.sin_addr);
        printf("\nConnection accepted from IP: %s\n", client_ip);



        //Recieving the message request here
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0); //Recieving a message into buffer, and returning number of bytes recieved
        if (bytes > 0) {
            buffer[bytes] = '\0';
            //printf("Received: %s\n", buffer); commented out, maybe need to use this?
            bytes_sent = 0;


            //My part of the code logic and structure:
            char reply[2];
            Server_Code = Extract_Server_Code(buffer);
            switch(Server_Code){
                case '1': //logging in to an existing account ---> Will also store the IP Address. Format: <Server_Code><UserID><Password>
                    printf("Serving Case 1.\n");
                    int Login_Status = Logging_In(buffer, bytes, Online_Count); //if unsuccessful, returns 0
                    
                    if(Login_Status !=  1){ //Returns 1 if Login was successful. If not 1, then unsuccessful
                        //send Client code for unsuccessful login
                        
                        if(Login_Status == 2){
                            reply[0] = '2'; //Wrong Credentials ---> Client_Code = 2
                        }
                        else{
                            reply[0] = '3'; //Max users reached ---> Client_Code = 3
                        }
                        reply[1] = '\0';
                        bytes_sent = send(client_socket, reply, strlen(reply), 0);

                        //terminate connection immediately
                        if(bytes_sent == 1){
                            shutdown(client_socket, SD_SEND);
                        }
                        
                    }
                    else{ //Login was successful
                        //send Client code for successful login
                        reply[0] = '1'; // ---> Client_Code = 1
                        reply[1] = '\0';
                        send(client_socket, reply, strlen(reply), 0);
                        
                        //store username
                        Extract_Sender_Username(buffer, Client[Online_Count].UserID); //extracts the second part. Format: <Server_Code><UserID><Password>
                        //store ip address
                        strcpy(Client[Online_Count].IP_Address, client_ip);
                        //increament online count
                        Online_Count++;
                        //terminate connection immediately
                        shutdown(client_socket, SD_SEND); 
                        printf("Currently Online: %d\n", Online_Count);
                    }
                    break;
                case '2': //creating a new account ---> Will also store the IP Address
                    printf("Serving Case 2.\n");
                    int Create_Account_Status = Creating_New_Account(buffer, bytes, Online_Count);
                    if(Create_Account_Status != 1){
                        
                        if(Create_Account_Status == 3){ //max online users reached (Client_Code = 3)
                            reply[0] = '3';
                        }
                        else{
                            reply[0] = '8'; //UserName already exists. Cannot create a new account (Client_Code = 8)
                        }
                        reply[1] = '\0';
                        send(client_socket, reply, strlen(reply), 0);
                        //terminate connection immediately
                        shutdown(client_socket, SD_SEND);
                    }
                    else{ //Login was successful
                        reply[0] = '1';
                        reply[1] = '\0';
                        send(client_socket, reply, strlen(reply), 0);
                        Extract_Sender_Username(buffer, Client[Online_Count].UserID);
                        strcpy(Client[Online_Count].IP_Address, client_ip);
                        Online_Count++;
                        shutdown(client_socket, SD_SEND);
                    }
                    break;
                case '3': //adding a new friend ---> <Server_Code><FriendID>
                    printf("Serving Case 3.\n");
                    char Add_Friend_UserID[8];
                    Extract_Sender_Username(buffer, Add_Friend_UserID);
                    int Add_Friend_Status = Add_New_Friend(Add_Friend_UserID);
                    
                    if(Add_Friend_Status == 5){ //No such friend/User exists
                        reply[0] = '5';
                        reply[1] = '\0';
                        send(client_socket, reply, strlen(reply), 0);
                    }else{ //Friend exists
                        reply[0] = '4';
                        reply[1] = '\0';
                        send(client_socket, reply, strlen(reply), 0);
                    }
                    shutdown(client_socket, SD_SEND);
                    break;
                case '4': //send message to someone format: ---> <Server_Code><Sender_UserID><Reciever_UserID>
                    printf("Serving Case 4.\n");
                    Extract_Sender_Username(buffer, Sender_Username);
                    Extract_Reciever_Username(buffer, Reciever_Username);
                    Extract_Chat_Text(buffer, message, bytes);
                    //will have to check if user is online
                    int Online_Reciever_Index = Find_Online_User_Index(Reciever_Username, Client, Online_Count); //-1 if not online
                    
                    if(Online_Reciever_Index == -1){ //reciever is offline
                        reply[0] =  '6';
                        reply[1] = '\0';
                        send(client_socket, reply, strlen(reply), 0);
                        shutdown(client_socket, SD_SEND);
                    }
                    else{
                        reply[0] =  '9'; //tell the sender that his message would be delievered successfully
                        reply[1] = '\0';
                        send(client_socket, reply, strlen(reply), 0);
                        shutdown(client_socket, SD_SEND); //close socket with the sender
                        //getting the reciever ip address
                        char *receiver_ip = Client[Online_Reciever_Index].IP_Address;

                        SOCKET receiver_sock;
                        struct sockaddr_in receiver_addr;
                        receiver_sock = socket(AF_INET, SOCK_STREAM, 0);

                        receiver_addr.sin_family = AF_INET;
                        receiver_addr.sin_port = htons(PORT);
                        receiver_addr.sin_addr.s_addr = inet_addr(receiver_ip);
                        //connect
                        connect(receiver_sock, (struct sockaddr*)&receiver_addr, sizeof(receiver_addr));

                        char reply_Reciever[BUFFER_SIZE];

                        strcpy(reply_Reciever, "7");
                        strcat(reply_Reciever, message);

                        send(receiver_sock, reply_Reciever, strlen(reply_Reciever), 0);

                        shutdown(receiver_sock, SD_SEND);

                    }
                    break;
                case '5': //logout format: <Server_Code><UserID>
                    //<UserID> is the id of the client who is trying to logout
                    printf("Serving Case 5.\n");
                    char Logout_UserID[8];
                    Extract_Sender_Username(buffer, Logout_UserID);
                    int Logout_Index = Find_Online_User_Index(Logout_UserID, Client, Online_Count);
                    //now remove this index
                    strcpy(Client[Logout_Index].UserID, "\0");
                    strcpy(Client[Logout_Index].IP_Address, "\0");
                    Online_Count--;
                    shutdown(client_socket, SD_SEND);
                    break;
            }

            //ending of my part of the code


            // Optional: reply , nah not needed now anymore
            //char reply[] = "Message received!";
            //send(client_socket, reply, strlen(reply), 0);
        }

        //closesocket(client_socket); will see if have to include this part or not
    }

    shutdown(server_socket, SD_SEND);
    WSACleanup();


    return 0;
}
