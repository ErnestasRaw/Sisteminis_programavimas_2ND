#include <windows.h>
#include <memory>
#include <iostream>
#include <string>

using namespace std;

struct ID3v1 {
    char tag[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    unsigned char genre;
};

LPCWSTR CharToLPCWSTR(const char* charArray) {
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, charArray, -1, NULL, 0);
    if (bufferSize == 0) {
        cerr << "Klaida konvertinant char i LPCWSTR." << endl;
        return nullptr;
    }

    wchar_t* wideStringBuffer = new wchar_t[bufferSize];
    MultiByteToWideChar(CP_UTF8, 0, charArray, -1, wideStringBuffer, bufferSize);

    return wideStringBuffer;
}

unique_ptr<ID3v1> readID3v1(const LPCWSTR filename) {
    HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "Nepavyko atidaryti failo (skaitymas)." << endl;
        return nullptr;
    }

    DWORD dwFileSize = GetFileSize(hFile, NULL);
    LPVOID lpBuffer = VirtualAlloc(NULL, dwFileSize, MEM_COMMIT, PAGE_READWRITE);

    DWORD dwBytesRead;
    ReadFile(hFile, lpBuffer, dwFileSize, &dwBytesRead, NULL);

    ID3v1* id3v1 = new ID3v1;
    memcpy(id3v1, (char*)lpBuffer + dwFileSize - 128, 128);

    VirtualFree(lpBuffer, 0, MEM_RELEASE);
    CloseHandle(hFile);

    return unique_ptr<ID3v1>(id3v1);
}

void writeID3v1(const LPCWSTR filename, const ID3v1& id3v1) {
    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "Nepavyko atidaryti failo (rasymas)." << endl;
        return;
    }

    SetFilePointer(hFile, -128, NULL, FILE_END);
    DWORD dwBytesWritten;
    WriteFile(hFile, &id3v1, 128, &dwBytesWritten, NULL);

    CloseHandle(hFile);
}

int main() {
    const char* filename = "bensound-far.mp3";

    unique_ptr<ID3v1> id3v1 = readID3v1(CharToLPCWSTR(filename));

    if (id3v1) {
        cout << "Pavadinimas: " << id3v1->title << endl;
        cout << "Atlikejas: " << id3v1->artist << endl;
        cout << "Albumas: " << id3v1->album << endl;

        cout << "Iveskite nauja pavadinima: ";
        cin.getline(id3v1->title, 30);
        cout << "Iveskite nauja atlikeja: ";
        cin.getline(id3v1->artist, 30);
        cout << "Iveskite nauja albuma: ";
        cin.getline(id3v1->album, 30);

        writeID3v1(CharToLPCWSTR(filename), *id3v1);
    }

    return 0;
}
