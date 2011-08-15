// convert.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <wchar.h>
#include <vector>

void ConvertFile(std::string ifname, std::string ofname, std::string mode);
void WStringReplace(std::wstring *str, std::wstring find, std::wstring replace);
void StringReplace(std::string *str, std::string find, std::string replace);
std::string I2A(int i);
void WStringReplaceBlock(std::wstring *str, std::wstring find, std::wstring replace, std::wstring block);

int main(int argc, char* argv[])
{
	// read arguments
	std::string mode = "-addxml";	// -removemotxml -addxmlfull
	std::string ofile = "out.xml";
	std::vector<std::string> ifiles;
	bool flag_out = false;
	for(int i=1; i<argc; i++)
	{
		std::string arg = argv[i];
		if( arg.at(0) == '-' )
		{
			if( !arg.compare("-addxmlfull") ) mode = "-addxmlfull";
			if( !arg.compare("-removemotxml") ) mode = "-removemotxml";
			if( !arg.compare("-fixc") ) mode = "-fixc";
			if( !arg.compare("-o") ) flag_out = true;
		}
		else
		{
			if( flag_out ) ofile = arg;
			else ifiles.push_back(arg);

			flag_out = false;
		}
	}

	if( ifiles.size() == 1 )
	{
		ConvertFile(ifiles[0], ofile, mode);
	}
	if( ifiles.size() > 1 )
	{
		if( ofile.find("%d",0) == std::wstring::npos )
			StringReplace(&ofile, ".", "%d.");
		for(int i=0; i<(int)ifiles.size(); i++)
		{
			std::string ofname = ofile;
			StringReplace(&ofname, "%d", I2A(i).c_str());

			ConvertFile(ifiles[i], ofname, mode);
		}
	}
	/*
	else	// convert files and write in xml - file
	{
		std::wofstream o_f(ofile.c_str());
		o_f << "<admst>\n";
		for(int i=0; i<(int)ifiles.size(); i++)
		{
			o_f << "<admst:template match=\"PrintFile" << ifiles[i].c_str() << "\">\n";
			ConvertFile(ifiles[i], o_f, "-addxml");
			o_f << "</admst:template>\n";
		}
		o_f << "</admst>\n";
		o_f.close();
	}
	*/


}

void ConvertFile(std::string ifname, std::string ofname, std::string mode)
{
	// open files
	std::wifstream in_f(ifname.c_str());
	std::wofstream o_f(ofname.c_str());
	std::wstring str;
	{
		int len;
		wchar_t *buffer;
		in_f.seekg (0, std::ios::end);
		len = in_f.tellg();
		in_f.seekg (0, std::ios::beg);
		buffer = (wchar_t *)malloc(len*sizeof(wchar_t));
		in_f.read(buffer,len);
		for(int i=0; i<len; i++)
			str += buffer[i];
		free(buffer);
	}

	// convert
	std::wstring line;
	bool in_tag = false;
	for(int pos=0; pos<(int)str.length()+1; pos++)
	{
		wchar_t ch = (pos==(int)str.length()) ? L'\n' : str.at(pos);
		if( ch == L'\n' )
		{
			// detect if it is programm line or adms-line
			// input: in_tag, line
			// output: prog
			bool prog = true;
			if( !in_tag )
			{
				for(int j=0; j<(int)line.length(); j++)
				{
					wchar_t ch2 = line.at(j);
					if( ch2 == L'<' ) {prog = false;break;}
					if( ch2 == L'-' )
						if( line.find(L"-->") != std::string::npos ) {prog=false;break;}
					if( ch2 == L' ' ) continue;
					if( ch2 == L'\t' ) continue;
					break;
				}
				if( !prog && !line.empty() && (line.at(line.length()-1) != L'>') )
					in_tag = true;
			}
			else
			{
				prog = false;
				if( !line.empty() && (line.at(line.length()-1) == L'>') )
					in_tag = false;
			}
			

			// replace
			// cx: % is %% and $ is \$
			//WStringReplace(&line, L"&apos;", L"'");	// no need
			//WStringReplace(&line, L"&amp;", L"&");	// no need
			if( !mode.compare("-addxml") )	// cx -> adms
				if( prog )
				{
					WStringReplace(&line, L"&", L"&amp;");
					WStringReplace(&line, L"<", L"&lt;");
					WStringReplace(&line, L">", L"&gt;");
					WStringReplaceBlock(&line, L"\\", L"\\\\", L"\\$");
					WStringReplace(&line, L"\"", L"&quot;");
				}
			if( !mode.compare("-removemotxml") ) // adms -> cx
				if( prog )
				{
					WStringReplace(&line, L"&lt;", L"<");
					WStringReplace(&line, L"&gt;", L">");
					WStringReplace(&line, L"&quot;", L"\"");	
					WStringReplaceBlock(&line, L"\\n", L"\n", L"\\\\n");
					WStringReplace(&line, L"\\\\", L"\\");
					WStringReplace(&line, L"&amp;", L"&");
					
				}
			if( !mode.compare("-addxmlfull") )
				if( prog )
				{
					WStringReplace(&line, L"&", L"&amp;");
					WStringReplace(&line, L"<", L"&lt;");
					WStringReplace(&line, L">", L"&gt;");
					WStringReplace(&line, L"$", L"\\$");
					WStringReplace(&line, L"%", L"%%");
					WStringReplaceBlock(&line, L"\\", L"\\\\", L"\\$");
					WStringReplace(&line, L"\"", L"&quot;");

				}
			if( !mode.compare("-fixc") ) // c -> cx
				if( prog )
				{
					WStringReplace(&line, L"$", L"\\$");
					WStringReplace(&line, L"%", L"%%");						
				}

			o_f << line;
			if( pos!=(int)str.length() ) o_f << L"\n";

			line.clear();
		}
		else
			line += ch;
	}

	o_f.close();
	in_f.close();
}

void WStringReplace(std::wstring *str, std::wstring find, std::wstring replace)
{
	for(int pos=0; (pos=(int)str->find(find,pos)) != std::wstring::npos; pos+=(int)replace.length())
		str->replace(pos, find.length(), replace);
}

void WStringReplaceBlock(std::wstring *str, std::wstring find, std::wstring replace, std::wstring block)
{
	for(int pos=0; (pos=(int)str->find(find,pos)) != std::wstring::npos; pos+=(int)replace.length())
	{
		bool rep = true;
		for(int d=0; d<((int)block.length()-(int)find.length()+1); d++)
		{
			if( pos - d < 0 ) continue;
			if( !str->substr(pos-d, block.length()).compare(block) ) rep = false;
		}
		if( rep ) 
			str->replace(pos, find.length(), replace);
	}
}

void StringReplace(std::string *str, std::string find, std::string replace)
{
	for(int pos=0; (pos=(int)str->find(find,pos)) != std::string::npos; pos+=(int)replace.length())
		str->replace(pos, find.length(), replace);
}

std::string I2A(int i)
{
	std::stringstream ss;
	ss << i;
	std::string str;
	str = ss.str();
	return(str);
}

