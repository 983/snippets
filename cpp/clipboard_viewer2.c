#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <fcntl.h>
#include <io.h>

const char* clipboard_format_name(UINT format){
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ff729168(v=vs.85).aspx
    switch (format){
        case CF_BITMAP: return "CF_BITMAP";
        case CF_DIB: return "CF_DIB";
        case CF_DIBV5: return "CF_DIBV5";
        case CF_DIF: return "CF_DIF";
        case CF_DSPBITMAP: return "CF_DSPBITMAP";
        case CF_DSPENHMETAFILE: return "CF_DSPENHMETAFILE";
        case CF_DSPMETAFILEPICT: return "CF_DSPMETAFILEPICT";
        case CF_DSPTEXT: return "CF_DSPTEXT";
        case CF_ENHMETAFILE: return "CF_ENHMETAFILE";
        case CF_GDIOBJFIRST: return "CF_GDIOBJFIRST";
        case CF_GDIOBJLAST: return "CF_GDIOBJLAST";
        case CF_HDROP: return "CF_HDROP";
        case CF_LOCALE: return "CF_LOCALE";
        case CF_METAFILEPICT: return "CF_METAFILEPICT";
        case CF_OEMTEXT: return "CF_OEMTEXT";
        case CF_OWNERDISPLAY: return "CF_OWNERDISPLAY";
        case CF_PALETTE: return "CF_PALETTE";
        case CF_PENDATA: return "CF_PENDATA";
        case CF_PRIVATEFIRST: return "CF_PRIVATEFIRST";
        case CF_PRIVATELAST: return "CF_PRIVATELAST";
        case CF_RIFF: return "CF_RIFF";
        case CF_SYLK: return "CF_SYLK";
        case CF_TEXT: return "CF_TEXT";
        case CF_TIFF: return "CF_TIFF";
        case CF_UNICODETEXT: return "CF_UNICODETEXT";
        case CF_WAVE: return "CF_WAVE";
        default: return "UNKNOWN_FORMAT";
    }
}

int main(){
 
    // If there are no clipboard formats, return. 
 
    if (CountClipboardFormats() == 0){
        printf("no clipboard formats\n");
        return -1;
    }
 
    // Open the clipboard. 
 
    if (!OpenClipboard(NULL)){
        printf("failed to open clipboard\n");
        return -1;
    }
 
    UINT uFormat = EnumClipboardFormats(0); 
 
    while (uFormat){
        printf("%6u - %s - ", (unsigned)uFormat, clipboard_format_name(uFormat));
        char szFormatName[80]; 
        if (GetClipboardFormatName(uFormat, szFormatName, sizeof(szFormatName))){
            printf("clipboard format: %s\n", szFormatName);
        }else{
            printf("Unknown clipboard format\n");
        }
        
        if (uFormat == CF_HDROP){
            HANDLE handle = GetClipboardData(CF_HDROP);
            
            UINT nFiles = DragQueryFile(handle, 0xFFFFFFFF, NULL, 0);
            
            printf("%d files:\n", (int)nFiles);
            for (UINT i = 0; i < nFiles; i++){
                char path[1024];
                DragQueryFile(handle, i, path, sizeof(path));
                printf("%i: %s\n", (int)i, path);
            }
            
            printf("\n");
        }else if (uFormat == CF_UNICODETEXT){
            
            // funny characters test: @äöüß§
            // hello in russian: Привет
            // bunch of zero width characters:
            // --->​<---
            
            _setmode(_fileno(stdout), _O_U16TEXT);
            
            WCHAR  *text = (WCHAR*)GetClipboardData(CF_UNICODETEXT);
            wprintf(L"%i chars:\n", (int)wcslen(text));
            wprintf(L"%s\n", text);
            printf("\n");
        }else if (uFormat == CF_TEXT){
            const char *text = (const char*)GetClipboardData(CF_TEXT);
            printf("%s\n", text);
            printf("\n");
        }
 
        uFormat = EnumClipboardFormats(uFormat); 
    } 
    CloseClipboard(); 
 
    return 0;
} 
