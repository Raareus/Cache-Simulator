/*Written by Graham Hill;*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc,char** argv){
    int size, assc, replace, write, LRU, block, tmp, tag_t, set_num, i, j, assc_count, k, assc_num, hit=0, miss=0, mem_writes=0, mem_reads=0, l, tag=0;
    char* file;
    float miss_rate;
    char op;
    long long int  num_sets;
    long long unsigned  address;
    
    size = atoi(argv[1]); //Size of the cache in bytes
    assc = atoi(argv[2]); //Associativity 1,2,4,....,64
    replace = atoi(argv[3]); //Replacement Policy: 0 for LRU, 1 for FIFO
    write = atoi(argv[4]); //Write-Back Policy: 0 for write-through, 1 for write-back
    file = argv[5]; //Trace File
    printf("Size is '%d', assc is '%d', replace is '%d', write is '%d'. \n", size, assc, replace, write);
    
    //Number of sets = Cache_size / (Block_Size*Associativity)
    block = 64;
    num_sets = size / ( block * assc);
        //printf( "%d\n", num_sets);

    long long int *cache[num_sets]; //allocate memory for base array pointer 
            for (i=0; i<=num_sets; i++){ //allocate memory for each pointer in array
                cache[i] = (long long int *)malloc(assc * num_sets * sizeof(long long int));
            }
            if(cache == NULL) 
                printf("Allocate Fail\n");


    //long long int cache[num_sets][assc];
    for(i=0; i<=num_sets; i++){
        for(j=0; j<=assc; j++){
            cache[i][j]=0;
        }
    }
    //printf("Cache Test: %lld\n", cache[0][0]);
    i=0;
    j=0;
    int *meta_data[num_sets]; //alocate memory for meta data (dirty bit)
            for (i=0; i<=num_sets; i++){ //allocate memory for each pointer in array
                meta_data[i] = (int *)malloc(assc * num_sets * sizeof(int));
            }
             if(meta_data == NULL)
                printf("Allocate Fail\n");

    for(i=0; i<=num_sets; i++){
        for(j=0; j<=assc; j++){
            meta_data[i][j]=0;
        }
    }
    int *tag_track[num_sets]; //alocate memory for tag tracking array
            for (j=0; j<=num_sets; j++){ //allocate memory for each pointer in array
                tag_track[j] = (int *)malloc(assc * set_num * sizeof(int));
            }
             if(tag_track == NULL)
                printf("Allocate Fail\n");
    for(i=0; i<=num_sets; i++){
        for(j=0; j<=assc; j++){
            tag_track[i][j]=0;
        }
    }
    i=0;
    j=0;
    tag_t=0;
    tmp=0;
    
    /*
    |||||||||||||||||||||||||||
    OPEN FILE AND START CACHING
    |||||||||||||||||||||||||||
    */

    FILE *ptr; //file pointer
    ptr = fopen(file , "r"); //open file for reading
    if(ptr == NULL)
        printf("File open Failure");
    while(fscanf(ptr, "%c %llx\n", &op, &address) != EOF){
        //l++;
        set_num = (address/block)%num_sets;
        tag = address/block;
        
        i = 0;
        ///////////////////////////////
        //////////CONDITION 1//////////
        ///////////////////////////////
        //printf("Entering  0 0");
        if(replace==0 && write==0) // LRU policy and write-through
        {      
            //printf("enter first loop");
            k = 0;
            if(op == 'R'){
                for(i=0;i<assc && k != 1 && k != 2;i++){
                    //printf("enter first loop");
                    //added increment of tag_track added k !=2
                    if(cache[set_num][i] == tag && k != 2){
                        hit++;
                        for(j=0;j<assc;j++){
                            if(cache[set_num][j] =! 0){
                                tag_track[set_num][j]++;
                            }
                        }
                        tag_track[set_num][i] = 0; 
                        //printf("Cache hit LRU\n");
                        k = 2;
                    }
                    else if(cache[set_num][i] != tag && i == (assc - 1)){
                        //printf("Cache miss\n");
                        miss++;
                        mem_reads++;
                        k=1;
                    }
                }
                while(k == 1){
                    for(i=0;i<assc && k !=0;i++){
                        //printf("k==1\n");
                        if (cache[set_num][i] == tag /*&& tag_track[set_num][i] != 0*/ && k != 0){
                            //printf("OOPS\n");
                            k=0;
                        }
                        else 
                        if(cache[set_num][i] == 0 && tag_track[set_num][i] == 0 && k != 0){
                            cache[set_num][i] = tag;
                            //printf("Cache ==0 and tag_track == 0\n");
                            for(j=0; j<assc; j++){
                                //printf("Increment tag_track 1 for loop\n");
                                if(cache[set_num][j] != 0/*tag_track[set_num][j] != 0*/){
                                    tag_track[set_num][j]++;
                                    //printf("Increment tag_track 1 if loop\n");
                                }
                            }
                            tag_track[set_num][i] = 0;
                            k=0;
                        }
                        //changed cache != to tag and added "&& i ==assc" commented out "&&tag_track[set_num][i] != 0
                        else if(cache[set_num][i] != 0 /*&& tag_track[set_num][i] > 0 */&& i == (assc - 1) && k != 0){
                            tmp = tag_track[set_num][0];
                            LRU = 0;
                            //printf("START\n");
                            for(j=0; j<assc ;j++){
                                //printf("Check for larger replacement for loop\n");
                                if(tmp < tag_track[set_num][j]){
                                    //printf("%d", j/*tag_track[set_num][j]*/);
                                    //printf("%d\n", tmp);
                                    //printf("Check for larger replacement if loop\n");
                                    tmp = tag_track[set_num][j];
                                    LRU = j;
                                }
                                //printf("Tag array: %d\n",tag_track[set_num][j]);
                                //printf("tmp: %d\n",tmp);
                                //printf("LRU: %d\n", LRU);
                            }
                            //printf("END\n");
                            //printf("%d", LRU);
                            cache[set_num][LRU] = tag;
                            for(j=0; j<assc; j++){
                                //printf("Increment tag_track 2 for loop\n");
                                if(cache[set_num][j] != 0/*tag_track[set_num][j] != 0*/)
                                    tag_track[set_num][j]++;
                                    //printf("Increment tag_track 2 if loop\n");
                                }
                            tag_track[set_num][LRU]=0;
                            k=0;
                        }
                    }
                }
            }
             else if(op == 'W'){
                //printf("Entering OP == W\n");
                k=1;
                for(i=0;i<assc && k==1;i++){
                    if(cache[set_num][i] == tag){
                        //printf("Hitting Write\n");
                        hit++;
                        mem_writes++;
                        for(j=0;j<assc;j++){
                            if(cache[set_num][j] =! 0){
                                tag_track[set_num][j]++;
                            }
                        }
                        tag_track[set_num][i] = 0;
                        k=0;
                    }
                    else if(cache[set_num][i] != tag && i == (assc - 1)){
                        //printf("miss write\n");
                        miss++;
                        mem_writes++;
                        k=0;
                    }
                }
            }

        }
        ///////////////////////////////
        //////////CONDITION 2//////////
        ///////////////////////////////
        else if(replace==0 && write==1) //LRU policy and write-back
        {
            //printf("Entering loop main 2\n");
            if(op == 'R'){
                for(i=0;i<assc;i++){
                    if(cache[set_num][i] == tag){
                        hit++;
                        //printf("Cache hit LRU\n");
                    }
                    else if(cache[set_num][i] != tag && i == (assc - 1)){
                        //printf("Cache miss\n");
                        miss++;
                        k=1;
                    }
                }
                while(k==1){
                    for(i=0;i<assc && k !=0;i++){
                        //printf("k==1\n");
                        if (cache[set_num][i] == tag && tag_track[set_num][i] != 0){
                            k=0;
                        }
                        else if(cache[set_num][i] == 0 && tag_track[set_num][i] == 0){
                            cache[set_num][i] = tag;
                            //printf("Cache ==0 and tag_track == 0\n");
                            for(j=0; j<assc; j++){
                                //printf("Increment tag_track 1 for loop\n");
                                if(tag_track[set_num][j] != 0){
                                    tag_track[set_num][j]++;
                                    //printf("Increment tag_track 1 if loop\n");
                                }
                            }
                            tag_track[set_num][i] = 1;
                            meta_data[set_num][i] = 0;
                            k=0;
                        }
                        else if(cache[set_num][i] !=0 && tag_track[set_num][i] != 0){
                            tmp = tag_track[set_num][0];
                            LRU = 0;
                            for(j=0; j<assc ;j++){
                                //printf("Check for larger replacement for loop\n");
                                if(tmp < tag_track[set_num][j]){
                                    //printf("Check for larger replacement if loop\n");
                                    tmp = tag_track[set_num][j];
                                    LRU = j;
                                }
                            }
                            cache[set_num][LRU] = tag;
                            if(meta_data[set_num][LRU] == 1){
                                mem_writes++;
                            }
                            for(j=0; j<assc; j++){
                                //printf("Increment tag_track 2 for loop\n");
                                if(tag_track[set_num][j] != 0)
                                    tag_track[set_num][j]++;
                                    //printf("Increment tag_track 2 if loop\n");
                                }
                            tag_track[set_num][LRU]=1;
                            meta_data[set_num][LRU] = 0;
                            mem_reads++;
                            k=0;
                        }
                    }
                }
            }
            else if(op == 'W'){
                for(i=0;i<assc;i++){
                    if(cache[set_num][i] == tag){
                        //printf("Hitting Write\n");
                        hit++;
                        meta_data[set_num][i] = 1;
                    }
                    else if(cache[set_num][i] != tag && i == (assc - 1)){
                        k = 1;
                    }
                }    
                while(k==1){
                    for(i=0;i<assc && k !=0;i++){
                        //printf("k==1\n");
                        if (cache[set_num][i] == tag && tag_track[set_num][i] != 0){
                            k=0;
                        }
                        else if(cache[set_num][i] == 0 && tag_track[set_num][i]== 0){
                            cache[set_num][i] = tag;
                            //printf("Cache ==0 and tag_track == 0\n");
                            for(j=0; j<assc; j++){
                                //printf("Increment tag_track 1 for loop\n");
                                if(tag_track[set_num][j] != 0){
                                    tag_track[set_num][j]++;
                                    //printf("Increment tag_track 1 if loop\n");
                                }
                            }
                            tag_track[set_num][i] = 1;
                            meta_data[set_num][i] = 1;
                            mem_reads++;
                            k=0;
                        }
                        else if(cache[set_num][i] !=0 && tag_track[set_num][i] != 0){
                            tmp = tag_track[set_num][0];
                            LRU = 0;
                            for(j=0; j<assc ;j++){
                                //printf("Check for larger replacement for loop\n");
                                if(tmp < tag_track[set_num][j]){
                                    //printf("Check for larger replacement if loop\n");
                                    tmp = tag_track[set_num][j];
                                    LRU = j;
                                }
                            }
                            cache[set_num][LRU] = tag;
                            if(meta_data[set_num][LRU] == 1){
                                mem_writes++;
                            }
                            for(j=0; j<assc; j++){
                                //printf("Increment tag_track 2 for loop\n");
                                if(tag_track[set_num][j] != 0)
                                    tag_track[set_num][j]++;
                                    //printf("Increment tag_track 2 if loop\n");
                                }
                            tag_track[set_num][LRU] = 1;
                            meta_data[set_num][LRU] = 1;
                            mem_reads++;
                            k=0;
                        }
                    }
                }    
            }
        }

        ///////////////////////////////
        //////////CONDITION 3//////////
        ///////////////////////////////
        else if(replace==1 && write==0) // FIFO policy and write-through
        {
            //printf("we made it\n");
            k=0;
            //printf("op: %c\n", op);
            //printf("tag: %d\n",tag);
            if(op == 'R'){
                //printf("we made it\n");
                for(i=0;i<assc;i++){
                    /*
                    printf("Cache:%lld\n",cache[set_num][i]);
                    printf("Tag:%d\n",tag);
                    printf("assc:%d\n",assc);
                    printf("I:%d\n",i);
                    printf("\n");
                    */
                    if(cache[set_num][i] == tag){
                        hit++;
                    }
                    else if(cache[set_num][i] != tag && i == (assc - 1)){
                        //printf("Cache miss\n");
                        miss++;
                        mem_reads++;
                        k=1;
                    }
                }
                while(k==1){
                    //printf("K==1\n");
                    for(i=0;i<assc && k !=0;i++){
                        if (cache[set_num][i] == tag && tag_track[set_num][i] == 1){
                            k=0;
                        }
                        else if(cache[set_num][i] == 0 && tag_track[set_num][i] == 0){
                            //printf("hitting cache\n");
                            cache[set_num][i] = tag;
                            tag_track[set_num][i] = 1;
                            k=0;
                        }
                        else if (cache[set_num][i] != 0 && tag_track[set_num][i] == 0){
                            cache[set_num][i] = tag;
                            tag_track[set_num][i] = 1;
                            k=0;
                        }
                        else if(cache[set_num][(assc - 1)] != 0 && tag_track[set_num][(assc - 1)] == 1){
                            for(j=0; j<assc; j++){
                                tag_track[set_num][j] = 0;
                            }
                            if(cache[set_num][0] != 0 && tag_track[set_num][0] == 0){
                                cache[set_num][0] = tag;
                                tag_track[set_num][0] = 1;
                                k=0;
                            }
                        }
                    }
                }
            }
            else if(op == 'W'){
                //printf("Entering OP == W\n");
                k=1;
                for(i=0;i<assc && k==1;i++){
                    if(cache[set_num][i] == tag){
                        //printf("Hitting Write\n");
                        hit++;
                        mem_writes++;
                        k=0;
                    }
                    else if(cache[set_num][i] != tag && i == (assc - 1)){
                        //printf("miss write\n");
                        miss++;
                        mem_writes++;
                        k=0;
                    }
                }
            }
        }

        ///////////////////////////////
        //////////CONDITION 4//////////
        ///////////////////////////////
        else if(replace ==1 && write==1) //FIFO policy and write-back
        {
           //printf("we made it\n");
            k=0;
            //printf("op: %c\n", op);
            //printf("tag: %d\n",tag);
            if(op == 'R'){
                //printf("we made it\n");
                for(i=0;i<assc;i++){
                    if(cache[set_num][i] == tag){
                        hit++;
                    }
                    else if(cache[set_num][i] != tag && i == (assc - 1)){
                        miss++;
                        k=1;
                    }
                }
                while(k==1){
                    //printf("K==1\n");
                    for(i=0;i<assc && k !=0;i++){
                        if (cache[set_num][i] == tag && tag_track[set_num][i] == 1){
                            k=0;
                        }
                        else if(cache[set_num][i] == 0 && tag_track[set_num][i] == 0){
                            //printf("hitting cache\n");
                            cache[set_num][i] = tag;
                            if(meta_data[set_num][i] == 1)
                                mem_writes++;
                            tag_track[set_num][i] = 1;
                            mem_reads++;
                            meta_data[set_num][i] = 0;
                            k=0;
                        }
                        else if (cache[set_num][i] != 0 && tag_track[set_num][i] == 0){
                            cache[set_num][i] = tag;
                            tag_track[set_num][i] = 1;
                            if(meta_data[set_num][i] == 1)
                                mem_writes++;
                            mem_reads++;
                            meta_data[set_num][i] = 0;
                            k=0;
                        }
                        else if(cache[set_num][(assc - 1)] != 0 && tag_track[set_num][(assc - 1)] == 1){
                            for(j=0; j<assc; j++){
                                tag_track[set_num][j] = 0;
                            }
                            if(cache[set_num][0] != 0 && tag_track[set_num][0] == 0){
                                cache[set_num][0] = tag;
                                tag_track[set_num][0] = 1;
                                if(meta_data[set_num][i] == 1)
                                    mem_writes++;
                                mem_reads++;
                                meta_data[set_num][i] = 0;
                                k=0;
                            }
                        }
                    }
                }
            }
            else if(op == 'W'){
                //printf("Entering OP == W\n");
                for(i=0;i<assc;i++){
                    if(cache[set_num][i] == tag){
                        //printf("Hitting Write\n");
                        hit++;
                        meta_data[set_num][i] = 1;
                        k=0;
                    }
                    else if(cache[set_num][i] != tag && i == (assc - 1)){
                        //printf("miss write\n");
                        miss++;
                        k=1;
                    }
                }
                while(k==1){
                    //printf("K==1\n");
                    for(i=0;i<assc && k !=0;i++){
                        if (cache[set_num][i] == tag && tag_track[set_num][i] == 1){
                            k=0;
                            meta_data[set_num][i] = 1;
                        }
                        else if(cache[set_num][i] == 0 && tag_track[set_num][i] == 0){
                            //printf("hitting cache\n");
                            cache[set_num][i] = tag;
                            if(meta_data[set_num][i] == 1)
                                mem_writes++;
                            tag_track[set_num][i] = 1;
                            mem_reads++;
                            meta_data[set_num][i] = 1;
                            k=0;
                        }
                        else if (cache[set_num][i] != 0 && tag_track[set_num][i] == 0){
                            cache[set_num][i] = tag;
                            tag_track[set_num][i] = 1;
                            if(meta_data[set_num][i] == 1)
                                mem_writes++;
                            mem_reads++;
                            meta_data[set_num][i] = 1;
                            k=0;
                        }
                        else if(cache[set_num][(assc - 1)] != 0 && tag_track[set_num][(assc - 1)] == 1){
                            for(j=0; j<assc; j++){
                                tag_track[set_num][j] = 0;
                            }
                            if(cache[set_num][0] != 0 && tag_track[set_num][0] == 0){
                                cache[set_num][0] = tag;
                                tag_track[set_num][0] = 1;
                                if(meta_data[set_num][i] == 1)
                                    mem_writes++;
                                mem_reads++;
                                meta_data[set_num][i] = 1;
                                k=0;
                            }
                        }
                    }
                }
            }

        }
        
        
    //printf("%d\n", l);
    }
    //printf("%d\n",LRU);
    //printf("hit: %d\nmiss:%d\nmiss_rate: %f\n", hit, miss, miss_rate);
    miss_rate = 1 - ((float)hit/((float)miss+(float)hit));
    //printf("hit: %d\nmiss:%d\nmiss_rate: %f\n", hit, miss, miss_rate);
    printf(" miss rate: %f \n mem writes: %d \n mem reads: %d\n", miss_rate, mem_writes, mem_reads);
    //printf("hit: %d\n miss: %d\n", hit, miss);
    //printf("mem writes: %d \nmem reads: %d \nhits: %d\nmisses %d\n", mem_writes, mem_reads, hit, miss);
    //printf("OP: %c\n", op);
    //printf("%llu\n", address);
    //printf("set_num : %d\ntag: %d\n",set_num,tag);
    //for(i=0; i<assc; i++)
    //    printf("cache: %lld\n", cache[25][i]);

    for (i=0; i<=num_sets; i++){ //deallocate memory for each pointer in array
        free(cache[i]);
    }
    //free(cache);

    for (i=0; i<=num_sets; i++){ //deallocate memory for each pointer in array
        free(tag_track[i]);
    }
    //free(tag_track);

    for (i=0; i<=num_sets; i++){ //deallocate memory for each pointer in array
        free(meta_data[i]);
    }
    //free(meta_data);
}