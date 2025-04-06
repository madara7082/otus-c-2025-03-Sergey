#include "main.h"

int main(int argc, char** argv)
{
    uint8_t buffer[4];
    long int pos = 0;
    long int tempPos = 0;
    size_t bytes = 0;
    fileTypes fileType = UNKNOWN;
    struct CentralDirectoryFileHeader zipCDFH;
    char* fileName;

    if(argc != 2)
    {
        printf("USAGE: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* inputFile = fopen(argv[1], "r");
    if(!inputFile)
    {
        printf("Failed to open %s for reading!\n", argv[1]);
        return EXIT_FAILURE;
    }

    fread(&buffer, 1, 4, inputFile);
    if (buffer[0]!=0xff || buffer[1]!=0xd8 || buffer[2]!=0xff || buffer[3]!=0xe0)   // передан не jpeg файл
    {
        printf("Unknown file type \n");
    }
    else
    {
        printf("This is a jpeg file \n");
    }

    while(!feof(inputFile))
    {
        bytes = fread(&buffer, 1, 4, inputFile);    // чтение по 4 байта из файла
        if (bytes < 4)                              // устраняем ситуацию с бесконечным чтением
            break;
        if (buffer[0]==0x50 && buffer[1]==0x4b && (buffer[2]==0x03 || buffer[2]==0x05 || buffer[2]==0x07) &&    // нашли сигнатуру zip
           (buffer[3]==0x04 || buffer[3]==0x06 || buffer[3]==0x08))
        {
            if (fileType == UNKNOWN)
            {
                printf("The file contains an archive! \n");
                fileType = EMPTY_ZIP;
            }
        }
        else if(buffer[0]==0x50 && buffer[1]==0x4b && buffer[2]==0x01 && buffer[3]==0x02)   // нашли сигнатуру CentralDirectoryFileHeader
        {
            tempPos = pos;                                                                  // сохраняем текущую позицию, чтобы после выходав вернуться на исходную
            fseek(inputFile, tempPos-=1, SEEK_SET);
            fread(&zipCDFH, 1, sizeof(zipCDFH), inputFile);                                 // читаем структуру zip CentralDirectoryFileHeader
            fileName = malloc(zipCDFH.filenameLength + 1);                                  // выделяем место под названия файлов
            fread(fileName, 1, zipCDFH.filenameLength, inputFile);                          // читаем названия
            puts(fileName);                                                                 // выводим
            free(fileName);                                                                 // чистим кучу
            fileType = NON_EMPTY_ZIP;
        }
        fseek(inputFile, pos++, SEEK_SET);                                                  // смещаем указатель на 1 байт, чтобы не идти по 4 байта и ничего не пропустить
    }

    if (fileType == EMPTY_ZIP)
    {
        printf("Archive is empty \n");
    }

    fclose(inputFile);
    return EXIT_SUCCESS;
}
