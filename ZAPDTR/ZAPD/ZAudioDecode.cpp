/**
 * Bruteforcing decoder for converting ADPCM-encoded AIFC into AIFF, in a way
 * that roundtrips with vadpcm_enc.
 */
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>

typedef signed char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

#ifdef _MSC_VER
#define __builtin_bswap16 _byteswap_ushort
#define __builtin_bswap32 _byteswap_ulong
#endif

#define bswap16(x) __builtin_bswap16(x)
#define bswap32(x) __builtin_bswap32(x)
#define BSWAP16(x) x = __builtin_bswap16(x)
#define BSWAP32(x) x = __builtin_bswap32(x)
#define BSWAP16_MANY(x, n)                                                                         \
	for (s32 _i = 0; _i < n; _i++)                                                                 \
	BSWAP16((x)[_i])

#define NORETURN __attribute__((noreturn))
#define UNUSED __attribute__((unused))

typedef struct
{
	u32 ckID;
	u32 ckSize;
} ChunkHeader;

typedef struct
{
	u32 ckID;
	u32 ckSize;
	u32 formType;
} Chunk;

typedef struct
{
	s16 numChannels;
	u16 numFramesH;
	u16 numFramesL;
	s16 sampleSize;
	s16 sampleRate[5];  // 80-bit float
	u16 compressionTypeH;
	u16 compressionTypeL;
} CommonChunk;

typedef struct
{
	s16 MarkerID;
	u16 positionH;
	u16 positionL;
} Marker;

typedef struct
{
	s16 playMode;
	s16 beginLoop;
	s16 endLoop;
} Loop;

typedef struct
{
	s8 baseNote;
	s8 detune;
	s8 lowNote;
	s8 highNote;
	s8 lowVelocity;
	s8 highVelocity;
	s16 gain;
	Loop sustainLoop;
	Loop releaseLoop;
} InstrumentChunk;

typedef struct
{
	s32 offset;
	s32 blockSize;
} SoundDataChunk;

typedef struct
{
	s16 version;
	s16 order;
	s16 nEntries;
} CodeChunk;

typedef struct
{
	u32 start;
	u32 end;
	u32 count;
	s16 state[16];
} ALADPCMloop;

static char usage[] = "input.aifc output.aiff";
static const char *progname, *infilename;
static int framesize = 9;

void fail_parse(const char* fmt, ...)
{
	char* formatted = NULL;
	va_list ap;
	va_start(ap, fmt);
	int size = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);
	if (size >= 0)
	{
		size++;
		formatted = (char*)malloc(size);
		if (formatted != NULL)
		{
			va_start(ap, fmt);
			size = vsnprintf(formatted, size, fmt, ap);
			va_end(ap);
			if (size < 0)
			{
				free(formatted);
				formatted = NULL;
			}
		}
	}

	if (formatted != NULL)
	{
		fprintf(stderr, "%s: %s [%s]\n", progname, formatted, infilename);
		free(formatted);
	}
	exit(1);
}


s32 myrand()
{
	static u64 state = 1619236481962341ULL;
	state *= 3123692312237ULL;
	state += 1;
	return state >> 33;
}

s16 qsample(f32 x, s32 scale)
{
	if (x > 0.0f)
	{
		return (s16)((x / scale) + 0.4999999);
	}
	else
	{
		return (s16)((x / scale) - 0.4999999);
	}
}

void clamp_to_s16(f32* in, s32* out)
{
	f32 llevel = -0x8000;
	f32 ulevel = 0x7fff;

	for (s32 i = 0; i < 16; i++)
	{
		if (in[i] > ulevel)
			in[i] = ulevel;
		if (in[i] < llevel)
			in[i] = llevel;

		if (in[i] > 0.0f)
		{
			out[i] = (s32)(in[i] + 0.5);
		}
		else
		{
			out[i] = (s32)(in[i] - 0.5);
		}
	}
}

s16 clamp_bits(s32 x, s32 bits)
{
	s32 lim = 1 << (bits - 1);
	if (x < -lim)
		return -lim;
	if (x > lim - 1)
		return lim - 1;
	return x;
}

