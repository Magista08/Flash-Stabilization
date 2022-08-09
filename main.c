#include <stdio.h>    /*FOR: printf, FILE*/
#include <stdlib.h>   /*FOR: exit*/
#include <string.h>   /*FOR: strcpy*/
#include <sys/time.h> /*FOR: gettimeofday*/
#include <time.h>     /*FOR: time, localtime*/

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/* const varibales */
const char  FILE_TYPE[] = ".bin";
const char  FILE_NAME[] = "xxx_";
const char  RAND_NUM = 0;
const char *RAND_PTR = &RAND_NUM;

/* global variable */
long long   g_llStorageUsed = 0;
FILE *fpLogFile;
/*
 * The following functions except get_maximum files are all test used the code in the TestingUncertuanCode.c
 * The sample is kind of restricted.
 */
void sentence_sticker(char* _pszOutputSences, char _szFilePath[], long long _llFileNum, int _bInsertFixedName);
void report_sentence(char* _pszOutputParagraph, unsigned long _u8FileNum, long long _llLoopingTimes);
void delete_all_files(char _szFilePath[], long long _llMaxFilesNum);
void write_data(char _szFileName[], long long _llBeginTime, long long _llCreatedFilesNum);


int check_data(long long _llFileNum, unsigned long _u8MaxFilesNum, int _nBeginNum, FILE *_fpCheckFile, long long _llFileName);
long long get_time_now();
long long get_maximum_files_num(char *_pFilePath);

char *itoa(unsigned long long _u8Num,char* _pStr,int _nRadix);


