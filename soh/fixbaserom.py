from os import path
import sys
import struct
import hashlib


def get_str_hash(byte_array):
    return str(hashlib.md5(byte_array).hexdigest())


# If the baserom exists and is correct, we don't need to change anything
if path.exists("baserom_non_mq.z64"):
    with open("baserom_non_mq.z64", mode="rb") as file:
        fileContent = bytearray(file.read())
        if get_str_hash(fileContent) == "9c1d795429220f5389045693a011b8f6":
            print("Found valid baserom - exiting early")
            sys.exit(0)

# Determine if we have a ROM file
romFileExtensions = ["z64", "n64", "v64"]

def find_baserom_original():
    for romFileExtLower in romFileExtensions:
        for romFileExt in (romFileExtLower, romFileExtLower.upper()):
            romFileNameCandidate = "baserom_original_non_mq." + romFileExt
            if path.exists(romFileNameCandidate):
                return romFileNameCandidate
    return None

romFileName = find_baserom_original()

if romFileName is None:
    print("Error: Could not find baserom_original_non_mq.z64/baserom_original_non_mq.n64/baserom_original_non_mq.v64.")
    sys.exit(1)

# Read in the original ROM
print("File '" + romFileName + "' found.")
with open(romFileName, mode="rb") as file:
    fileContent = bytearray(file.read())

# Strip the overdump
print("Stripping overdump...")
fileContent = fileContent[0:0x3600000]

fileContentLen = len(fileContent)

# Check if ROM needs to be byte/word swapped
# Little-endian
if fileContent[0] == 0x40:
    # Word Swap ROM
    print("ROM needs to be word swapped...")
    words = str(int(fileContentLen/4))
    little_byte_format = "<" + words + "I"
    big_byte_format = ">" + words + "I"
    tmp = struct.unpack_from(little_byte_format, fileContent, 0)
    struct.pack_into(big_byte_format, fileContent, 0, *tmp)

    print("Word swapping done.")

# Byte-swapped
elif fileContent[0] == 0x37:
    # Byte Swap ROM
    print("ROM needs to be byte swapped...")
    halfwords = str(int(fileContentLen/2))
    little_byte_format = "<" + halfwords + "H"
    big_byte_format = ">" + halfwords + "H"
    tmp = struct.unpack_from(little_byte_format, fileContent, 0)
    struct.pack_into(big_byte_format, fileContent, 0, *tmp)

    print("Byte swapping done.")

# Patch the header
print("Patching header...")
fileContent[0x3E] = 0x50

for i in range(0x35CF000, len(fileContent)):
    fileContent[i] = 0xFF

# Check to see if the ROM is a "vanilla" Debug ROM
str_hash = get_str_hash(bytearray(fileContent))
if str_hash != "9c1d795429220f5389045693a011b8f6":
    print("Error: Expected a hash of 9c1d795429220f5389045693a011b8f6 but got " + str_hash + ". " +
          "The baserom has probably been tampered, find a new one")

    if str_hash == "32fe2770c0f9b1a9cd2a4d449348c1cb":
        print("The provided baserom is a rom which has been edited with ZeldaEdit and is not suitable for use with decomp. " +
              "Find a new one.")

    sys.exit(1)

# Write out our new ROM
print("Writing new ROM 'baserom_non_mq.z64'.")
with open("baserom_non_mq.z64", mode="wb") as file:
    file.write(bytes(fileContent))

print("Done!")