s32 readaifccodebook(FILE* fhandle, s32**** table, s16* order, s16* npredictors)
{
	BSWAP16(*order);
	BSWAP16(*npredictors);
	*table = (s32***)malloc(*npredictors * sizeof(s32**));
	for (s32 i = 0; i < *npredictors; i++)
	{
		(*table)[i] = (s32**)malloc(8 * sizeof(s32*));
		for (s32 j = 0; j < 8; j++)
		{
			(*table)[i][j] = (s32*)malloc((*order + 8) * sizeof(s32));
		}
	}

	for (s32 i = 0; i < *npredictors; i++)
	{
		s32** table_entry = (*table)[i];
		for (s32 j = 0; j < *order; j++)
		{
			for (s32 k = 0; k < 8; k++)
			{
				s16 ts = 0;
				BSWAP16(ts);
				table_entry[k][j] = ts;
			}
		}

		for (s32 k = 1; k < 8; k++)
		{
			table_entry[k][*order] = table_entry[k - 1][*order - 1];
		}

		table_entry[0][*order] = 1 << 11;

		for (s32 k = 1; k < 8; k++)
		{
			s32 j = 0;
			for (; j < k; j++)
			{
				table_entry[j][k + *order] = 0;
			}

			for (; j < 8; j++)
			{
				table_entry[j][k + *order] = table_entry[j - k][*order];
			}
		}
	}
	return 0;
}

ALADPCMloop* readlooppoints(FILE* ifile, s16* nloops)
{
	BSWAP16(*nloops);
	ALADPCMloop* al = (ALADPCMloop*)malloc(*nloops * sizeof(ALADPCMloop));
	for (s32 i = 0; i < *nloops; i++)
	{
		BSWAP32(al[i].start);
		BSWAP32(al[i].end);
		BSWAP32(al[i].count);
		BSWAP16_MANY(al[i].state, 16);
	}
	return al;
}

s32 inner_product(s32 length, s32* v1, s32* v2)
{
	s32 out = 0;
	for (s32 i = 0; i < length; i++)
	{
		out += v1[i] * v2[i];
	}

	// Compute "out / 2^11", rounded down.
	s32 dout = out / (1 << 11);
	s32 fiout = dout * (1 << 11);
	return dout - (out - fiout < 0);
}

void my_decodeframe(u8* frame, s32* decompressed, s32* state, s32 order, s32*** coefTable)
{
	s32 ix[16];

	u8 header = frame[0];
	s32 scale = 1 << (header >> 4);
	s32 optimalp = header & 0xf;

	if (framesize == 5)
	{
		for (s32 i = 0; i < 16; i += 4)
		{
			u8 c = frame[1 + i / 4];
			ix[i] = c >> 6;
			ix[i + 1] = (c >> 4) & 0x3;
			ix[i + 2] = (c >> 2) & 0x3;
			ix[i + 3] = c & 0x3;
		}
	}
	else
	{
		for (s32 i = 0; i < 16; i += 2)
		{
			u8 c = frame[1 + i / 2];
			ix[i] = c >> 4;
			ix[i + 1] = c & 0xf;
		}
	}

	for (s32 i = 0; i < 16; i++)
	{
		if (framesize == 5)
		{
			if (ix[i] >= 2)
				ix[i] -= 4;
		}
		else
		{
			if (ix[i] >= 8)
				ix[i] -= 16;
		}
		decompressed[i] = ix[i];
		ix[i] *= scale;
	}

	for (s32 j = 0; j < 2; j++)
	{
		s32 in_vec[16];
		if (j == 0)
		{
			for (s32 i = 0; i < order; i++)
			{
				in_vec[i] = state[16 - order + i];
			}
		}
		else
		{
			for (s32 i = 0; i < order; i++)
			{
				in_vec[i] = state[8 - order + i];
			}
		}

		for (s32 i = 0; i < 8; i++)
		{
			s32 ind = j * 8 + i;
			in_vec[order + i] = ix[ind];
			state[ind] = inner_product(order + i, coefTable[optimalp][i], in_vec) + ix[ind];
		}
	}
}

void get_bounds(s32* in, s32* decompressed, s32 scale, s32* minVals, s32* maxVals)
{
	s32 minv, maxv;
	if (framesize == 9)
	{
		minv = -8;
		maxv = 7;
	}
	else
	{
		minv = -2;
		maxv = 1;
	}
	for (s32 i = 0; i < 16; i++)
	{
		s32 lo = in[i] - scale / 2;
		s32 hi = in[i] + scale / 2;
		lo -= scale;
		hi += scale;
		if (decompressed[i] == minv)
			lo -= scale;
		else if (decompressed[i] == maxv)
			hi += scale;
		minVals[i] = lo;
		maxVals[i] = hi;
	}
}

void write_header(FILE* ofile, const char* id, s32 size)
{
	fwrite(id, 4, 1, ofile);
	BSWAP32(size);
	fwrite(&size, sizeof(s32), 1, ofile);
}

