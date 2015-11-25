#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char *argv[]){
    int i, j, n, m, aux, temp, voltas[101], min;
    
    scanf("%d %d", &n, &m);
    for(i = 0, temp = 0; i < n; i++, temp = 0){
        for(j = 0; j < m; j++){
            scanf("%d", &aux);
            temp += aux;
        }
        voltas[i] = temp;
    }
    
    for (i = 0; i < 3; i++){
        min = i;
        for(j = 0; j < n; j++){
            if((voltas[j] < voltas[min])){
                min = j;
            }
        }
        
        voltas[min] = INT_MAX;
        printf("%d\n", min+1);
    }
    
    return EXIT_SUCCESS;
}