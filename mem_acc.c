#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

long long convert_to_bytes(char *size_str) {
    int len = strlen(size_str);
    long long size = 0;
    if (len > 2) {
        char suffix[3];
        strncpy(suffix, &size_str[len - 2], 2);
        suffix[2] = '\0';
        char prefix[32];
        strncpy(prefix, size_str, len - 2);
        prefix[len - 2] = '\0';
        long long num_bytes = atoll(prefix);
        if (strcasecmp(suffix, "mb") == 0) {
            size = num_bytes * 1024LL * 1024LL;
        } else if (strcasecmp(suffix, "gb") == 0) {
            size = num_bytes * 1024LL * 1024LL * 1024LL;
        } else if (strcasecmp(suffix, "kb") == 0) {
            size = num_bytes * 1024LL;
        } else {
            switch (suffix[1]) {
                case 'b':
                case 'B':
                    size = num_bytes;
                    break;
                case 'k':
                case 'K':
                    size = num_bytes * 1024LL;
                    break;
                case 'm':
                case 'M':
                    size = num_bytes * 1024LL * 1024LL;
                    break;
                case 'g':
                case 'G':
                    size = num_bytes * 1024LL * 1024LL * 1024LL;
                    break;
                default:
                    printf("Error: Invalid suffix '%s'\n", suffix);
            }
        }
    } else {
        printf("Error: Invalid size string '%s'\n", size_str);
    }
    return size;
}


