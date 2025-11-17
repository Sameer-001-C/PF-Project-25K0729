//works on devc++


//example sample:

#include <direct.h>
#include <stdio.h>

int main() {
    int status = _mkdir("D:/myFiles");

    if (status == 0) {
        printf("Directory created successfully.\n");
    } else {
        perror("Error creating directory");
    }

    return 0;
}