int main(int args, char** argv){

    if (args < 2)
    {
        printf("USAGE: $0 <file_path>\n");
        return -1;
    }
    char szFilePath[1024];
    strcpy(szFilePath, argv[1]);
    strcat(szFilePath,"/");

    /* Test the path is valid or not*/
    char szRandFile[200];
    memset(szRandFile, 0, sizeof(szRandFile));
    sentence_sticker(szRandFile, szFilePath, 100, 1);
    FILE *fpTestFile = fopen(szRandFile, "w+");
    if (fpTestFile == NULL)
    {
        printf("The File Path is incorrect.\n");
        return -1;
    }
    fclose(fpTestFile);
    remove(szRandFile);


    /* Doing the loop to find the how many files can be created */
    long long llMaxFileNums = get_maximum_files_num(szFilePath);
    if(llMaxFileNums == 0)
    {
        printf("Cannot Detect Max Files Number. Some errors occure!\n");
        return -1;
    }

    /* write the maximum creating files into the .log file */
    fpLogFile = fopen("DataDetectingLog.log", "w+");
    char szMaxFilesSentences[256] = {0};
    sprintf(szMaxFilesSentences, "The maximum creating files is: %lld\n\n", llMaxFileNums);
    fwrite(szMaxFilesSentences, sizeof(szMaxFilesSentences), 1, fpLogFile);

    /* delete all the files*/
    delete_all_files(szFilePath, llMaxFileNums);

    /* begin to do the clear job*/
    printf("Begin to do the Clear job!\n");
    llMaxFileNums --;
    /* begin to do the clear and write job */
    long long llCreatedFiles = 0;
    long long allFilesNamesList[llMaxFileNums];
    for(unsigned long i = 0; i < llMaxFileNums; i++) allFilesNamesList[i] = 0;

    long long nClearBigLooping = 0;

    while(1)
    {
        unsigned long u8IndexNum =  llCreatedFiles % llMaxFileNums;
        if (u8IndexNum == 0)
        {
            printf( "Finished %lld Big Cycle.\n\n",nClearBigLooping);
            nClearBigLooping++;
        }
        /* Clean the file and check for the data */
        if (allFilesNamesList[u8IndexNum] != 0)
        {
            /* get the filenames */
            char szLastFile[256];
            memset(szLastFile, 0, sizeof(szLastFile));
            sentence_sticker(szLastFile, szFilePath, allFilesNamesList[u8IndexNum], 0);

            /* read the file */
            FILE *pLastFile = fopen(szLastFile, "r");
            if (pLastFile == NULL){
                printf("Created Files: %lld; Open file error (while checking data); Files:%s\n", llCreatedFiles, szLastFile);
                if (fpLogFile == NULL)
                {
                    fpLogFile = fopen("DataDetectingLog.log", "w+");
                }
                char szErrorSentences[256] = {0};
                sprintf(szErrorSentences, "Files: %lld; Open file error (while checking data); Files:%lld.bin;\n", llCreatedFiles, allFilesNamesList[u8IndexNum]);
                if ( fwrite(szErrorSentences, sizeof(szErrorSentences), 1, fpLogFile) != 1)
                {
                    printf("Create Files: %lld; Write Report Error; Files: DataDetectingLog.log + %s;\n", llCreatedFiles, szLastFile);
                    return -1;
                }
                return -1;

            }
            fseek(pLastFile, 0, SEEK_SET);
            unsigned char chbeginning_num = allFilesNamesList[u8IndexNum] % 256;
            int bCheckData = check_data(llCreatedFiles, llMaxFileNums, chbeginning_num, pLastFile, allFilesNamesList[u8IndexNum]);
            if (bCheckData != 0)
            {
                return -1;
            }

            /* remove files and make sure it removed successfully */
            fseek(pLastFile, 0,SEEK_END);
            g_llStorageUsed -= ftell(pLastFile);
            fseek(pLastFile, 0, SEEK_SET);
            fclose(pLastFile);
            if (remove(szLastFile) != 0)
            {
                char szErrorSentences[256] = {0};
                sprintf(szErrorSentences, "Create Files: %lld; Remove File Error; File: %lld.bin;\n",llCreatedFiles, allFilesNamesList[u8IndexNum]);
                if (fpLogFile == NULL)
                {
                    fpLogFile = fopen("DataDetectingLog.log", "w+");
                }
                if ( fwrite(szErrorSentences, sizeof(szErrorSentences), 1, fpLogFile) != 1)
                {
                    printf("Create Files: %lld; Write Report Error; Files: DataDetectingLog.log + %s;\n", llCreatedFiles, szLastFile);
                    return -1;
                }

                printf("Create Files: %lld; Remove File Error; File: %s;\n",llCreatedFiles, szLastFile);
                return -1;
            }
        }

        /* get the file name */
        char szNewFileName[256];

        long long llTimeNow = get_time_now();
        if (allFilesNamesList[u8IndexNum] % 256 == llTimeNow % 256) llTimeNow ++;
        allFilesNamesList[u8IndexNum] = llTimeNow;

        sentence_sticker(szNewFileName, szFilePath, allFilesNamesList[u8IndexNum], 0);

        /* write the data into the file */
        write_data(szNewFileName, allFilesNamesList[u8IndexNum], llCreatedFiles);

        /* begin to Write the report sentences into log files */
        char szOutputParagraph[1024];
        report_sentence(szOutputParagraph, llCreatedFiles, nClearBigLooping);
        fprintf(fpLogFile, "%s", szOutputParagraph);
        if (fflush(fpLogFile) != 0)
        {
            printf("Create Files: %lld; Flush Log Error; Files: DataDetectingLog.log + %s;\n", llCreatedFiles, szNewFileName);
        }
        llCreatedFiles++;
    }


}


void sentence_sticker(char* _pszOutputSences, char _szFilePath[], long long _llFileNum, int _bInsertFixedName)
{
    /* Change the number of files into the string type */
    char szStrFileNum[256]; /*! strings to store the number of the files*/
    itoa(_llFileNum, szStrFileNum, 10);

    /* Combine sentences into one paragraph */
    strcpy(_pszOutputSences, _szFilePath);
    if (_bInsertFixedName)
    {
        strcat(_pszOutputSences, FILE_NAME);
    }
    strcat(_pszOutputSences, szStrFileNum);
    strcat(_pszOutputSences, FILE_TYPE);
}

