#include <stdio.h>    /*FOR: printf, FILE*/
#include <string.h>   /*FOR: strcpy*/
#include <stdlib.h>   /*FOR: exit*/

const char  FILE_TYPE[] = ".bin";
const char  FILE_NAME[] = "xxx_";
const char  RAND_NUM = 0;
const char *RAND_PTR = &RAND_NUM;


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


long long get_maximum_files_num(char *_pFilePath)
{
    long long lMaxFilesNum = 0; /*! the maximum number of creating files in the storage */

    /* looping to create the files until cannot fseek or fwrite */
    char szMaxFinalFile[256];
    while(1)
    {

        /* create a file*/
        sentence_sticker(szMaxFinalFile, _pFilePath, lMaxFilesNum, 1);
        FILE *pRandFile = fopen(szMaxFinalFile, "w+");
        if (pRandFile == NULL)
        {
            printf("Hello\n");
            return lMaxFilesNum;
        }

        /* write a number into the file*/
        for (unsigned long i = 0; i < 10*1024*1024; i++){
            unsigned int u4OutSignal = fwrite(RAND_PTR, 1, 1, pRandFile);
            if(u4OutSignal != 1)
            {
                fclose(pRandFile);
                return lMaxFilesNum; /*OUT: the number of creating files*/
            }
        }

        /* close the file and add the num of creating files */
        fclose(pRandFile);
        lMaxFilesNum++;
    }

}

void delete_all_files(char _szFilePath[], long long _u8MaxFilesNum)
{
    for(long long u8DeleteNum = _u8MaxFilesNum; u8DeleteNum >= 0; u8DeleteNum--)
    {
        /* get the name of the name of the files that needed to delete */
        char szDeleteFileName[256]; /*! string of delete file name */
        sentence_sticker(szDeleteFileName, _szFilePath, u8DeleteNum, 1);

        /* remove the file and check wheather the process is successful */
        int bErrorSignal = remove(szDeleteFileName); /*! Signal of successfully deleting files*/
        if (bErrorSignal != 0 && u8DeleteNum != _u8MaxFilesNum)
        {
            printf("Times: %lld; Delete Files Error; File: %s;\n", u8DeleteNum, szDeleteFileName);
            exit(-1);
        }
    }
}

int main(int args, char**argv){
    if (args < 2)
    {
        printf("USAGE: $0 <filepath>\n");
        return -1;
    }
    char szFilePath[1024];
    strcpy(szFilePath, argv[1]);
    strcat(szFilePath,"/");

    /* Test the path is valid or not*/
    char szRandFile[200];
    memset(szRandFile, '\0', sizeof(szRandFile));
    sentence_sticker(szRandFile, szFilePath, 100, 1);
    FILE* pTestFile = fopen(szRandFile, "w+");
    if (pTestFile == NULL)
    {
        printf("The File Path is incorrect.\n");
        return -1;
    }
    fclose(pTestFile);
    remove(szRandFile);
    printf("%s\n",szFilePath);

    long long u8MaxFileNums = get_maximum_files_num(szFilePath);
    if(!u8MaxFileNums)
    {
        printf("Cannot Detect Max Files Number. Some errors occure!\n");
        return -1;
    }
    printf("The maximum number for creating files at %s is:", szFilePath);
    printf("%lld\n",u8MaxFileNums);
    delete_all_files(szFilePath, u8MaxFileNums);
}