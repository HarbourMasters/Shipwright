#include "OutputFormatter.h"
#include <Globals.h>

void OutputFormatter::Flush()
{
	//if (!Globals::Instance->otrMode) // OTRTODO: MULTITHREADING
	{
		if (col > lineLimit && !Globals::Instance->otrMode)
		{
			str.append(1, '\n');
			str.append(currentIndent, ' ');

			uint32_t newCol = currentIndent + (wordP - word);

			for (uint32_t i = 0; i < wordNests; i++)
				nestIndent[nest - i] -= col - newCol;

			col = newCol;
		}
		else
		{
			str.append(space, spaceP - space);
		}
		spaceP = space;

		str.append(word, wordP - word);
		wordP = word;
		wordNests = 0;
	}
}

int OutputFormatter::Write(const char* buf, int count)
{
	// OTRTODO
	//if (!Globals::Instance->singleThreaded)
		//return 0;

	for (int i = 0; i < count; i++)
	{
		char c = buf[i];

		if (c == ' ' || c == '\t' || c == '\n')
		{
			if (wordP - word != 0)
			{
				Flush();
			}

			if (c == '\n')
			{
				col = 0;
				*spaceP++ = c;
			}
			else if (c == '\t')
			{
				int n = tabSize - (col % tabSize);
				col += n;
				for (int j = 0; j < n; j++)
					*spaceP++ = ' ';
			}
			else
			{
				col++;
				*spaceP++ = c;
			}

			currentIndent = nestIndent[nest];
		}
		else
		{
			col++;

			if (c == '(')
			{
				nest++;
				nestIndent[nest] = col;
				wordNests++;
			}
			else if (c == ')')
			{
				if (nest > 0)
					nest--;
				if (wordNests > 0)
					wordNests--;
			}

			*wordP++ = c;
		}
	}

	return count;
}

int OutputFormatter::Write(const std::string& buf)
{
	return Write(buf.data(), buf.size());
}

thread_local OutputFormatter* OutputFormatter::Instance;

int OutputFormatter::WriteStatic(const char* buf, int count)
{
	return Instance->Write(buf, count);
}

int (*OutputFormatter::StaticWriter())(const char* buf, int count)
{
	Instance = this;
	return &WriteStatic;
}

OutputFormatter::OutputFormatter(uint32_t tabSize, uint32_t indentation, uint32_t lineLimit)
	: tabSize{tabSize}, lineLimit{lineLimit}, col{0}, nest{0}, nestIndent{indentation},
	  currentIndent{indentation}, wordNests(0), wordP{word}, spaceP{space}
{
}

std::string OutputFormatter::GetOutput()
{
	Flush();

	return std::move(str);
}
