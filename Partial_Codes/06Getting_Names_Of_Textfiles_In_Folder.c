//This would be the actual path tho

//ClientA refers to the person using the client software
//Donot use this Code. An easier implementation has been divised.
#include <stdio.h>
#include <dirent.h>
#include <string.h>

int main() {
    const char *path = "D:/PF_Project/Client/Chats/25K0729"; //replace 25K0729 with ClientA's UserName
    DIR *d;
    struct dirent *dir;

    d = opendir(path);
    if (!d) {
        printf("Could not open directory.\n");
        return 1;
    }

    while ((dir = readdir(d)) != NULL) {

        // Skip "." and ".."
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            continue;

        // Check if file ends with ".txt"
        char *dot = strrchr(dir->d_name, '.');
        if (!dot || strcmp(dot, ".txt") != 0)
            continue;

        // Print the person ID without ".txt"
        *dot = '\0';  // remove extension
        printf("%s\n", dir->d_name);
    }

    closedir(d);
    return 0;
}
