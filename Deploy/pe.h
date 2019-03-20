#ifndef PE_H
#define PE_H

#include <QFile>
#include <QString>
#include "igetlibinfo.h"

//Alghoritm of read dll file

//1. find PEMagic
//2. read magic
//3. check arhitecture (PE32 or PE32+)
//4. find on map of PE achitecture peraw addresses the address of section count
//5. find like point 4 address of begin sections headers
//6. read all sections or skip all sections and read only neaded information (rva addresses)
//7. read a size and rva address of import table and import table deley.
//8. convert rva addresses to raw addresses and jomp to begin import tables.
//9. read the all data of import tables and split it.
//10 find needed library of binary file.

struct LIB_META_INFO {
    unsigned short mashine = 0x0;
    unsigned short type = 0x0;
    unsigned int addressImports = 0x0;
    unsigned int sizeImportTable = 0x0;
};

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#define IMAGE_SIZEOF_SHORT_NAME 8

// Directory Entries
#define IMAGE_DIRECTORY_ENTRY_EXPORT          	0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          	1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        	2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       	3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        	4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       	5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG             6   // Debug Directory
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       		7   // (X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    	7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       	8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS           	9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    	10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT  	11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT         		12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   	13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 	14   // COM Runtime descriptor

struct IMAGE_DATA_DIRECTORY {
    DWORD VirtualAddress;
    DWORD Size;
};

struct IMAGE_FILE_HEADER {
  WORD  Machine;
  WORD  NumberOfSections;
  DWORD TimeDateStamp;
  DWORD PointerToSymbolTable;
  DWORD NumberOfSymbols;
  WORD  SizeOfOptionalHeader;
  WORD  Characteristics;
};

struct IMAGE_OPTIONAL_HEADER {
  WORD                 Magic;
  BYTE                 MajorLinkerVersion;
  BYTE                 MinorLinkerVersion;
  DWORD                SizeOfCode;
  DWORD                SizeOfInitializedData;
  DWORD                SizeOfUninitializedData;
  DWORD                AddressOfEntryPoint;
  DWORD                BaseOfCode;
  DWORD                BaseOfData;
  DWORD                ImageBase;
  DWORD                SectionAlignment;
  DWORD                FileAlignment;
  WORD                 MajorOperatingSystemVersion;
  WORD                 MinorOperatingSystemVersion;
  WORD                 MajorImageVersion;
  WORD                 MinorImageVersion;
  WORD                 MajorSubsystemVersion;
  WORD                 MinorSubsystemVersion;
  DWORD                Win32VersionValue;
  DWORD                SizeOfImage;
  DWORD                SizeOfHeaders;
  DWORD                CheckSum;
  WORD                 Subsystem;
  WORD                 DllCharacteristics;
  DWORD                SizeOfStackReserve;
  DWORD                SizeOfStackCommit;
  DWORD                SizeOfHeapReserve;
  DWORD                SizeOfHeapCommit;
  DWORD                LoaderFlags;
  DWORD                NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};

struct IMAGE_NT_HEADERS {
  DWORD                 Signature;
  IMAGE_FILE_HEADER     FileHeader;
  IMAGE_OPTIONAL_HEADER OptionalHeader;
};

struct IMAGE_SECTION_HEADER {
  BYTE  Name[IMAGE_SIZEOF_SHORT_NAME];
  union {
    DWORD PhysicalAddress;
    DWORD VirtualSize;
  } Misc;
  DWORD VirtualAddress;
  DWORD SizeOfRawData;
  DWORD PointerToRawData;
  DWORD PointerToRelocations;
  DWORD PointerToLinenumbers;
  WORD  NumberOfRelocations;
  WORD  NumberOfLinenumbers;
  DWORD Characteristics;
};

class PE : public IGetLibInfo {

private:

//    int defSection(DWORD rva)
//    {
//        for (int i = 0; i < numberOfSection; ++i)
//        {
//            DWORD start = sections[i].VirtualAddress;
//            DWORD end = start + ALIGN_UP(sections[i].VirtualSize, sectionAligment);

//            if(rva >= start && rva < end)
//                return i;
//        }
//        return -1;
//    }

//    DWORD rvaToOff(DWORD rva)
//    {
//        int indexSection = defSection(rva);
//        if(indexSection != -1)
//            return rva - sections[indexSection].VirtualAddress + sections[indexSection].PointerToRawData;
//        else
//            return 0;
//    }

    int findIndexPE(QFile &file);
    bool fillMetaInfo(LIB_META_INFO& info, const QString &file);

    constexpr static unsigned int PE_MAGIC = 0x00004550;
    constexpr static unsigned int INDEX_PE_MAGIC = 0x80;
    constexpr static unsigned int INDEX_MAGIC = 0x18;
    constexpr static unsigned int INDEX_IMPORTS_32 = 104;
    constexpr static unsigned int INDEX_IMPORTS_64 = 120;
    constexpr static unsigned int NUMBER_RVA_AND_SIZES_32 = 92;
    constexpr static unsigned int NUMBER_RVA_AND_SIZES_64 = 108;

public:
    enum class MashineTypesS: unsigned short {
        IMAGE_FILE_MACHINE_UNKNOWN = 0x0,
        IMAGE_FILE_MACHINE_AMD64 = 0x8664,    // x64
        IMAGE_FILE_MACHINE_ARM = 0x1c0,       // ARM little endian
        IMAGE_FILE_MACHINE_ARM64 = 0xaa64,    // ARM64 little endian
        IMAGE_FILE_MACHINE_ARMNT = 0x1c4,     // ARM Thumb-2 little endian
        IMAGE_FILE_MACHINE_I386 = 0x14c,      // Intel 386 or later processors and compatible processors
    };

    enum class RunType: unsigned short {
        _UNKNOWN = 0x0,
        _32bit = 0x10B,
        _64bit = 0x20B,
        _ROM = 0x107,
    };

    bool is32bit(const QString& file, const LIB_META_INFO *info = nullptr);
    bool dependecies(QStringList& lisr, const QString& file,
                     const LIB_META_INFO *info = nullptr);
    PE();

    bool getLibInfo(const QString& lib, LibInfo& info) override;

    ~PE() override;

};

#endif // PE_H
