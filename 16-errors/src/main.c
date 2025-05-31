#include "main.h"

int main(void)
{

    if (debugInit("logFile.log", DEBUG))
    {
        printf("Error opening file \n");
        return EXIT_FAILURE;
    }

    LOG_DEBUG("This is a debug log");
    LOG_INFO("Info log #%d", 1);
    LOG_WARNING("Warning message here");
    LOG_ERROR("Connection failed!");

    debugClose();
    return EXIT_SUCCESS;
}
