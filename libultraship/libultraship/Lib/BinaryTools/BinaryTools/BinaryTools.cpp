#include "BinaryWriter.h"
#include "BinaryReader.h"
#include "Binary.h"
#include <array>

struct TestPod
{
    float x;
    float y;
    float z;
    uint32_t cash;
    int32_t score;
};

int main()
{
    printf("**** Test 1 - Write + Read a few values ****\n");
    //Test writing a few values and reading them back
    {
        {
            printf("Writing some values to file... ");
            BinaryWriter writer("./TestBin1.bin");
            writer.WriteFloat(1232.3f);
            writer.WriteFloat(300.7f);
            writer.WriteFloat(1680.0f);
            writer.WriteUint32(8000);
            writer.WriteInt32(-2003443);
            printf("Done!\n");
        }
        {
            printf("Reading those values back...\n");
            BinaryReader reader("./TestBin1.bin");
            printf("Float: %f\n", reader.ReadFloat());
            printf("Float: %f\n", reader.ReadFloat());
            printf("Float: %f\n", reader.ReadFloat());
            printf("Uint32: %d\n", reader.ReadUint32());
            printf("Int32: %d\n", reader.ReadInt32());
            printf("Done!\n");
        }

    }

    printf("\n\n**** Test 2 - Write + Read a POD struct directly to/from memory ****\n");
    //Test writing a struct from memory, reading it back and casting the data onto it
    {
        //Write data
        {
            TestPod writeData = {};
            writeData.x = 1234.44f;
            writeData.y = 1734.44f;
            writeData.z = 22334.44f;
            writeData.cash = 1003;
            writeData.score = -64230;
            printf("sizeof(TestPod) = %zd\n", sizeof(TestPod));

            printf("Writing POD struct from memory... ");
            BinaryWriter writer("./TestBin2.bin");
            writer.WriteFromMemory(&writeData, sizeof(TestPod));
            printf("Done!\n\n");
        }
        //Read it back
        {
            TestPod readData = {};
            readData.x = 0.00000000f;
            readData.y = 0.00000000f;
            readData.z = 0.00000000f;
            readData.cash = 0;
            readData.score = 0;

            printf("Reading back data directly into POD struct location in memory... ");
            BinaryReader reader("./TestBin2.bin");
            reader.ReadToMemory(&readData, sizeof(TestPod));
            printf("Done!\n");
            printf("Printing values...\n");
            printf("Float: %f\n", readData.x);
            printf("Float: %f\n", readData.y);
            printf("Float: %f\n", readData.z);
            printf("Uint32: %d\n", readData.cash);
            printf("Int32: %d\n", readData.score);
        }
    }

    printf("\n\n**** Test 3 - Read a POD struct directly to/from memory from handmade binary file ****\n");
    //Test reading data from handmade binary file straight into POD struct memory location
    {
        {
            TestPod readData = {};
            readData.x = 0.00000000f;
            readData.y = 0.00000000f;
            readData.z = 0.00000000f;
            readData.cash = 0;
            readData.score = 0;

            printf("Reading data directly into POD struct location in memory... ");
            BinaryReader reader("./TestBin3.bin");
            reader.ReadToMemory(&readData, sizeof(TestPod));
            printf("Done!\n");
            printf("Printing values...\n");
            printf("Float: %f\n", readData.x);
            printf("Float: %f\n", readData.y);
            printf("Float: %f\n", readData.z);
            printf("Uint32: %d\n", readData.cash);
            printf("Int32: %d\n", readData.score);
        }
    }

    printf("\n\n**** Test 4 - Read a POD struct from a file to memory and read data from that memory area with BinaryReader ****\n");
    //Test reading data from handmade binary file straight into POD struct memory location
    {
        {
            TestPod readData = {};
            readData.x = 0.00000000f;
            readData.y = 0.00000000f;
            readData.z = 0.00000000f;
            readData.cash = 0;
            readData.score = 0;

            printf("Reading data directly into memory... ");
            printf("Done!\n");
            auto span = ReadAllBytes("./TestBin3.bin");
            printf("Reading values of memory buffer with BinaryReader... ");
            printf("Done!\n");

            BinaryReader reader(span.Data(), (uint32_t)span.Size());
            printf("Printing values...\n");
            printf("Float: %f\n", reader.ReadFloat());
            printf("Float: %f\n", reader.ReadFloat());
            printf("Float: %f\n", reader.ReadFloat());
            printf("Uint32: %d\n", reader.ReadUint32());
            printf("Int32: %d\n", reader.ReadInt32());
            delete span.Data();
        }
    }

    struct test
    {
        int a;
        int b;
    };

    std::array<test, 3> testArray;
    testArray[0] = test{ 2, 3 };
    testArray[1] = test{ 4, 5 };
    testArray[2] = test{ 6, 7 };
    //Intentionally specifying size of 2 here to see if end() actually points to the end of last element of the span
    Span<test> testSpan(testArray.data(), 2);

    test* begin = testSpan.begin();
    test* end = testSpan.end();
    auto& front = testSpan.front();
    auto& back = testSpan.back();

    auto& zero = testSpan[0];
    auto& one = testSpan[1];
    auto& two = testSpan[2];

    printf("Testing use of range based for loops with Span<T>...\n");
    for (auto& val : testSpan)
        printf("value: {a: %d, b: %d}\n", val.a, val.b);

    auto a = 2;
}
