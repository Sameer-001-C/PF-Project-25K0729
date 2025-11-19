// This function will extract the server code for further processing and decision taking

char Extract_Server_Code(char *message){
    char Server_Code = message[0];
    return Server_Code;
}

// Use it like this: Server_Code = Extract_Server_Code(message)
