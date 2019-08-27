#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

const long long max_size = 2000;        // max length of strings
const long long N = 40;                 // number of closest words that would be shown
const long long max_w = 50;             // max length of vocabulary entries

int main(int argc, char **argv){
    FILE *one, *two;
    char st1[max_size];
    char *bestw_one[N], *bestw_two[N];
    char file_one[max_size], file_two[max_size],st[100][max_size];
    float dist_one, dist_two, len_one, len_two;
    float bestd_one[N], bestd_two[N], vec_one[max_size], vec_two[max_size];
    long long words_one, size_one, words_two, size_two;
    long long a, b, c, d, e, cn, bi[100];
    float *M_one, *M_two;
    char *vocab_one, *vocab_two;
    
    float total_difference = 0;
    
    if(argc < 3) {
        printf("Use this to compare the cosine distance between words.\n");
        return 0;
    }    

    strcpy(file_one, argv[1]);
    strcpy(file_two, argv[2]);


    one = fopen(file_one, "rb");
    two = fopen(file_two, "rb");
    if(one == NULL || two == NULL){
        printf("Input file(s) are not correct.");
        return -1;
    }

    // Read the words
    fscanf(one, "%lld", &words_one);
    fscanf(two, "%lld", &words_two);
    fscanf(one, "%lld", &size_one);
    fscanf(two, "%lld", &size_two);
    if((words_one != words_two) || (size_one != size_two)){
        printf("These are possibly from diffrent training datas");
        return -1;
    }

    // vocab : List of all the vocabulary words
    vocab_one = (char *)malloc((long long) words_one * max_w * sizeof(char));
    vocab_two = (char *)malloc((long long) words_two * max_w * sizeof(char));
    // bestw : List of the cloest words
    // Only best words of the first, as that is the standard
    for (a = 0; a < N; a++){ 
        bestw_one[a] = (char *)malloc(max_size * sizeof(char));
        bestw_two[a] = (char *)malloc(max_size * sizeof(char));
    }

    M_one = (float *)malloc((long long)words_one * (long long)size_one * sizeof(float));
    M_two = (float *)malloc((long long)words_two * (long long)size_two * sizeof(float));
    if(M_one == NULL ){
        printf("Unable to allocate memory");
        return -1;
    }
    
    for(b = 0; b < words_one; b++){
        a = 0;
        while (1) {
            vocab_one[b * max_w + a] = fgetc(one);
            vocab_two[b * max_w + a] = fgetc(two);
            if (feof(one) || (vocab_one[b * max_w + a] == ' '))break;
            if ((a < max_w) && (vocab_one[b * max_w + a] != '\n')) a++;
        }
        vocab_one[b * max_w + a] = 0;
        vocab_two[b * max_w + a] = 0;
        for (a = 0; a < size_one; a++){
            fread(&M_one[a + b * size_one], sizeof(float), 1, one);
            fread(&M_two[a + b * size_two], sizeof(float), 1, two);
        }
        len_one = 0;
        len_two = 0;
        for (a = 0; a < size_one; a++){
            len_one += M_one[a + b * size_one] * M_one[a + b * size_one];
            len_two += M_two[a + b * size_two] * M_two[a + b * size_two];
        }
        len_one = sqrt(len_one);
        len_two = sqrt(len_two);
        for (a = 0; a < size_one; a++){
            M_one[a + b * size_one] /= len_one;
            M_two[a + b * size_two] /= len_two;
        }
    }
    fclose(one);
    fclose(two);
    while(1) {
        for(a = 0; a < N; a++) {
            bestd_one[a] = 0;
            bestd_two[a] = 0;
        }
        for(a = 0; a < N; a++) {
            bestw_one[a][0] = 0; 
            bestw_two[a][0] = 0;
        }
        printf("Enter word or sentence (EXIT to break): ");
        a = 0;
        while(1){
            st1[a] = fgetc(stdin);
            if((st1[a] == '\n') || (a >= max_size - 1)) {
                st1[a] = 0;
                break;
            }
            a++;
        }

        //Read the input
        if(!strcmp(st1, "EXIT")) break;
        cn = 0;
        b = 0;
        c = 0;
        while(1) {
            st[cn][b] = st1[c];
            b++;
            c++;
            st[cn][b] = 0;
            if (st1[c] == 0) break;
            if (st1[c] == ' ') {
                cn++;
                b = 0;
                c++;
            }
        }
        cn++;
        for (a = 0; a < cn; a++){
            //Detect Word
            for (b = 0; b < words_one; b++) if (!strcmp(&vocab_one[b * max_w], st[a])) break;
            if (b == words_one) b = -1;
            bi[a] = b;
            printf("\nWord: %s  Position in vocabulary %lld\n", st[a], bi[a]);
            if (b == -1) {
                printf("Out of dictionary word!\n");
                break;
            }
        }

        //Here is where the vectors are retrieved
        if (b == -1) continue;
        for(a = 0; a < size_one; a++) {
            vec_one[a] = 0;
            vec_two[a] = 0;
        }
        for(b = 0; b < cn; b++) {
            if(bi[b] == -1) continue;
            for (a = 0; a < size_one; a++) {
                vec_one[a] += M_one[a + bi[b] * size_one];
                vec_two[a] += M_two[a + bi[b] * size_two];
            }
        }
        len_one = 0;
        len_two = 0;
        for (a = 0; a < size_one; a++) {
            len_one += vec_one[a] * vec_one[a];
            len_two += vec_two[a] * vec_two[a];
        }
        len_one = sqrt(len_one);
        len_two = sqrt(len_two);
        for (a = 0; a < size_one; a++) {
            vec_one[a] /= len_one;
            vec_two[a] /= len_two;
        }
        for (a = 0; a < N; a++) {
            bestd_one[a] = -1;
            bestw_one[a][0] = 0;
            bestd_two[a] = -1;
            bestw_two[a][0] = 0;
        }
        
        //First Vector
        for (c = 0; c < words_one; c++){
            a = 0;
            for (b=0; b < cn; b++) if (bi[b] == c) a = 1;
            if (a == 1) continue;
            dist_one = 0;
            dist_two = 0;
            for (a = 0; a < size_one; a++) {
                dist_one += vec_one[a] * M_one[a + c * size_one];
            }
            for (a = 0; a < N; a++) {
                if (dist_one > bestd_one[a]){
                    for (d = N - 1; d > a ; d--) {
                        bestd_one[d] = bestd_one[d - 1];
                        strcpy(bestw_one[d], bestw_one[d - 1]);
                    }
                    bestd_one[a] = dist_one;
                    strcpy(bestw_one[a], &vocab_one[c * max_w]);
                    break;
                }
            }
        }

        for( b = 0; b < N; b++){
            for ( c = 0; c < words_two; c++){
                a = 0;
                for (e = 0; e < cn; e++) if(bi[e] == c) a = 1;
                if(a == 1) continue;
                dist_two = 0;
                if(!strcmp(&vocab_two[c * max_w], bestw_one[b])){
                    for(a = 0; a < size_one; a++){
                        dist_two += vec_two[a] * M_two[a + c * size_two];
                    } 
                    strcpy(bestw_two[b], bestw_one[b]);
                    bestd_two[b] = dist_two;
                    total_difference += fabsf(bestd_one[b] - bestd_two[b]);
                }
            }
        }

        printf("%50s\t%8s\t%8s\n", "Word", "First", "Second");
        printf("---------------------------------------------------------------------------------\n");

        for (a = 0; a < N; a++){
            printf("%50s\t%f\t%f\n", bestw_one[a], bestd_one[a], bestd_two[a]);
        }
        printf("Total Difference: %f\n", total_difference);
    }
    return 0;
}