char* OldMain(char* infilename)
{
	s16 order = -1;
	s16 nloops = 0;
	ALADPCMloop* aloops = NULL;
	s16 npredictors = -1;
	s32*** coefTable = NULL;
	s32 state[16];
	s32 decompressed[16];
	s32 soundPointer = -1;
	s32 currPos = 0;
	s32 nSamples = 0;
	Chunk FormChunk = Chunk();
	ChunkHeader Header = ChunkHeader();
	CommonChunk CommChunk = CommonChunk();
	InstrumentChunk InstChunk;
	SoundDataChunk SndDChunk = SoundDataChunk();
	FILE* ifile = NULL;
	FILE* ofile = NULL;

	if ((ifile = fopen(infilename, "rb")) == NULL)
	{
		fail_parse("AIFF-C file could not be opened");
		exit(1);
	}

	memset(&InstChunk, 0, sizeof(InstChunk));

	BSWAP32(FormChunk.ckID);
	BSWAP32(FormChunk.formType);
	if ((FormChunk.ckID != 0x464f524d) || (FormChunk.formType != 0x41494643))
	{  // FORM, AIFC
		fail_parse("not an AIFF-C file");
	}

	for (;;)
	{
		s32 num = fread(&Header, sizeof(Header), 1, ifile);
		u32 ts = 0;

		if (num <= 0)
			break;

		BSWAP32(Header.ckID);
		BSWAP32(Header.ckSize);

		Header.ckSize++;
		Header.ckSize &= ~1;
		s32 offset = ftell(ifile);

		switch (Header.ckID)
		{
		case 0x434f4d4d:  // COMM
		{
			BSWAP16(CommChunk.numChannels);
			BSWAP16(CommChunk.numFramesH);
			BSWAP16(CommChunk.numFramesL);
			BSWAP16(CommChunk.sampleSize);
			BSWAP16(CommChunk.compressionTypeH);
			BSWAP16(CommChunk.compressionTypeL);
			s32 cType = (CommChunk.compressionTypeH << 16) + CommChunk.compressionTypeL;
			if (cType == 0x56415043 || cType == 0x41445039)
			{  // VAPC or ADP9
				framesize = 9;
			}
			else if (cType == 0x41445035)
			{  // ADP5
				framesize = 5;
			}
			else if (cType == 0x4850434d)
			{  // HPCM
				framesize = 16;
			}
			else
			{
				char comprType[5] = {
					CommChunk.compressionTypeH >> 8, CommChunk.compressionTypeH & 0xFF,
					CommChunk.compressionTypeL >> 8, CommChunk.compressionTypeL & 0xFF, 0};
				fail_parse("file is of the wrong compression type [got %s (%08x)]", &comprType,
				           cType);
			}
			if (CommChunk.numChannels != 1)
			{
				fail_parse("file contains %d channels, only 1 channel supported",
				           CommChunk.numChannels);
			}
			if (CommChunk.sampleSize != 16)
			{
				fail_parse("file contains %d bit samples, only 16 bit samples supported",
				           CommChunk.sampleSize);
			}

			nSamples = (CommChunk.numFramesH << 16) + CommChunk.numFramesL;

			// Allow broken input lengths
			if (nSamples % 16)
			{
				nSamples -= (nSamples % 16);
			}

			if (nSamples % 16 != 0)
			{
				fail_parse("number of chunks must be a multiple of 16, found %d with remainder %d",
				           nSamples, nSamples % 16);
			}
		}
			break;

		case 0x53534e44:  // SSND
			BSWAP32(SndDChunk.offset);
			BSWAP32(SndDChunk.blockSize);
			assert(SndDChunk.offset == 0);
			assert(SndDChunk.blockSize == 0);
			soundPointer = ftell(ifile);
			break;

		case 0x4150504c:  // APPL
			BSWAP32(ts);
			if (ts == 0x73746f63)
			{  // stoc
				u8 len = 0;
				if (len == 11)
				{
					char ChunkName[12];
					s16 version;
					ChunkName[11] = '\0';
					if (strcmp("VADPCMCODES", ChunkName) == 0)
					{
						BSWAP16(version);
						if (version != 1)
						{
							fail_parse("Unknown codebook chunk version");
						}
						readaifccodebook(ifile, &coefTable, &order, &npredictors);
					}
					else if (strcmp("VADPCMLOOPS", ChunkName) == 0)
					{
						BSWAP16(version);
						if (version != 1)
						{
							fail_parse("Unknown loop chunk version");
						}
						aloops = readlooppoints(ifile, &nloops);
						if (nloops != 1)
						{
							fail_parse("Only a single loop supported");
						}
					}
				}
			}
			break;
		}

		fseek(ifile, offset + Header.ckSize, SEEK_SET);
	}

	if (coefTable == NULL)
	{
		fail_parse("Codebook missing from bitstream");
	}

	for (s32 i = 0; i < order; i++)
	{
		state[15 - i] = 0;
	}

	u32 outputBytes = nSamples * sizeof(s16);
	u8* outputBuf = (u8*)malloc(outputBytes);

	fseek(ifile, soundPointer, SEEK_SET);
	s32 fails = 0;
	while (currPos < nSamples)
	{
		u8 input[9];
		u8 encoded[9];
		s32 lastState[16];
		s32 decoded[16];
		s16 guess[16];
		s16 origGuess[16];

		memcpy(lastState, state, sizeof(state));

		// Decode for real
		my_decodeframe(input, decompressed, state, order, coefTable);
		memcpy(decoded, state, sizeof(state));

		// Create a guess from that, by clamping to 16 bits
		for (s32 i = 0; i < 16; i++)
		{
			origGuess[i] = clamp_bits(state[i], 16);
		}

		memcpy(state, decoded, sizeof(state));
		memcpy(outputBuf + currPos * 2, decoded, sizeof(decoded));
		currPos += 16;
	}
	if (fails)
	{
		fprintf(stderr, "%s %d\n", infilename, fails);
	}

	// Write an incomplete file header. We'll fill in the size later.
	fwrite("FORM\0\0\0\0AIFF", 12, 1, ofile);

	// Subtract 4 from the COMM size to skip the compression field.
	write_header(ofile, "COMM", sizeof(CommonChunk) - 4);
	CommChunk.numFramesH = nSamples >> 16;
	CommChunk.numFramesL = nSamples & 0xffff;
	BSWAP16(CommChunk.numChannels);
	BSWAP16(CommChunk.numFramesH);
	BSWAP16(CommChunk.numFramesL);
	BSWAP16(CommChunk.sampleSize);
	fwrite(&CommChunk, sizeof(CommonChunk) - 4, 1, ofile);

	if (nloops > 0)
	{
		s32 startPos = aloops[0].start, endPos = aloops[0].end;
		const char* markerNames[2] = {"start", "end"};
		Marker markers[2] = {{1, startPos >> 16, startPos & 0xffff},
		                     {2, endPos >> 16, endPos & 0xffff}};
		write_header(ofile, "MARK", 2 + 2 * sizeof(Marker) + 1 + 5 + 1 + 3);
		s16 numMarkers = bswap16(2);
		fwrite(&numMarkers, sizeof(s16), 1, ofile);
		for (s32 i = 0; i < 2; i++)
		{
			u8 len = (u8)strlen(markerNames[i]);
			BSWAP16(markers[i].MarkerID);
			BSWAP16(markers[i].positionH);
			BSWAP16(markers[i].positionL);
			fwrite(&markers[i], sizeof(Marker), 1, ofile);
			fwrite(&len, 1, 1, ofile);
			fwrite(markerNames[i], len, 1, ofile);
		}

		write_header(ofile, "INST", sizeof(InstrumentChunk));
		InstChunk.sustainLoop.playMode = bswap16(1);
		InstChunk.sustainLoop.beginLoop = bswap16(1);
		InstChunk.sustainLoop.endLoop = bswap16(2);
		InstChunk.releaseLoop.playMode = 0;
		InstChunk.releaseLoop.beginLoop = 0;
		InstChunk.releaseLoop.endLoop = 0;
		fwrite(&InstChunk, sizeof(InstrumentChunk), 1, ofile);
	}

	// Save the coefficient table for use when encoding. Ideally this wouldn't
	// be needed and "tabledesign -s 1" would generate the right table, but in
	// practice it's difficult to adjust samples to make that happen.
	write_header(ofile, "APPL", 4 + 12 + sizeof(CodeChunk) + npredictors * order * 8 * 2);
	fwrite("stoc", 4, 1, ofile);
	CodeChunk cChunk;
	cChunk.version = bswap16(1);
	cChunk.order = bswap16(order);
	cChunk.nEntries = bswap16(npredictors);
	fwrite("\x0bVADPCMCODES", 12, 1, ofile);
	fwrite(&cChunk, sizeof(CodeChunk), 1, ofile);
	for (s32 i = 0; i < npredictors; i++)
	{
		for (s32 j = 0; j < order; j++)
		{
			for (s32 k = 0; k < 8; k++)
			{
				s16 ts = bswap16(coefTable[i][k][j]);
				fwrite(&ts, sizeof(s16), 1, ofile);
			}
		}
	}

	write_header(ofile, "SSND", outputBytes + 8);
	SndDChunk.offset = 0;
	SndDChunk.blockSize = 0;
	fwrite(&SndDChunk, sizeof(SoundDataChunk), 1, ofile);
	fwrite(outputBuf, outputBytes, 1, ofile);

	// Fix the size in the header
	s32 fileSize = bswap32(ftell(ofile) - 8);
	fseek(ofile, 4, SEEK_SET);
	fwrite(&fileSize, 4, 1, ofile);

	fclose(ifile);
	fclose(ofile);
	return 0;
}