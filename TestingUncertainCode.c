#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
/* const varibales */
const char  FILE_TYPE[] = ".bin";
const char  FILE_NAME[] = "xxx_";


/* global variable */
long long   g_lStorageUsed = 0;

void sentence_sticker(char* _pszOutputSences, char _szFilePath[], long long _lFileNum, int _bInsertFixedName);
void report_sentence(char* _pszOutputParagraph, unsigned long _u8FileNum);

void write_data(char szFileName[], long long lBeginTime);


int check_data(long long _lFileNum, unsigned long _u8MaxFilesNum, int _nBeginNum, FILE *_pCheckFile, long long _lFileName);
long long get_time_now();


char *itoa(unsigned long long num,char* str,int radix);


int main(){
    int max_files_num = 100;
    long long file_num = 1;
    long long FilesNamesList[max_files_num];
    for(unsigned long i = 0; i < max_files_num; i++) FilesNamesList[i] = 0;
    FILE *logfile;
    logfile = fopen("/mnt/e/Internship_ShanghaiElectric/TestingFlashMemory/DataDetectingLog.log", "w+");

    int counting = 200;
    while(counting){
        long position = file_num % max_files_num;

        if (FilesNamesList[position]) {
            //get the filenames
            char file_name[256];
            sentence_sticker(file_name, "/mnt/e/Internship_ShanghaiElectric/TestingFlashMemory/100Files/", FilesNamesList[position], 0);

            //read the file
            FILE *fp = fopen(file_name, "r");
            fseek(fp, 0, SEEK_SET);
            unsigned char beginning_num = FilesNamesList[position] % 256;
            int CheckStatus = check_data(file_num, max_files_num, beginning_num, fp, FilesNamesList[position]);
            if (CheckStatus){
                printf("Data incorrect");
                exit(-1);
            }

            //remove the file and make sure it removed successfully
            fseek(fp, 0,SEEK_END);
            g_lStorageUsed -= ftell(fp);
            fseek(fp, 0, SEEK_SET);
            fclose(fp);
            if (remove(file_name)) {
                printf("Cannot Remove the file: %s", file_name);
                exit(-1);
            }
        }
        char new_file_name[256];
        long long TimeNow = get_time_now();
        if (FilesNamesList[position] % 256 == TimeNow % 256) TimeNow ++;
        FilesNamesList[position] = TimeNow;
        sentence_sticker(new_file_name, "/mnt/e/Internship_ShanghaiElectric/TestingFlashMemory/100Files/", FilesNamesList[position], 0);
        //write the data into the file
        write_data(new_file_name, FilesNamesList[position]);

        // Begin to Write the report sentences into log files
        char output_sentences[256];
        report_sentence(output_sentences, file_num);
        fprintf(logfile, "%s", output_sentences);
        counting--;
        file_num++;
    }

}

void sentence_sticker(char* _pszOutputSences, char _szFilePath[], long long _lFileNum, int _bInsertFixedName)
{
    /* Change the number of files into the string type */
    char szStrFileNum[256]; /*! strings to store the number of the files*/
    itoa(_lFileNum, szStrFileNum, 10);

    /* Combine sentences into one paragraph */
    strcpy(_pszOutputSences, _szFilePath);
    if (_bInsertFixedName)
    {
        strcat(_pszOutputSences, FILE_NAME);
    }
    strcat(_pszOutputSences, szStrFileNum);
    strcat(_pszOutputSences, FILE_TYPE);
}