void report_sentence(char* _pszOutputParagraph, unsigned long _u8FileNum, long long _llLoopingTimes)
{
    /* Get the time right now for the first sentences */
    struct tm *tmpShowingTime; /*! struct to store the string */
    time_t tmTimeNow;          /*! time_t to get the time right now */
    time(&tmTimeNow);
    tmpShowingTime = localtime(&tmTimeNow);

    /* Write the time info into the szTimeOutput*/
    char szTimeOutput[256] = {0};                   /*! a string to combine the whole time sentences */
    sprintf(szTimeOutput, "%u-%02u-%02u %02u:%02u:%02u\n", (tmpShowingTime->tm_year + 1900), (tmpShowingTime->tm_mon + 1), tmpShowingTime->tm_mday,
                                                                 tmpShowingTime->tm_hour,          tmpShowingTime->tm_min,       tmpShowingTime->tm_sec);


    /* stick the beginning sentences */
    char szFilesNumOutput[256] = {0};
    char szStorageOutput[256] = {0};
    char szLoopOutput[256] = {0};
    sprintf(szFilesNumOutput, "Created Files       : %lu\n" ,_u8FileNum);
    sprintf(szStorageOutput,  "Storage is now used : %lld\n\n", g_llStorageUsed);
    sprintf(szLoopOutput,     "Looping times is    : %lld\n", _llLoopingTimes);

    /* form the whole sentences */
    strcpy(_pszOutputParagraph, szTimeOutput);
    strcat(_pszOutputParagraph, szFilesNumOutput);
    strcat(_pszOutputParagraph, szLoopOutput);
    strcat(_pszOutputParagraph, szStorageOutput);
}

void delete_all_files(char _szFilePath[], long long _llMaxFilesNum)
{
    for(long long llDeleteNum = _llMaxFilesNum; llDeleteNum >= 0; llDeleteNum--)
    {
        /* get the name of the name of the files that needed to delete */
        char szDeleteFileName[256]; /*! string of delete file name */
        sentence_sticker(szDeleteFileName, _szFilePath, llDeleteNum, 1);

        /* remove the file and check wheather the process is successful */
        int nErrorSignal = remove(szDeleteFileName); /*! Signal of successfully deleting files*/
        if (nErrorSignal != 0 && llDeleteNum != _llMaxFilesNum)
        {
            char szErrorSentences[256] = {0};
            sprintf(szErrorSentences, "Delete Files: %lld; Delete Files Error; File: %lld.bin;\n", llDeleteNum, llDeleteNum);
            if (fpLogFile == NULL)
            {
                fpLogFile = fopen("DataDetectingLog.log", "w+");
            }
            if ( fwrite(szErrorSentences, sizeof(szErrorSentences), 1, fpLogFile) != 1)
            {
                printf("Delete Files: %lld; Write Report Error; Files: DataDetectingLog.log + %s;\n", llDeleteNum, szDeleteFileName);
                exit(-1);
            }

            printf("Delete Files: %lld; Delete Files Error; File: %s;\n", llDeleteNum, szDeleteFileName);
            fclose(fpLogFile);
            exit(-1);
        }
    }
    printf("Hello\n");
}

