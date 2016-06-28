#include <iostream>
#include <fstream>
#include <string>
using namespace std;
/*
*		my_sim.cpp is a simulator designed to model the functioning
*		of the H1 computer. The program reads in a binary machine 
*		code file and decodes the information, performing operations
*		on registers.
*		Author: Shane Kelly
*/

int main(int argc, char* argv[])
{
	// declare input stream for binary file
	ifstream inStream;
	string fileName;
	// check to see if file name was passed as command line argument
	if (argc != 2)
	{
		cout << "Please enter a filename: " << endl;
		cin >> fileName;
	}
	// if command line argument is present then set to fileName
	else{
		fileName = string(argv[1]);
	}

	// check to see if .mac extension is present
	int length = sizeof(fileName);
	size_t lastindex = fileName.find_last_of(".");
	
	// if .mac extension is missing then concat the extension
	if (lastindex == string::npos)
	{	
		fileName = fileName + ".mac";
	}
	// if .mac extension is present then continue with fileName as is
	else if (fileName.substr(lastindex, length).compare(".mac") == 0)
	{
		fileName = fileName;
	}
	
	// try to open binary input stream from fileName
	inStream.open(fileName.c_str(), ios::binary);
	// if file is not found then alert user and halt operations
	if ( !inStream )
	{
		cout << fileName << " not found in present directory ... halting" 
		<< endl;
		return 1;
	}
	
	// begin sim
	cout << "beginning simulation of " << fileName << endl;
	// declare and initialize registers
	short ac = 0, pc = 0, sp = 0, ir = 0, address = 0, temp = 0;
	// create an array to represent H1 memory
	short mem[4095];
	// create int sized variable to retrieve and pass opcodes
	unsigned int opcode;
	short word;
	// initialize all registers to 0x0000
	for (int i = 0; i < 4096; i++)
		mem[i] = 0x0000;

	// perform loop until end of file is reached
	while (!inStream.eof())
	{
		// read in 16-bit words to word variable from binary file
		inStream.read((char *)&word, sizeof(word));
		// sequentially fill registers with words from binary file
		mem[ir++] = word;
	}
		// set instruction register to 0
		ir = 0;

	// looping mechanism for performance of H1 operations
	while (true)
	{
		
		// cout << hex << "0000 = " << mem[pc] << endl;
		// set ir with data from first register in memory
		ir = mem[pc++];
		// cout << "ir before bit shift = " << hex << ir << endl;
		// perform bit shifting operations and remove signed extension
		if ((ir & 0xfff0) == 0xfff0)
      	opcode =  0xffff & ir;
      else if ((ir & 0xff00) == 0xff00)
      	opcode = (0xffff & ir) >> 4;
      else if ((ir & 0xf000) == 0xf000)
      	opcode = (0xffff & ir) >> 8;
      else
      	opcode = (0xffff & ir) >> 12;

		/*cout << "opcode = " << hex << opcode << endl;
		cout << "ir = " << hex << ir << endl;
		cout << "ac = " << ac << endl;
		cout << "sp = " << sp << endl;
		cout << "pc = " << pc << endl;
      code block for tracing / debugging */
		int x;
		// switch statement, performs relevant operations for a given opcode
		switch (opcode)
		{
			case 0: ac = mem[ir & 0x0fff];									//ld
				break;
			case 1: mem[ir & 0x0fff] = ac;									//st
				break;
			case 2: ac = ac + mem[(ir & 0x0fff)];							//add
				break;
			case 3: ac = ac - mem[(ir & 0x0fff)];							//sub
				break;
			case 4: ac = mem[(sp & 0x0fff) + (ir & 0x0fff)];			//ldr
				break;
			case 5: mem[(sp & 0x0fff) + (ir & 0x0fff)] = ac;			//str
				break;
			case 6: ac = ac + mem[(sp & 0x0fff) + (ir & 0x0fff)];		//addr
				break;
			case 7: ac = ac - mem[(sp & 0x0fff) + (ir & 0x0fff)];		//subr
				break;
			case 8: ac = (ir & 0x0fff);										//ldc
				break;
			case 9: pc = (ir & 0x0fff);										//ja
				break;
			case 0xa: if (ac >= 0){ 											//jzop
					pc = (ir & 0x0fff);
				}
				break;
			case 0xb: if (ac < 0){												//jn
					pc = (ir & 0x0fff);
				}
				break;
			case 0xc: if (ac == 0x0000){										//jz
					pc = (ir & 0x0fff);
				}
				break;
			case 0xd: if (ac != 0){												//jnz
					pc = (ir & 0x0fff);
				}
				break;
			case 0xe: 
				mem[--sp & 0x0fff] = pc;										//call
				pc = (ir & 0x0fff);
				break;
			case 0xf0: pc = mem[0x0fff & sp++];								//ret
				break;
			case 0xf1: ac = mem[ac];											//ldi
				break;
			case 0xf2: mem[ac] = mem[0x0fff & sp++];						//sti
				break;
			case 0xf3: mem[--sp & 0x0fff] = ac; 							//push
				break;
			case 0xf4: ac = mem[0x0fff & sp++];								//pop
				break;
			case 0xf5: sp = sp - (ir & 0x00ff);								//aloc
				break;
			case 0xf6: sp = sp + (ir & 0x00ff);								//dloc
				break;
			case 0xf7: temp = ac;												//swap
				ac = sp;
				sp = temp;
				break;
			case 0xfd:
				cout << dec << ac << endl;
				break;
			case 0xfff7: temp = ac;
				while (true)											//sout
				{
					cout << hex <<(char)mem[temp++];
					if (mem[temp] == 0x0000)
					{
						break;
					}
				}
				break;
			case 0xfff9: cout << hex << ac << endl;						//hout
				break;
			case 0xfffd: cout << ac << endl;
				break;
			case 0xffff:															//halt
				return 0;
				break;
			default:
				cout << "ERROR OPCODE NOT SUPPORTED!" << endl;
				return 2;
				break;
			
		}
		
		//cin >> x;
	}
}
