#include "encoder.h"


int main(int argc, char** argv)
{
    int c = 0;
    uint8_t difference = 0;
	uint8_t prefix = 0;

    if(argc != 4)
    {
        printf("USAGE: %s <input file> <encoding> <output file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* inputFile = fopen(argv[1], "r");
    if(!inputFile)
    {
        printf("Failed to open %s for reading!\n", argv[1]);
        return EXIT_FAILURE;
    }

    FILE* outputFile = fopen(argv[3], "w");
	if(!outputFile)
    {
        printf("Failed to open %s for writing!\n", argv[3]);
        return EXIT_FAILURE;
    }

    if (!strcmp(argv[2], "cp1251") || (!strcmp(argv[2], "iso-8859-5")))
    {
        while((c = getc(inputFile)) != EOF)			// читаем данные в int, чтобы не остановиться на первой букве я
        {
            if (c > 0x7f)							// отличаются от utf-8 после появления старшего бита
            {
                if (!strcmp(argv[2], "cp1251"))
                {
                    if ((c >= 0xc0) && (c <= 0xef))		// от А до п
                    {
                        difference = 0x30;
                        prefix = 0xd0;
                    }
                    else if (c == 0xa8)					// буква Ё
                    {
                        difference = 0x27;
                        prefix = 0xd0;
                    }
                    else if (c == 0xb8)					// буква ё
                    {
                        difference = 0x27;
                        prefix = 0xd0;
                    }
                    else if (c >= 0xf0)					// от р до я
                    {
                        difference = 0x70;
                        prefix = 0xd1;
                    }
                }
                else                                    // iso-8859-5
                {
                    if ((c >= 0xb0) && (c <= 0xdf))		// от А до п
                    {
                        difference = 0x20;
                        prefix = 0xd0;
                    }
                    else if (c == 0xa1)					// буква Ё
                    {
                        difference = 0x20;
                        prefix = 0xd0;
                    }
                    else if (c == 0xf1)					// буква ё
                    {
                        difference = 0x60;
                        prefix = 0xd1;
                    }
                    else if (c >= 0xe0)					// от р до я
                    {
                        difference = 0x60;
                        prefix = 0xd1;
                    }
                }
                putc(prefix, outputFile);
                putc(c-difference, outputFile);
            }
            else									    // идентично utf-8, просто перекладываем
                putc(c, outputFile);
        }
    }
    else if (!strcmp(argv[2], "koi8-r"))
    {
        while((c = getc(inputFile)) != EOF)			    // читаем данные в int, чтобы не остановиться на первой букве я
        {
            encodeToUtf8(c, outputFile);                // преобразуем и пишем в файл
        }
    }
    else
    {
        printf("Unknown encoding");
    }
    fclose(inputFile);
    fclose(outputFile);
    return EXIT_SUCCESS;
}