void report_sentence(char* _pszOutputParagraph, unsigned long _u8FileNum)
{
    /* Get the time right now for the first sentences */
    struct tm *ptmShowingTime; /*! struct to store the string */
    time_t tmTimeNow;          /*! time_t to get the time right now */
    time(&tmTimeNow);
    ptmShowingTime = localtime(&tmTimeNow);


    char szYears[10], szMons[10], szDays[10]; /*! strings to store year, month, day */
    char szHours[10], szMins[10], szSecs[10]; /*! strings to store hour, minute, second */
    char szTimeOutput[256];                   /*! a string to combine the whole time sentences */

    /* the process of transfer unsigned long type to char[], and combine to one sentences
     * in the form of: yyyy-mm-dd hh:mm:ss >>>>>
     */

    /* transfer the unsigned long year, month, day into string*/
    itoa((ptmShowingTime->tm_year+1900), szYears,10);
    itoa((ptmShowingTime->tm_mon+1), szMons, 10);
    itoa(ptmShowingTime->tm_mday, szDays,10);

    /* The following code is to transder the hour minute and second, if they are < 0, string add "0" >>>*/
    if (ptmShowingTime->tm_hour < 10)
    {
        char szTempHours[10];
        itoa(ptmShowingTime->tm_hour, szTempHours, 10);
        strcpy(szHours, "0");
        strcat(szHours, szTempHours);
    }
    else
    {
        itoa(ptmShowingTime->tm_hour, szHours, 10);
    }

    if (ptmShowingTime->tm_min < 10)
    {
        char szTempMins[10];
        itoa(ptmShowingTime->tm_min, szTempMins, 10);
        strcpy(szMins, "0");
        strcat(szMins, szTempMins);
    }
    else
    {
        itoa(ptmShowingTime->tm_min, szMins, 10);
    }

    if (ptmShowingTime->tm_sec < 10)
    {
        char szTempSecs[10];
        itoa(ptmShowingTime->tm_sec,  szTempSecs, 10);
        strcpy(szSecs, "0");
        strcat(szSecs, szTempSecs);

    }
    else
    {
        itoa(ptmShowingTime->tm_sec,  szSecs, 10);
    }


    /* Combing the sentences together */
    strcpy(szTimeOutput,szYears);
    strcat(szTimeOutput,"-");
    strcat(szTimeOutput,szMons);
    strcat(szTimeOutput,"-");
    strcat(szTimeOutput,szDays);
    strcat(szTimeOutput," ");
    strcat(szTimeOutput,szHours);
    strcat(szTimeOutput,":");
    strcat(szTimeOutput,szMins);
    strcat(szTimeOutput,":");
    strcat(szTimeOutput, szSecs);
    strcat(szTimeOutput,"\n");

    /* define the constant words for the second and third sentences */
    const char REPORT_FILES_SENTENCES[]     = "Created Files       : ";
    const char REPORT_STORAGE_SENTENCES[]   = "Storage is now used : ";

    /* stick the beginning sentences */
    char szFilesNumOutput[256];
    char szStorageOutput[256];
    strcpy(szFilesNumOutput, REPORT_FILES_SENTENCES);
    strcpy(szStorageOutput,   REPORT_STORAGE_SENTENCES);

    /* change the int type to the char type */
    char szStorageInfo[256];
    char szFileInfo[256];
    itoa(_u8FileNum, szFileInfo,10);
    itoa(g_lStorageUsed, szStorageInfo, 10);

    /* begin form the sentences one by one */
    strcat(szFilesNumOutput, szFileInfo);
    strcat(szFilesNumOutput, "\n");
    strcat(szStorageOutput,  szStorageInfo);
    strcat(szStorageOutput,   "\n\n");

    /* form the whole sentences */
    strcpy(_pszOutputParagraph, szTimeOutput);
    strcat(_pszOutputParagraph, szFilesNumOutput);
    strcat(_pszOutputParagraph, szStorageOutput);
}


