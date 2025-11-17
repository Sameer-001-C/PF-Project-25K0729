//we could maybe implement this into a function. just maybe?

//this stores the entire contents of a file into a buffer

FILE *fp = fopen("chat.txt", "r"); //we would enter the location of file, the Client'sFriendLocation would be here

char buffer[5000];     //big enough to hold all of chat
buffer[0] = '\0';      //starting as an empty string

char line[256];

while (fgets(line, sizeof(line), fp)) {
    strcat(buffer, line);
}

fclose(fp);



//GUI:

//then, we could use Raylib like this:

DrawText(buffer, 20, 20, 20, BLACK); //this prints the entire file on screen. maybe implement this on a loop? or idk, append?

