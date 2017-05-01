#include <Windows.h>
#include <tchar.h>
#include <ShlObj.h>
#include <atlcomcli.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "color_info.h"

ColorInfo parse_xresources_file(std::string);

int wmain(int argc, wchar_t* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: xres2lnk <XResources file> <link file>" << std::endl;
        return 1;
    }

    auto hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (!SUCCEEDED(hr))
    {
        std::cerr << "COM init failed";
        return hr;
    }

    auto buf = new wchar_t[MAX_PATH];
    wchar_t* filePart;
    auto bufLength = GetFullPathNameW(argv[2], MAX_PATH, buf, &filePart);
    
    if (bufLength > MAX_PATH)
    {
        delete[] buf;
        buf = new wchar_t[bufLength];
        bufLength = GetFullPathNameW(argv[2], bufLength, buf, &filePart);
    }

    if (bufLength == 0)
    {
        std::cerr << "GetFullPathName failed" << std::endl;
        return GetLastError();
    }

    // Read xresources file into memory
    std::ifstream in(argv[1], std::ios::in);
    if (!in)
    {
        std::cerr << "Could not read xresources file '" << argv[1] << "'" << std::endl;
        return errno;
    }

    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();

    // Parse xresources file for color info
    auto colorInfo = parse_xresources_file(contents);

    CComPtr<IShellLink> sps1;
    sps1.CoCreateInstance(CLSID_ShellLink);
    hr = CComQIPtr<IPersistFile>(sps1)->Load(buf, 0);

    if (!SUCCEEDED(hr))
    {
        std::cerr << "Could not read link file '" << buf << "'" << std::endl;
        return hr;
    }

    NT_CONSOLE_PROPS* props;
    hr = CComQIPtr<IShellLinkDataList>(sps1)->CopyDataBlock(NT_CONSOLE_PROPS_SIG, (void**)&props);

    // If there's already a data block, remove it and replace
    if (SUCCEEDED(hr))
    {
        CComQIPtr<IShellLinkDataList>(sps1)->RemoveDataBlock(NT_CONSOLE_PROPS_SIG);
    }
    else
    {
        props = (NT_CONSOLE_PROPS*)LocalAlloc(LPTR, sizeof(NT_CONSOLE_PROPS));
        props->dbh.cbSize = sizeof(NT_CONSOLE_PROPS);
        props->dbh.dwSignature = NT_CONSOLE_PROPS_SIG;
        props->dwWindowSize.X = 162;
        props->dwWindowSize.Y = 43;
        props->dwScreenBufferSize.X = 162;
        props->dwScreenBufferSize.Y = 1000;
        props->uHistoryBufferSize = 25;
        props->uNumberOfHistoryBuffers = 4;
        props->uCursorSize = 25;
        props->bQuickEdit = TRUE;
        props->bAutoPosition = TRUE;
        wcscpy(props->FaceName, L"Lucida Console");
        props->wFillAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    }

    for (int i = 0; i < 16; i++)
    {
        props->ColorTable[i] = colorInfo.NumberedColors[i].to_color_ref();
    }

    hr = CComQIPtr<IShellLinkDataList>(sps1)->AddDataBlock(props);

    LocalFree(props);

    if (!SUCCEEDED(hr))
    {
        std::cerr << "Could not add data block" << std::endl;
        return hr;
    }

    hr = CComQIPtr<IPersistFile>(sps1)->Save(buf, TRUE);

    if (!SUCCEEDED(hr))
    {
        std::cerr << "Could not save link data" << std::endl;
    }

    CoUninitialize();
    return 0;
}