void write_data(char _szFileName[], long long _llBeginTime, long long _llCreatedFilesNum)
{
    /* Open the file */
    FILE *fpWriteFile = fopen(_szFileName, "w+");
    if (fpWriteFile == NULL){
        printf("Create Files: %lld; Create Files Error (while writing data); Files: %s;\n", _llCreatedFilesNum, _szFileName);

        char szErrorSentences[256] = {0};
        sprintf(szErrorSentences, "Create Files: %lld; Create Files Error (while writing data); Files: %s;\n", _llCreatedFilesNum, _szFileName);
        if (fpLogFile == NULL)
        {
            fpLogFile = fopen("DataDetectingLog.log", "w+");
        }
        if ( fwrite(szErrorSentences, sizeof(szErrorSentences), 1, fpLogFile) != 1)
        {
            printf("Delete Files: %lld; Write Report Error; Files: DataDetectingLog.log + %s;\n", _llCreatedFilesNum, _szFileName);
            fclose(fpLogFile);
            exit(-1);
        }
        fclose(fpLogFile);
        exit(-1);
    }

    fseek(fpWriteFile, 0, SEEK_SET);

    /* create a list. beginning @ the reminder of BeginTime*/
    unsigned char u1BeginWriteNum = _llBeginTime % 256;
    unsigned char aWrittenArray[256];
    for(int wri_posi = 0; wri_posi < 256; wri_posi++)
    {
        aWrittenArray[wri_posi] = u1BeginWriteNum++;
        /* although unsigned char < 256, just make sure it will not >= 256 */
        if (u1BeginWriteNum >= 256) u1BeginWriteNum =0;
    }

    /* begin to write the data into the file */
    for(unsigned long i = 0; i < 10*4*1024; i++)
    {
        unsigned long u8ReturnNumber = fwrite(aWrittenArray, sizeof(aWrittenArray[0]), ARRAY_SIZE(aWrittenArray), fpWriteFile);
        if (u8ReturnNumber != ARRAY_SIZE(aWrittenArray))
        {
            /* Report the Error Senteces into the terminal */
            unsigned long long u8ApproxBytes = i * 256;
            printf("Create Files: %lld; Write Data Error; Files: %s; Bytes: %llu;\n", _llCreatedFilesNum, _szFileName, u8ApproxBytes);

            /* Write the Error Sentences into the .log file */
            char szErrorSentences[256] = {0};
            sprintf(szErrorSentences, "Create Files: %lld; Write Data Error; Files: %s; Bytes: %llu;\n", _llCreatedFilesNum, _szFileName, u8ApproxBytes);
            if (fpLogFile == NULL) {
                fpLogFile = fopen("DataDetectingLog.log", "w+");
            }
            if ( fwrite(szErrorSentences, sizeof(szErrorSentences), 1, fpLogFile) != 1){
                printf("Create Files: %lld; Write Report Error; Files: DataDetectingLog.log + %s;\n", _llCreatedFilesNum, _szFileName);
                fclose(fpLogFile);
                exit(-1);
            }
            fclose(fpLogFile);

            /*Exit the program */
            exit(-1);
        }
    }

    /* record the file size and close the file*/
    fseek(fpWriteFile,0,SEEK_END);
    g_llStorageUsed = g_llStorageUsed + ftell(fpWriteFile);
    fclose(fpWriteFile);
}

long long get_maximum_files_num(char *_pFilePath)
{
    long long lMaxFilesNum = 0; /*! the maximum number of creating files in the storage */

    /* looping to create the files until cannot fseek or fwrite */
    char szMaxFinalFile[256];
    while(1)
    {

        /* create a file*/
        sentence_sticker(szMaxFinalFile, _pFilePath, lMaxFilesNum, 1);
        FILE *fpRandFile = fopen(szMaxFinalFile, "w+");
        if (fpRandFile == NULL)
        {
            fclose(fpRandFile);
            return lMaxFilesNum;
        }

        /* write a number into the file*/
        for (unsigned long i = 0; i < 10*1024*1024; i++){
            unsigned long u8OutSignal = fwrite(RAND_PTR, 1, 1, fpRandFile);
            if(u8OutSignal != 1)
            {
                fclose(fpRandFile);
                return lMaxFilesNum; /*OUT: the number of creating files*/
            }
        }

        /* close the file and add the num of creating files */
        fclose(fpRandFile);
        lMaxFilesNum++;
    }

}

