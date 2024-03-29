#include "gmupx.h"

static const char *VersionString[] = {
    "",
    "8.0",
    "8.1",
    "8.2",
};

static const uint DataPointer[] = {
    0,
    DATA_POINTER_80,
    DATA_POINTER_81,
    DATA_POINTER_82,
};

static const uint DataLocation[] = {
    0,
    DATA_ADDRESS_80,
    DATA_ADDRESS_81,
    DATA_ADDRESS_82,
};

int main(int argc, const char *argv[])
{
    // Check UPX
    bool hasUpx = systemf("where upx /Q") == 0;

    if (!hasUpx)
        printf(
            "Cannot detect UPX on your system.\n"
            "Patching process will be inefficient and results in\n"
            "slower game loading.\n\n");

    // Check argument
    if (argc != 2)
        fail("Usage: %s <game maker exe>", argv[0]);

    const char *input = argv[1];

    // Check file
    printf("Checking file <%s>...\n", input);

    if (!fexist(input))
        fail("Cannot open file, %s", strerror(errno));

    FILE *fInput = fopen(input, "rb");

    // Check executable file
    if (fcmp(fInput, MZ_STUB, sizeof(MZ_STUB), MZ_STUB_ADDRESS) != 0)
        fail("File is not an executable.");

    // Check compressed file
    if (fcmp(fInput, UPX_STUB, sizeof(UPX_STUB), UPX_STUB_ADDRESS) == 0)
        fail("File is already compressed with UPX.");

    // Check version
    const gm_version version =
        freaduint(fInput, DATA_POINTER_80) == DATA_ADDRESS_80 ? GM_80 :
        freaduint(fInput, DATA_POINTER_81) == DATA_ADDRESS_81 ? GM_81 :
        freaduint(fInput, DATA_POINTER_82) == DATA_ADDRESS_82 ? GM_82 :
        GM_UNKNOWN;

    if (version == GM_UNKNOWN)
        fail("Cannot detect a Game Maker 8.0, 8.1, or 8.2 game.");

    printf("Detected a Game Maker %s game.\n", VersionString[version]);

    fclose(fInput);

    // Create backup
    char backup[1024];
    sprintf(backup, "%s.bak", input);

    fcopy(input, backup);
    if (!fexist(backup))
        fail("Failed to create backup file.");

    printf("Backup file created on <%s>.\n", backup);

    if (hasUpx &&
        prompt(
            "Detected UPX on your system.\n"
            "Do you want to compress the executable?"))
    {
        char
            tempFile[MAX_PATH];
        FILE
            *tempHandle;
        uint
            runnerSize,
            runnerUpxSize;

        // Copy runner to temp
        if (GetTempFileName(".", "gmupx", 0, tempFile) == 0)
            fail("Failed to create temporary file.\n");

        tempHandle = fopen(tempFile, "wb");

        runnerSize = DataLocation[version];
        char *runnerData = malloc(runnerSize);

        fInput = fopen(input, "rb+");
        fread(runnerData, sizeof(char), runnerSize, fInput);

        fwrite(runnerData, sizeof(char), runnerSize, tempHandle);
        free(runnerData);

        fclose(tempHandle);

        // Get compressed runner size
        if (systemf("upx --lzma %s -qqq", tempFile))
            fail("Failed to test executable compression.\n");

        tempHandle = fopen(tempFile, "rb");
        runnerUpxSize = fsize(tempHandle);

        fclose(tempHandle);
        remove(tempFile);

        // Patch original executable
        fseek(fInput, DataPointer[version], SEEK_SET);
        if (fwrite(&runnerUpxSize, sizeof(uint), 1, fInput) != 1)
            fail("Failed to patch executable.");

        fclose(fInput);

        // Compress original executable
        if (systemf("upx --lzma %s -qqq", input))
           fail("Failed to compress executable.\n");

        /**
         * TODO add statictics: 
         * file size, file size upx,
         * runner size, runner size upx,
         * data size
         **/
    }
    else if (prompt(
        "Inefficient patching will be performed. Continue?"))
    {
        uint buffer = 0;
        fInput = fopen(input, "rb+");
        fseek(fInput, DataPointer[version], SEEK_SET);
        fwrite(&buffer, sizeof(uint), 1, fInput);
        fclose(fInput);
    }

    printf("Done.\n");

    return EXIT_SUCCESS;
}

bool fexist(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f) fclose(f);
    return f != NULL;
}

uint fcmp(FILE *file, const char *str, size_t size, long offset)
{
    int p = 0;
    fseek(file, offset, SEEK_SET);
    while (--size)
        if (fgetc(file) != str[p++])
            break;
    return size;
}

uint freaduint(FILE *file, long offset)
{
    uint buffer;
    fseek(file, offset, SEEK_SET);
    fread(&buffer, sizeof(buffer), 1, file);
    return buffer;
}

void fcopy(const char *source, const char *dest)
{
    char buffer[4096];
    FILE *in = fopen(source, "rb");
    FILE *out = fopen(dest, "wb");

    while (!feof(in))
    {
        size_t bytes = fread(buffer, sizeof(char), sizeof(buffer), in);
        if (bytes)
            fwrite(buffer, sizeof(char), bytes, out);
    }

    fclose(in);
    fclose(out);
}

int systemf(const char *format, ...)
{
    char command[1024];
    va_list args;
    va_start(args, format);
    vsprintf(command, format, args);
    va_end(args);
    return system(command);
}

int prompt(const char *question)
{
    int c;
    printf("%s [y/n] ", question);
    while (c = getchar(), c != 'y' && c != 'n');
    return c == 'y';
}

long fsize(FILE *stream)
{
    long p1, p2;
    p1 = ftell(stream);
    fseek(stream, 0, SEEK_END);
    p2 = ftell(stream);
    return p2 - p1;
}