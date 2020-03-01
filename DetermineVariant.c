#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int numberOfTasks = atoi(argv[1]);
    int numberOnTheList = atoi(argv[2]);
    
    srand(numberOnTheList);
    
    int variantNumber = (rand() % numberOfTasks) + 1;
    printf("Variant number: %d\n", variantNumber);

    return 0; 
}
