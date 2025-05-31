#include "main.h"

#pragma pack(1)
typedef struct wav
{
  uint32_t riff;          // RIFF (4 байта)
  uint32_t fileSize;      // Размер файла (4 байта)
  uint32_t wave;          // WAVE (4 байта)
  uint32_t fmt;           //fmt  (4 байта)
  uint32_t fmtSize;       // Размер FMT-блока (4 байта)
  uint16_t audioFormat;   // Аудиоформат (2 байта)
  uint16_t channelNum;    // Количество каналов (2 байта)
  uint32_t freq;          // Частота дискретизации (4 байта)
  uint32_t bitrate;       // Байтрейт (4 байта)
  uint16_t blockSize;     // Размер блока (2 байта)
  uint16_t bitPerSample;  // Бит на семпл (2 байта)
  uint32_t subchunk2Id;   // data (4 байта)
  uint32_t dataSize;      // Размер данных (4 байта)
                          // Сырые аудиоданные (N байт)
};
#pragma pack()

int main(int argc, char** argv)
{
    struct wav wavFile;
    uint8_t* wavData = NULL;
    long int pos = 0;
    long int dataCounter = 0;
    size_t bytes = 0;

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

    fread(&wavFile, 1, sizeof(wavFile), inputFile);   // читаем структуру
    wavData = malloc(wavFile.dataSize);           // выделяем место под названия файлов
    if (!wavData)
    {
      printf("Not enought memory\n");
      return EXIT_FAILURE; 
    }
    printf("%x \n", wavFile.dataSize);
    fseek(inputFile, sizeof(wavFile), SEEK_SET);
    fread(wavData, 1, wavFile.dataSize, inputFile);  // чтение аудиоданных

    fclose(inputFile);
    return EXIT_SUCCESS;
}