int main(int argc, char* argv[]) {
    if (argc < 5) {
        printf("Usage: %s memory_size(byte) num_iter num_loop seq_rate\n", argv[0]);
        return 1;

    }
    int *rand_idx = NULL;
    int len_of_rand_dat = 0;
    long long rand_dat_mem_size = 0;
    long long rand_dat_max_idx = 0;
    int memory_size = atoi(argv[1]);
    int num_iter = atoi(argv[2]);
    int num_loop = atoi(argv[3]);
    float seq_rate = atof(argv[4]);

    // TODO get random index from rand_idx.dat
    DIR *dir;
    struct dirent *ent;
    char *dir_name = "./"; // current dir

    dir = opendir(dir_name);
    if(dir == NULL){
         printf("Error: Failed to open directory '%s'\n", dir_name);
         return 1;
    }

    char *prefix = "randidx_";
    char *suffix = ".dat";
    int prefix_len = strlen(prefix);
    int suffix_len = strlen(suffix);
    char *start_ptr = NULL;
    char *end_ptr = NULL;

    char size_str[256], num_str[256];

    while((ent = readdir(dir)) != NULL){
        char *file_name = ent->d_name;
        int name_len = strlen(file_name);
        if(name_len > 4 && strcmp(&file_name[name_len - 4], ".dat") == 0){
            char full_path[256];
            snprintf(full_path, sizeof(full_path), "%s%s", dir_name, file_name);
            FILE *f = fopen(full_path, "r");
            if(f == NULL){
                printf("Error: Failed to open file '%s'\n", full_path);
                return 1;
            }else{
                /* printf("'%s' found\n", file_name); */
                if(strncmp(file_name, prefix, strlen(prefix)) != 0){
                    printf("Error: invalid fiename (prefix)\n");
                    return 1;
                }
                if(strcmp(&file_name[strlen(file_name) - strlen(suffix)], suffix) != 0){
                    printf("Error: invalid fiename (suffix)\n");
                    return 1;
                }
                // Get size_str
                int num_len = strlen(&file_name[prefix_len]) - strlen(strstr(&file_name[prefix_len], "_")) - 1;
                int last_delim_index = strrchr(file_name, '_') - file_name;
                int size_len = last_delim_index - prefix_len;
                strncpy(size_str, &file_name[prefix_len], size_len);
                size_str[size_len] = '\0';
                // Get num_str
                sscanf(&file_name[prefix_len + size_len + 1], "%[^.]", num_str);

                /* printf("size_str:%s\n", size_str); */
                /* printf("num_str:%s\n", num_str); */
                int rand_num = atoi(num_str);
                rand_dat_mem_size = convert_to_bytes(size_str);
                rand_dat_max_idx = rand_dat_mem_size/sizeof(int); // constraint.
                printf("target memory_size of rand_idx file(%s) is %llu byte\n", file_name, rand_dat_mem_size);
                printf("rand_memory_max_idx:%llu\n", rand_dat_max_idx);

                // init random idx
                rand_idx = (int*)malloc(rand_num * sizeof(int));
                char buffer[256];
                while(fgets(buffer, sizeof(buffer), f) != NULL){
                    rand_idx[len_of_rand_dat] = atoi(buffer);
                    /* printf("%d\n", rand_idx[tmp_cnt]); */
                    len_of_rand_dat++;
                }
                fclose(f);
                if(rand_num != len_of_rand_dat){
                    printf("rand file's max idx(%u) and counted max idx(%d) is not same\n", rand_num, len_of_rand_dat);
                    return 1;
                }
                break;
            }
        }
    }

    // Allocate memory
    int num_int = (int)(memory_size / sizeof(int));
    int seq_num = (int)(num_int * seq_rate);
    int ran_num = num_int - seq_num;

    if(ran_num != 0){
        if(ran_num < rand_dat_max_idx){
            printf("target random memory's max index(%d) is less than randidx~.dat file's one(%lld)\n", ran_num, rand_dat_max_idx);
            printf("it could occur index overflow\n");
            printf("generate randidx~.dat file that smaller or same with %d byte\n", ran_num * 4);
            return 1;
        }
    }
    printf("num_iter:%d\n", num_iter);
    printf("num_seq_int: %d\tsize: %d byte\n", seq_num, seq_num*4);
    printf("num_ran_int: %d\tsize: %d byte\n", ran_num, ran_num*4);

    int* seq_mem = (int*)malloc(seq_num * sizeof(int));
    int* ran_mem = (int*)malloc(ran_num * sizeof(int));

    struct timeval ts_start;
    struct timeval ts_end;
    struct timeval ts_diff;

    // Initialize memory with random values
    printf("initialization memory start\n");
    for (int i = 0; i < seq_num; i++) {
        seq_mem[i] = rand();
    }
    for (int i = 0; i < ran_num; i++) {
        ran_mem[i] = rand();
    }
    printf("initialization memory end\n");
    int dummy = 0;
    if(num_loop > 0){
        printf("infinite loop start\n");
        while(1){
            gettimeofday(&ts_start,NULL);
            for (int i = 0; i < num_iter; i++) {
                for (int k = 0; k < seq_num/16; k+=16) {
                    // 4byte * 16 = 64byte <- 64byte is prefetcher unit
                    dummy = seq_mem[k];
                    dummy = seq_mem[k+1];
                    dummy = seq_mem[k+2];
                    dummy = seq_mem[k+3];
                    dummy = seq_mem[k+4];
                    dummy = seq_mem[k+5];
                    dummy = seq_mem[k+6];
                    dummy = seq_mem[k+7];
                    dummy = seq_mem[k+8];
                    dummy = seq_mem[k+9];
                    dummy = seq_mem[k+10];
                    dummy = seq_mem[k+11];
                    dummy = seq_mem[k+12];
                    dummy = seq_mem[k+13];
                    dummy = seq_mem[k+14];
                    dummy = seq_mem[k+15];
                }
                for (int k = 0; k < ran_num/16; k+=16) { // access random memory address as much as seq 
                    dummy = ran_mem[rand_idx[k]];
                    dummy = ran_mem[rand_idx[k+1]];
                    dummy = ran_mem[rand_idx[k+2]];
                    dummy = ran_mem[rand_idx[k+3]];
                    dummy = ran_mem[rand_idx[k+4]];
                    dummy = ran_mem[rand_idx[k+5]];
                    dummy = ran_mem[rand_idx[k+6]];
                    dummy = ran_mem[rand_idx[k+7]];
                    dummy = ran_mem[rand_idx[k+8]];
                    dummy = ran_mem[rand_idx[k+9]];
                    dummy = ran_mem[rand_idx[k+10]];
                    dummy = ran_mem[rand_idx[k+11]];
                    dummy = ran_mem[rand_idx[k+12]];
                    dummy = ran_mem[rand_idx[k+13]];
                    dummy = ran_mem[rand_idx[k+14]];
                    dummy = ran_mem[rand_idx[k+15]];
                    /* dummy = ran_mem[rand()%ran_num]; */
                }
            }
            gettimeofday(&ts_end,NULL);
            ts_diff.tv_sec = ts_end.tv_sec - ts_start.tv_sec;
            ts_diff.tv_usec = ts_end.tv_usec - ts_start.tv_usec;
            if(ts_diff.tv_usec < 0){
                ts_diff.tv_sec = ts_diff.tv_sec - 1;
                ts_diff.tv_usec = ts_diff.tv_usec + 1000000;
            }
            printf("[total access time]%ld.%ld\n",ts_diff.tv_sec, ts_diff.tv_usec);
        }
    }else{
        gettimeofday(&ts_start,NULL);
        for (int i = 0; i < num_iter; i++) {
            for (int k = 0; k < seq_num/16; k+=16) {
                // 4byte * 16 = 64byte <- 64byte is prefetcher unit
                dummy = seq_mem[k];
                dummy = seq_mem[k+1];
                dummy = seq_mem[k+2];
                dummy = seq_mem[k+3];
                dummy = seq_mem[k+4];
                dummy = seq_mem[k+5];
                dummy = seq_mem[k+6];
                dummy = seq_mem[k+7];
                dummy = seq_mem[k+8];
                dummy = seq_mem[k+9];
                dummy = seq_mem[k+10];
                dummy = seq_mem[k+11];
                dummy = seq_mem[k+12];
                dummy = seq_mem[k+13];
                dummy = seq_mem[k+14];
                dummy = seq_mem[k+15];
            }
            for (int k = 0; k < ran_num/16; k+=16) { // access random memory address as much as seq 
                dummy = ran_mem[rand_idx[k]];
                dummy = ran_mem[rand_idx[k+1]];
                dummy = ran_mem[rand_idx[k+2]];
                dummy = ran_mem[rand_idx[k+3]];
                dummy = ran_mem[rand_idx[k+4]];
                dummy = ran_mem[rand_idx[k+5]];
                dummy = ran_mem[rand_idx[k+6]];
                dummy = ran_mem[rand_idx[k+7]];
                dummy = ran_mem[rand_idx[k+8]];
                dummy = ran_mem[rand_idx[k+9]];
                dummy = ran_mem[rand_idx[k+10]];
                dummy = ran_mem[rand_idx[k+11]];
                dummy = ran_mem[rand_idx[k+12]];
                dummy = ran_mem[rand_idx[k+13]];
                dummy = ran_mem[rand_idx[k+14]];
                dummy = ran_mem[rand_idx[k+15]];
                /* dummy = ran_mem[rand()%ran_num]; */
            }
        }
        gettimeofday(&ts_end,NULL);

    }
    ts_diff.tv_sec = ts_end.tv_sec - ts_start.tv_sec;
    ts_diff.tv_usec = ts_end.tv_usec - ts_start.tv_usec;
    if(ts_diff.tv_usec < 0){
        ts_diff.tv_sec = ts_diff.tv_sec - 1;
        ts_diff.tv_usec = ts_diff.tv_usec + 1000000;
    }
    printf("[total access time]%ld.%ld\n",ts_diff.tv_sec, ts_diff.tv_usec);

}
