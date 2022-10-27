# BinaryTools
C++ classes for reading/writing binary data and some helper functions/classes. Based on C#'s `BinaryReader` and `BinaryWriter`.

## BinaryReader & BinaryWriter
Classes which can read/write binary data to/from a file or memory buffer. Both have functions for the most common primitive types. Ex: `uint32_t`, `int32_t`, `uint64_t`, `int64_t`, `float`, `double`, etc. See `BinaryReader.h` and `BinaryWriter.h` for a full list. The constructor used determines whether the class reads from a file (the constructor provides a file path), or a memory region (it provides a memory address and size). They can also read and write entire structs to or from memory using `ReadToMemory` and `WriteFromMemory`, respectively. 

## Other helpers and included classes
- `Span<T>`: A very simple wrapper around a fixed sized memory region used by ReadAllBytes. You must free the memory the span points to if it's heap allocated.
- `MemoryBuffer`: A simple class which inherits std::streambuf. Used by BinaryReader/Writer when interacting with a memory buffer.
- `ReadAllBytes(const std::string& filePath)`: Function that reads all bytes from a file and returns them in a Span<T>. Since it's using a span you must free the memory it returns once you're done with it.

## Example
This example shows how to read/write files and in memory buffers using `BinaryReader` and `BinaryWriter`.
```c++
#include <BinaryReader.h>
#include <BinaryWriter.h>
#include <string>
#include <iostream>
#include <assert.h>

//You can specify data layouts with a struct and read/write them in one go
struct ExampleHeader
{
    uint32_t Signature = 0xF00F;
    uint32_t Version = 10;
    uint32_t Value0 = 1024;
    float Value1 = 45.2f;
    float Value2 = 800.9f;
};

int main()
{
    //Write test file to your project directory
    std::string testFilePath = "TestFile0.bin";

    //File writing
    {
        BinaryWriter writer(testFilePath);
        writer.WriteUint32(100);
        writer.WriteFloat(512.0f);
        writer.WriteNullTerminatedString("Hello binary!");
        //Write padding bytes to align to value. E.g. position = 13. After align(4), position = 16, the next multiple of 4.
        writer.Align(4);

        //Can also write whole structs/classes to files
        ExampleHeader header;
        writer.WriteFromMemory(&header, sizeof(ExampleHeader));
    }

    //File reading
    {
        BinaryReader reader(testFilePath); //Assumes that the file already exists
        assert(reader.ReadUint32() == 100);
        assert(reader.ReadFloat() == 512.0f);
        assert(reader.ReadNullTerminatedString() == "Hello binary!");
        reader.Align(4);

        //Can also read whole structs/classes from files
        ExampleHeader header;
        reader.ReadToMemory(&header, sizeof(ExampleHeader));
        assert(header.Signature == 0xF00F);
        assert(header.Version == 10);
        assert(header.Value0 == 1024);
        assert(header.Value1 == 45.2f);
        assert(header.Value2 == 800.9f);
    }

    //Reading from memory
    {
        uint32_t someBuffer[5] = { 256, 700, 12, 895, 5784 };
        BinaryReader reader2((char*)&someBuffer, 5 * sizeof(uint32_t));
        assert(reader2.ReadUint32() == 256);
        assert(reader2.ReadUint32() == 700);
        assert(reader2.ReadUint32() == 12);
        assert(reader2.ReadUint32() == 895);
        assert(reader2.ReadUint32() == 5784);
    }
}

```