void write_data(char _szFileName[], long long _lBeginTime)
{
    /* Open the file */
    FILE *pWriteFile = fopen(_szFileName, "w+");
    fseek(pWriteFile, 0, SEEK_SET);

    /* create a list. beginning @ the reminder of BeginTime*/
    unsigned char u1BeginWriteNum = _lBeginTime % 256;
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
        unsigned long u8ReturnNumber = fwrite(aWrittenArray, sizeof(aWrittenArray[0]), ARRAY_SIZE(aWrittenArray), pWriteFile);
        if (u8ReturnNumber != ARRAY_SIZE(aWrittenArray))
        {
            unsigned long long u8ApproxBytes = i * 256;
            printf("Times: %llu; Write Data Error; Files: %s; Bytes: %lu;", u8ApproxBytes, _szFileName, i);
            exit(-1);
        }
    }

    /* record the file size and close the file*/
    fseek(pWriteFile,0,SEEK_END);
    g_lStorageUsed = g_lStorageUsed + ftell(pWriteFile);
    fclose(pWriteFile);
}



int check_data(long long _lFileNum, unsigned long _u8MaxFilesNum, int _nBeginNum, FILE *_pCheckFile, long long _lFileName)
{
    for (unsigned long i = 0; i < 10 * 4 * 1024; i++)
    {

        unsigned char aTempRead[256] = {0};/*! Creating a array to read the data in the file */
        fread(aTempRead, sizeof(aTempRead[0]), ARRAY_SIZE(aTempRead), _pCheckFile);

        /* Check the completeness of the array */
        for (int j = 0; j < 256; j++)
        {
            if ((aTempRead[j] != (int) _nBeginNum + j) &&
                ((int) _nBeginNum + j >= 256 && aTempRead[j] != ((int) _nBeginNum + j - 256)))
            {
                unsigned long lCorrectFilesNum = _lFileNum;
                if(lCorrectFilesNum >= _u8MaxFilesNum) lCorrectFilesNum = lCorrectFilesNum - _u8MaxFilesNum;
                unsigned long u8ByteNum;/*! How many bytes that used */
                u8ByteNum = i * 256 + j;
                printf("Times: %lu; Check Data Error; File: %lld.bin; Bytes: %lu;", lCorrectFilesNum, _lFileName, u8ByteNum);
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
char* itoa(unsigned long long _u8Num,char* _pStr,int _nRadix){

    char szIndex[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";   /*! 索引表 */
    unsigned long long u8Num;                                /*! 存放要转换的整数的绝对值,转换的整数可能是负数 */
    int i=0,j,k;                                             /*! i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况 */
    /*! k用来指示调整顺序的开始位置 */
    /*! j用来指示调整顺序时的交换。 */

    /* 获取要转换的整数的绝对值 */
    if(_nRadix==10 && _u8Num <0)/* 要转换成十进制数并且是负数 */
    {
        u8Num = (unsigned) -u8Num; /* 将num的绝对值赋给unum */
        _pStr[i++]='-';            /* 在字符串最前面设置为'-'号，并且索引加1 */
    }
    else u8Num= _u8Num;            /* 若是num为正，直接赋值给unum*/

    /* 转换部分，注意转换后是逆序的 */
    do{
        int index_num = u8Num%(unsigned)_nRadix;
        _pStr[i++]=szIndex[index_num]; /* 取unum的最后一位，并设置为str对应位，指示索引加1 */
        u8Num/=_nRadix;/* unum去掉最后一位 */

    }while(u8Num);/* 直至unum为0退出循环 */

    _pStr[i]='\0';/* 在字符串最后添加'\0'字符，c语言字符串以'\0'结束。 */

    /* 将顺序调整过来 */
    if(_pStr[0]=='-')
    {
        k=1; /* 如果是负数，符号不用调整，从符号后面开始调整 */
    }
    else
    {
        k=0; /* 不是负数，全部都要调整 */
    }

    char cTemp; /*! 临时变量，交换两个值时用到 */
    for(j=k;j<=(i-1)/2;j++) /*! 头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1 */
    {
        cTemp=_pStr[j];     /*头部赋值给临时变量*/
        _pStr[j]=_pStr[i-1+k-j];/*尾部赋值给头部 */
        _pStr[i-1+k-j]=cTemp;/*将临时变量的值(其实就是之前的头部值)赋给尾部*/
    }

    return _pStr;//OUT:返回转换后的字符串

}