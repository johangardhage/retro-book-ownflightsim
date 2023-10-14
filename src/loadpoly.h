// LOADPOLY.CPP
//
// A set of functions for loading a World class
// database of polygon-fill objects.
//
// file reading routines
// Copyright 1993 by Christopher Lampton, Dale Sather and The Waite Group Press

#ifndef LOADPOLY_H
#define LOADPOLY_H

class PolyFile {
protected:
	FILE *fp;

public:
	PolyFile()
	{
		fp = NULL;
	}
	~PolyFile()
	{
		Close();
	}

	int Open(const char *filename)
	{
		fp = fopen(filename, "rt");
		return (fp == NULL);
	}

	void Close()
	{
		if (fp != NULL) {
			fclose(fp);
			fp = NULL;
		}
	}

	char nextchar()
	{
		char ch;

		while (!feof(fp)) {
			if (fread(&ch, 1, 1, fp) != 1) {
				break;
			}
			if (isspace(ch)) {
				continue;
			}

			if (ch == '*') {
				while (1) {
					if (fread(&ch, 1, 1, fp) != 1) {
						break;
					}
					if (ch == '\n') {
						break;
					}
				}
			} else {
				return (ch);
			}
		}
		return(0);
	}

	int getnumber()
	{
		char ch;
		int sign = 1;

		int num = 0;
		if ((ch = nextchar()) == '-') {
			sign = -1;
			ch = nextchar();
		}
		while (isdigit(ch)) {
			num = num * 10 + ch - '0';
			ch = nextchar();
		}
		return(num * sign);
	}
};

static PolyFile _pf;
static int polycount;

#endif