int check_data(long long _llFileNum, unsigned long _u8MaxFilesNum, int _nBeginNum, FILE *_fpCheckFile, long long _llFileName)
{
    for (unsigned long i = 0; i < 10 * 4 * 1024; i++)
    {

        unsigned char aTempRead[256] = {0};/*! Creating a array to read the data in the file */
        fread(aTempRead, sizeof(aTempRead[0]), ARRAY_SIZE(aTempRead), _fpCheckFile);

        /* Check the completeness of the array */
        for (int j = 0; j < 256; j++)
        {
            if ((aTempRead[j] != (int) _nBeginNum + j) &&
                ((int) _nBeginNum + j >= 256 && aTempRead[j] != ((int) _nBeginNum + j - 256)))
            {
                /* Calculate the correct the files that is written incorrectly */
                unsigned long lCorrectFilesNum = _llFileNum;
                if(lCorrectFilesNum >= _u8MaxFilesNum) lCorrectFilesNum = lCorrectFilesNum - _u8MaxFilesNum;

                /* Calculate the correct bytes of the written files */
                unsigned long u8ByteNum;/*! How many bytes that used */
                u8ByteNum = i * 256 + j;

                /* Report the Error Senteces into the terminal */
                printf("Create Files: %lu; Check Data Error; File: %lld.bin; Bytes: %lu;\n", lCorrectFilesNum, _llFileName, u8ByteNum);

                char szErrorSentences[256] = {0};
                sprintf(szErrorSentences, "Create Files: %lu; Check Data Error; File: %lld.bin; Bytes: %lu;\n", lCorrectFilesNum, _llFileName, u8ByteNum);
                if (fpLogFile == NULL) {
                    fpLogFile = fopen("DataDetectingLog.log", "w+");
                }
                if ( fwrite(szErrorSentences, sizeof(szErrorSentences), 1, fpLogFile) != 1){
                    printf("Create Files: %lu; Write Report Error; Files: DataDetectingLog.log + %lld.bin;\n", lCorrectFilesNum, _llFileName);
                    fclose(fpLogFile);
                    exit(-1);
                }
                fclose(fpLogFile);

                return 1; /*OUT: if it is 1, means the data in the file is incorrect*/
            }
        }
    }
    return 0;/*OUT: if it is 0, means the data in the file is correct*/
}

/*get the time write now and return the file name (long long type, need use itoa to transfer) */
long long get_time_now(){
    /* get the time right now and return the time */
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    return time_now.tv_sec * 1000000 + time_now.tv_usec; /*OUT: secs + mircorsecs*/
}

/* Copy and did a little change from the Internet */
char *itoa(unsigned long long _u8Num,char* _pStr,int _nRadix){

    char szIndex[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";   /*! 索引表 */
    unsigned long long u8Num;                                /*! 存放要转换的整数的绝对值,转换的整数可能是负数 */
    int i=0,j,k;                                             /*! i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况 */
                                                             /*! k用来指示调整顺序的开始位置 */
                                                             /*! j用来指示调整顺序时的交换。 */

    /* 获取要转换的整数的绝对值 */
    /* 要转换成十进制数并且是负数 */
    if(_nRadix==10 && _u8Num <0)
    {
        /* 将num的绝对值赋给unum */
        u8Num = (unsigned) -u8Num;

        /* 在字符串最前面设置为'-'号，并且索引加1 */
        _pStr[i++]='-';
    }
    else
    {
        /* 若是num为正，直接赋值给unum */
        u8Num= _u8Num;
    }

    /* 转换部分，注意转换后是逆序的 */
    do{
        int index_num = u8Num%(unsigned)_nRadix;

        /* 取unum的最后一位，并设置为str对应位，指示索引加1 */
        _pStr[i++]=szIndex[index_num];

        /* unum去掉最后一位 */
        u8Num/=_nRadix;

    }while(u8Num);/* 直至unum为0退出循环 */

    /* 在字符串最后添加'\0'字符，c语言字符串以'\0'结束。 */
    _pStr[i]='\0';

    /* 将顺序调整过来 */
    if(_pStr[0]=='-')
    {
        /* 如果是负数，符号不用调整，从符号后面开始调整 */
        k=1;
    }
    else
    {
        /* 不是负数，全部都要调整 */
        k=0;
    }

    char cTemp; /*! 临时变量，交换两个值时用到 */
    for(j=k;j<=(i-1)/2;j++) /*! 头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1 */
    {
        /* 头部赋值给临时变量,尾部赋值给头部,将临时变量的值(其实就是之前的头部值)赋给尾部 */
        cTemp=_pStr[j];
        _pStr[j]=_pStr[i-1+k-j];
        _pStr[i-1+k-j]=cTemp;
    }

    return _pStr; /*OUT:返回转换后的字符串*/

}
