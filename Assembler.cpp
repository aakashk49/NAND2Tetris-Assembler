#include<iostream>
#include<stdio.h>
#include<string>
#include<unordered_map>
using namespace std;

#define Assert(X,Y) if(!(X)) \
{ printf(Y); while (1); }

int gnLine = 0;
unordered_map<string, string> CompTable = {
	{"0","101010"},
	{ "1", "111111" },
	{"-1", "111010"},
	{"D", "001100"},
	{"A", "110000"},
	{"M", "110000"},
	{"!D", "001101"},
	{"!A", "110001"},
	{"!M", "110001"},
	{"-D", "001111"},
	{"-A", "110011"},
	{"-M", "110011"},
	{"D+1", "011111"},
	{"A+1", "110111"},
	{"M+1", "110111"},
	{"D-1", "001110"},
	{"A-1", "110010"},
	{"M-1", "110010"},
	{"D+A", "000010"},
	{"D+M", "000010"},
	{"D-A", "010011"},
	{"D-M", "010011"},
	{"A-D", "000111"},
	{"M-D", "000111"},
	{"D&A", "000000"},
	{"D&M", "000000"},
	{"D|A", "010101"},
	{"D|M", "010101"}
};

unordered_map<string, string> JmpTable = {
	{ "JGT", "001" },
	{ "JEQ", "010" },
	{ "JGE", "011" },
	{ "JLT", "100" },
	{ "JNE", "101" },
	{ "JLE", "110" },
	{ "JMP", "111" }
};

unordered_map<string, int> SymbolTable = {
{ "SP", 0 },
{"LCL", 1},
{"ARG", 2},
{"THIS", 3},
{"THAT", 4},
{"SCREEN", 16384 },
{"KBD", 24576}
};
string convComp(string comp)
{
	string ret;
	//int a = comp.find('M');
	if (comp.find('M') !=-1)
		ret += '1';
	else ret += '0';
	ret += CompTable[comp];
	if (ret.size() != 7)
		while (1);
	return ret;

}
string convDest(string dest)
{
	string ret = "000";
	if (dest.find('M')!=-1)
	ret[2] = '1';
	if (dest.find('D') != -1)
		ret[1] = '1';
	if (dest.find('A') != -1)
		ret[0] = '1';
	return ret;
}


string ConvertC_Ins(string ins)
{
	string bin="111" ;
	string dest, comp, jmp;
	int eq = ins.find('=');
	if (eq >= 0)
	{
		dest = ins.substr(0, eq);
		ins = ins.substr(eq + 1);
	}
	int sc = ins.find(';');
	if (sc >= 0)
	{
		comp = ins.substr(0, sc);
		jmp = ins.substr(sc + 1);
	}
	else if (ins.size() > 0)
	{
		comp = ins;
	}
	//jmp = ins.substr(ins.find(';')+1,0);
	bin +=convComp(comp);
	bin += convDest(dest);
	if (jmp.size() > 0)
	{
		if (jmp.size() != 3)
			while (1);
		if (JmpTable.find(jmp) == JmpTable.end())
			while (1);
		bin += JmpTable[jmp];
	}
	else bin += "000";
	if (bin.size() != 16)
		while (1);
	return bin;
}
int gnNextVarAddr = 16;
string ConvertA_Ins(const string ins)
{
	string bin = "0";
	int num = -1;
	if (ins[0] >= '0'&&ins[0] <= '9')
		num = stoi(ins);
	else
	{
		if (SymbolTable.find(ins) == SymbolTable.end()) //Not Present in ST
			SymbolTable[ins] = gnNextVarAddr++;

		num = SymbolTable[ins];
	}
	char aBinC[15];
	for (int i = 14; i >= 0; --i)
	{
		if ((num > 0))
		{
			if((num % 2) > 0)
				aBinC[i] = '1';
			else 
				aBinC[i] = '0';

			num /= 2;
		}
		else 
			aBinC[i] = '0';
	}
	for (int i = 0; i < 15; ++i)
		bin.push_back(aBinC[i]);
	return bin;
}
void AddReg2SymTable()
{
	string reg = "R";
	char c = '0';
	for (int i = 0; i < 16; ++i)
	{

			SymbolTable[reg += (c + (i%10))] = i;
			reg.pop_back();
			if (i == 9)reg += "1";
	}
}

int main(int argc,char*argv[])
{
	AddReg2SymTable();
	//system("cd");
	char hackFn[50];
	if (argc < 2)
	{
		printf("\nPass File to Convert");
		return 0;
	}
	char line[100];
	FILE* fp = fopen(argv[1], "r");
	char *fn = strtok(argv[1],".");
	strcpy(hackFn, fn);
	strcat(hackFn, ".hack");
	if (!fp)
	{
		printf("\nFile Not found");
		while (1);
	}
	FILE *hp = fopen(hackFn, "w");
	if (!hp)
	{
		printf("\nUnable to Create hack File");
		while (1);
	}
	for (int pass = 0; pass < 2; ++pass)
	{
		while (fgets(line, 100, fp) != NULL)
		{
			if (line[0] != '\n')
			{
				int i = 0;
				while (i < 99 && line[i] == ' ')i++;
				if (line[i] == '/')
					continue;
				else
				{
					int j = 0;
					string ins;
					char c = line[i + j];
					do
					{
						if (pass == 0 && j == 0 && c != '(')
							break;
						ins.push_back(c);
						j++;
						c = line[i + j];
					} while (c != '\n' && c != ' ' && c != '/' && c!=')');
					string bin;
					if (pass == 0)
					{
						if (ins[0] == '(')
							SymbolTable[ins.substr(1)] = gnLine;
						else gnLine++;
					}
					else if (pass == 1 && ins[0] != '(')
					{
						bin = ((ins[0] == '@') ? ConvertA_Ins(ins.substr(1)) : ConvertC_Ins(ins));
						fprintf(hp, "%s\n", bin.c_str());
					}
				}
			}
		}
		if (pass == 0)
			fseek(fp, 0, 0);
	}
	fclose(fp);
	fclose(hp);
}