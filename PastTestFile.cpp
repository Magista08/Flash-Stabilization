#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>


int main(){
    // Testing microseconds
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    sleep(0.025);
    struct timeval time_latest;
    gettimeofday(&time_latest, NULL);
    printf("%d   %d\n", time_now.tv_sec, time_latest.tv_sec);
    printf("%d   %d\n", time_now.tv_usec, time_latest.tv_usec);

    long long time_storage_usec = time_now.tv_usec;
    long long time_storage_sec  = time_now.tv_sec;
    if (time_storage_sec < time_storage_usec) printf("The theory is correct!!!\n");
    else                                      printf("The theory is incorrect!!!\n");



    FILE *fp;
    int *a;
    int b = 2;
    a = &b;
    unsigned char readfile[256];
    fp = fopen("10MBFile.bin", "w+");
    //for(long long i = 0; i < 10*1024*1024; i++) fputc(0,fp);
    if(!fseek(fp,10*1024*1024,SEEK_SET)) fwrite(a,sizeof(a),1,fp); //NOT USEFUL
    fclose(fp);

    fp = fopen("10MBFile.bin", "r");
    fseek(fp,0,SEEK_END);
    long size = ftell(fp);
    printf("Creation finished\n");
    printf("%u\n",readfile[0]);
    printf("The file's size is %ld", size);




    char string[256];
    for(int i = 0; i < 10; i++){
        itoa(i, string, 10);
        printf("%s\n", string);
    }





    /*
    char sen_1[256] = "Robert Downey Jr";
    char sen_2[256] = "Love you ";
    char sen_3[256] = "times";
    char tran_lines = "\n";

    char test_1 = 2;
    int test_2 = 3000;
    char test_sen_1[2], test_sen_2[10];

    itoa(test_1, test_sen_1, 10);
    itoa(test_2, test_sen_2, 10);

    strcat(sen_1, test_sen_1);
    strcat(sen_2, test_sen_2);

    strcat(sen_1, "\n");
    strcat(sen_2, "\n");

    FILE *fp;
    fp = fopen("10MBFile.txt", "w+");

    fprintf(fp,sen_1);
    fprintf(fp,sen_2);
    fprintf(fp,sen_3);
    fclose(fp);
     */
    /*
    FILE *fp;
    char output[256], output_1[256];
    char filepath[] = "";
    unsigned long rand_num = 100000;

    SentenceSticker(output, filepath,  rand_num, 1);

    fp = fopen(output, "w+");
    fclose(fp);




    SentenceSticker(output_1, filepath,  rand_num, 0);
    //fp = fopen(output_1, "w+");
    //fclose(fp);
    */
    /*
    char src[40];
    char dest[100];
    char temp[100];

    memset(dest, '\0', sizeof(dest));
    strcpy(src, "This is runoob.com");
    strcpy(temp, src);
    strcat(temp, "\tHello World!");
    strcpy(dest, temp);

    printf("Final Target String： %s\n", dest);
     */
    /*
    char output_sentence[256];
    long number = 1564897489;
    long cpu_used = 10*1024;

    ReportSentence(output_sentence, number, cpu_used);
    printf("%s", output_sentence);
     */

    //Test creating files and deleting files
    /*
    char actual_path[100] = {0};
    FILE *fp;
    for(unsigned char i = 0; i < 100; i++){
        SentenceSticker(actual_path,"/mnt/e/Internship_ShanghaiElectric/TestingFlashMemory/100Files/", i, 1);
        fp = fopen(actual_path, "w+");
        if(fp == NULL){
            printf("Cannot Create the file\n");
            printf("%d", errno);
        }
        fclose(fp);
    }
    DeleteAllFiles("/mnt/e/Internship_ShanghaiElectric/TestingFlashMemory/100Files/", 99);


    char output[100];
    ReportSentence(output, 100, 100);
    printf("%s", output);
    */

    //Test Write Data in and check data
    /*
    struct timeval tm;
    gettimeofday(&tm, NULL);
    long long beginningnum = tm.tv_sec*1000000 + tm.tv_usec;
    WriteData("/mnt/e/Internship_ShanghaiElectric/TestingFlashMemory/100Files/testing.bin", beginningnum);
    printf("cpu used %lld\n", cpu_used);
    unsigned char check_begin = beginningnum % 256;
    FILE *fp;
    fp = fopen("/mnt/e/Internship_ShanghaiElectric/TestingFlashMemory/100Files/testing.bin","r");
    int detecing = CheckData(0,0,check_begin,fp);
    if(!detecing)
    printf("%d",detecing);
    else return -1;
    fclose(fp);
    */

}

