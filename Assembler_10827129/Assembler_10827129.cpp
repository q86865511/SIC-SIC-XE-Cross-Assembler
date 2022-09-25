// 10827129 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <string.h>
#include <io.h>
#include <algorithm>
#include <sstream>
#include <math.h>
#include <iomanip>
#include <stdio.h>


using namespace std;

typedef struct token {
	string token_Data;
	int num_Of_Table;
	int num_Of_Column;
} tK;

struct Literal {
	int num_Of_Table;
	int num_Of_Column;
	string name;
	int address;
	int serialNum;
};

struct LiteralBuffer {
	int line ;
	int loc ;
	string statement ;
	string ASCIICode ;
	int serialNum ;
	string name ;
};

struct TokenAndAddress { 
	int tableNum;
	int entryNum;
	string name;
	int address; 
};

struct XEType {
	int format ;
	int opcode ;
	int r1 ;
	int r2 ;
	int n ;
	int i ;
	int x ;
	int b ;
	int p ;
	int e ;
	int disp ;
	int address ;
};

struct InstructionSet {
	int opcode ;
	int operand ;
	int format ; 
	int isXE ;
};

struct AssemblerList {
	int format ;
	int line ;
	int loc ;
	string objectCode ;
};

struct StandardType {
	int opcode ;
	int x ;
	int address ;
};

class Lexer {
	private :
		vector<tK> token_List;
		vector<string> delimiter;
		vector<tK> hash5;
		vector<tK> hash6;
		vector<tK> hash7;
		vector<int> line_Counter;
		string name_Of_File;
		
		bool isWhiteSpace( char temp_Ch ) {
			if ( temp_Ch == '\t' || temp_Ch == ' ' || temp_Ch == '\n' )
				return true;
			return false;
		}
		
		bool isDelimiter( char temp_Ch ) {
			for( int i = 0 ; i < delimiter.size() ; i ++ ) {
				if ( temp_Ch == delimiter[i].c_str()[0] )
					return true;
			}
			
			return false;
		}
		
		bool locateToken( string token_Data, int &num_Of_Table, int &num_Of_Column, vector< vector<string> > table ) {
			for( int index_Of_Table = 0 ; index_Of_Table < 4 ; index_Of_Table ++ ) {
				vector<string> table_Data = table[index_Of_Table];
				for( int index_Of_Column = 0 ; index_Of_Column < table_Data.size() ; index_Of_Column ++ ) {
					if ( !strcmp( token_Data.c_str(), table_Data[index_Of_Column].c_str() ) ) {
						num_Of_Table = index_Of_Table + 1;
						num_Of_Column = index_Of_Column + 1;
						return true;
					}
				}
			}
			
			return false;	
		}
		
		bool isDecNumber( string token_Data ) {
			for ( int i = 0 ; i < token_Data.length() ; i ++ ) {
				if ( token_Data.at( i ) < '0' || token_Data.at( i ) > '9' )
					return false;
			}
			
			return true;
		}
		
		int locateTable( string token_Data, string conditionP ) {
			if ( conditionP == "WHITESPACE" ) {
				if ( isDecNumber( token_Data ) ) 
					return 6;
				else
					return 5;
			}
			else if ( conditionP == "DELIMITER" ) {
				if ( isDecNumber( token_Data ) ) 
					return 6;
				else
					return 5;
			}
			else if ( conditionP == "DELIMITER_STRING" ) 
				return 7;
			else if ( conditionP == "HEXNUMBER")
				return 6;
			return -1;
		}
		
		int hashFunction( token tk ) {
			int ascii = 0;
			for ( int i = 0 ; i < tk.token_Data.length() ; i ++ )
				ascii += tk.token_Data[i];
			
			int hash_Key = ascii % 100;

			if ( tk.num_Of_Table == 5 ) {
				for ( int key = hash_Key ; key < hash5.size() ; key ++ ) {
					if ( hash5[key].token_Data.empty() ) {
						tk.num_Of_Column = key;
						hash5[key] = tk;
						hash_Key = key;
						break;
					}
					else {
						if ( hash5[key].token_Data == tk.token_Data )
							return key;
						else if ( key == 99 )
							key = -1;
					}
				}
			}
			else if ( tk.num_Of_Table == 6 ) {
				for ( int key = hash_Key ; key < hash6.size() ; key ++ ) {
					if ( hash6[key].token_Data.empty() ) {
						tk.num_Of_Column = key;
						hash6[key] = tk;
						hash_Key = key;
						break;
					}
					else {
						if ( hash6[key].token_Data == tk.token_Data )
							return key;
						else if ( key == 99 )
							key = -1;
					}
				}
			}
			else if ( tk.num_Of_Table == 7 ) {
				for ( int key = hash_Key ; key < hash7.size() ; key ++ ) {
					if ( hash7[key].token_Data.empty() ) {
						tk.num_Of_Column = key;
						hash7[key] = tk;
						hash_Key = key;
						break;
					}
					else {
						if ( hash7[key].token_Data == tk.token_Data )
							return key;
						else if ( key == 99 )
							key = -1;
					}
				}
			}
			return hash_Key;
		}
		
	public :
		Lexer( vector<string> table4_Data ) {
			copy( table4_Data.begin(), table4_Data.end(), back_inserter( delimiter ) );
			
			hash5.resize( 100 );
			hash6.resize( 100 );
			hash7.resize( 100 );
		}
		
		void analysis( string file_Name, vector< vector<string> > table ) {
			name_Of_File = file_Name;
			fstream file;
			file.open( file_Name.c_str() );
			int num_Of_Word = 0;
			while ( file.peek() != EOF ) {
				string reg_Of_String;
				char temp_Ch;
				file.get( temp_Ch );
				while ( !isDelimiter( temp_Ch ) && !isWhiteSpace( temp_Ch ) && !file.eof() ) {
					reg_Of_String += temp_Ch;
					file.get( temp_Ch );
				}

				if ( isWhiteSpace( temp_Ch ) || file.eof() ) {
					if ( reg_Of_String != "" ) {
						token tk_St;
						tk_St.token_Data = reg_Of_String;
						if ( locateToken( reg_Of_String, tk_St.num_Of_Table, tk_St.num_Of_Column, table ) )
							token_List.push_back( tk_St );
						else {
							tk_St.num_Of_Table = locateTable( reg_Of_String, "WHITESPACE" );
							tk_St.num_Of_Column = hashFunction( tk_St );
							token_List.push_back( tk_St );
						}
						
						num_Of_Word ++;
					} 
					
					if ( temp_Ch == '\n' || file.eof() ) {
						line_Counter.push_back( num_Of_Word );
						num_Of_Word = 0;
					}
					
					if ( file.eof() )
						cout << "All analyses are already done !!" << endl;
				}
				else if ( isDelimiter( temp_Ch ) ) {
					if ( ( reg_Of_String == "C" || reg_Of_String == "c" ) && temp_Ch == '\'' ){
						token tk_St;
						tk_St.token_Data += temp_Ch;
						locateToken( tk_St.token_Data, tk_St.num_Of_Table, tk_St.num_Of_Column, table ) ;
						token_List.push_back( tk_St );
						num_Of_Word ++;
						
						reg_Of_String.clear();
						file.get( temp_Ch );
						while ( !isDelimiter( temp_Ch ) && !file.eof() ) {
							reg_Of_String += temp_Ch;
							file.get( temp_Ch );
						}
						
						token tk2_St;
						tk2_St.token_Data = reg_Of_String;
						tk2_St.num_Of_Table = locateTable( reg_Of_String, "DELIMITER_STRING" );
						tk2_St.num_Of_Column = hashFunction( tk2_St );
						token_List.push_back( tk2_St );
						num_Of_Word ++;
					}
					else if ( ( reg_Of_String == "X" || reg_Of_String == "x" ) && temp_Ch == '\'' ){
						token tk_St;
						tk_St.token_Data += temp_Ch;
						locateToken( tk_St.token_Data, tk_St.num_Of_Table, tk_St.num_Of_Column, table ) ;
						token_List.push_back( tk_St );
						num_Of_Word ++;
						
						reg_Of_String.clear();
						file.get( temp_Ch );
						while ( temp_Ch != '\'' && !file.eof() ) {
							reg_Of_String += temp_Ch;
							file.get( temp_Ch );
						}
						
						token tk2_St;
						tk2_St.token_Data = reg_Of_String;
						tk2_St.num_Of_Table = locateTable( reg_Of_String, "HEXNUMBER" );
						tk2_St.num_Of_Column = hashFunction( tk2_St );
						token_List.push_back( tk2_St );
						num_Of_Word ++;
					}
					else if ( reg_Of_String != "" ) {
						token tk_St;
						tk_St.token_Data = reg_Of_String;
						if ( locateToken( reg_Of_String, tk_St.num_Of_Table, tk_St.num_Of_Column, table ) )
							token_List.push_back( tk_St );
						else {
							tk_St.num_Of_Table = locateTable( reg_Of_String, "DELIMITER" );
							tk_St.num_Of_Column = hashFunction( tk_St );
							token_List.push_back( tk_St );
						}
						num_Of_Word ++;
					}
					
					token tk_St;
					tk_St.token_Data += temp_Ch;
					locateToken( tk_St.token_Data, tk_St.num_Of_Table, tk_St.num_Of_Column, table ) ;
					token_List.push_back( tk_St );
					num_Of_Word ++;
					
					if ( temp_Ch == '.' ) {
						getline( file, reg_Of_String ); // ���ѥᱼ 
						line_Counter.push_back( num_Of_Word );
						num_Of_Word = 0;
					}
				}
			}
			
			file.close();
		}
		
		void writeFile() {
			ofstream output_File;
			output_File.open( "output.txt" );
			fstream input_File;
			input_File.open( name_Of_File.c_str() );
			char temp_Ch;
			int token_Index = 0;
			int column = 0;
			while( input_File.peek() != EOF ) {
				while ( true ) {
					input_File.get( temp_Ch );
					cout << temp_Ch;
					output_File << temp_Ch;
					if ( temp_Ch == '\n' )
						break;
					else if ( input_File.peek() == EOF ) {
						if ( temp_Ch != '\n' ) {
							cout << '\n';
							output_File << '\n';
						}
						break;
					}		
				}
				
				for( int i = token_Index ; i < token_Index + line_Counter[column] && i < token_List.size() ; i ++ ) {
					cout << "(" << token_List[i].num_Of_Table << "," << token_List[i].num_Of_Column << ")";
					output_File << "(" << token_List[i].num_Of_Table << "," << token_List[i].num_Of_Column << ")";
					if ( i + 1 == token_Index + line_Counter[column] ) {
						token_Index = i + 1;
						break;
					}			
				}
				
				cout << endl;
				output_File << endl;
				column ++;
			}
			
			input_File.close();
		}
		
		vector< vector<tK> > changeTkFormat() {
			vector< vector<tK> > newTokenList;
			int token_Index = 0;
			for ( int i = 0 ; i < line_Counter.size() ; i ++ ) {
				vector<tK> tempList;
				for ( int j = 0 ; j < line_Counter[i] && token_Index < token_List.size() ; j ++ ) {
					tempList.push_back( token_List[token_Index] );
					token_Index++;
				}
				
				newTokenList.push_back( tempList );
			}

			return newTokenList;
		}
		
		vector< vector<tK> > get567Table() {
			vector< vector<tK> > temp;
			temp.push_back( hash5 );
			temp.push_back( hash6 );
			temp.push_back( hash7 );
			return temp;
		}
		
		string getNameOFFile(){
			return name_Of_File;
		}
		
		vector<string> getTokenResult() {
			vector<string> result;
			fstream input_File;
			input_File.open( name_Of_File.c_str() );
			while( input_File.peek() != EOF ) {
				char temp[50];
				input_File.getline( temp, sizeof( temp ) );
				string tempS( temp );
				result.push_back( tempS );
			} 
			
			input_File.close();
			return result;
		}
};

class Table {
	private :
		vector< vector<string> > table;
		
	public :	
		void readTable() {
			for ( char table_Num = '1' ; table_Num < '5' ; table_Num ++ ) {
				string table_Name = "Table";
				table_Name += table_Num;
				table_Name += ".table";
				
				fstream table_File;
				table_File.open( table_Name.c_str() );
				if ( !table_File )
				cout << "Table" << table_Num << " File does not exist !!" << endl;	
				else {
					vector<string> temp_Table;
					while ( table_File.peek() != EOF ) {
						string temp_Line;
						getline( table_File, temp_Line );
						if ( temp_Line[temp_Line.length() - 1] == ' ' )
							temp_Line.erase( temp_Line.length() - 1 );
						temp_Table.push_back( temp_Line );
					}
					
					table_File.close();
					table.push_back( temp_Table );
				}
			}
		}
		
		vector<string> getTable4() {
			return table.at(3);
		}
		
		vector< vector<string> > getAllTable() {
			return table;
		}
		
		vector< vector<tK> > getTkTable() {
			vector< vector<tK> > tkTable;
			for ( int i = 0 ; i < table.size() ; i ++ ) {
				vector<tK> temp_Table;
				for ( int j = 0 ; j < table[i].size() ; j ++ ) {
					tK token;
					token.token_Data = table[i][j];
					token.num_Of_Table = i + 1;
					token.num_Of_Column = j + 1;
					temp_Table.push_back( token );
				}
				tkTable.push_back( temp_Table );
			}
			
			return tkTable;
		}
}; 

class Assembler {
	public :
		vector< vector<tK> > token_List;
		vector< vector<tK> > table_List;
		vector<Literal> literal_Table; 
		vector<LiteralBuffer> literal_Buffer_Table ;
		vector<int> digitsVector ;
		vector<InstructionSet> instructionSet ; 
		vector<AssemblerList> assemblerList ; // ans
		
		XEType xeType ;
		string ASCIICode;
		StandardType standardType ;
		TokenAndAddress symbol_Table[100];
		TokenAndAddress integer_Table[100];
		TokenAndAddress string_Table[100];
		
		int line_Counter_Temp;
		int pc; // next location
		int start_Location;
		int locTemp;
		int locTempForEQU;
		int digits;
		int literalNum;
		int instructionSetEntry;
		int operandNumber;
		int balance;
		
		bool isSTART;
		bool isExcutable;
		bool isBYTE;
		bool isWORD;
		bool isRESB;
		bool isRESW;
		bool isComment;
		bool isPseudo;
		bool isEND;
		bool isEQU;
		bool isBASE;
		bool isLTORG;
		bool isIndexRegister;
		
		bool isXE;
		bool isF4;
		bool r1IsDone;
		
		int indexRegister;
		int BASERegister;
	
	public :
		Assembler( vector< vector<tK> > temp_List, vector< vector<tK> > table1234, vector< vector<tK> > hash567 ) {
			copy( temp_List.begin(), temp_List.end(), back_inserter( token_List ) );
		/*	for ( int i = 0 ; i < token_List.size() ; i ++ ) {
				for ( int j = 0 ; j  < token_List[i].size() ; j ++ )
					cout << token_List[i][j].token_Data << endl;
			}*/
			table_List.insert( table_List.begin(), table1234.begin(), table1234.end() );

			line_Counter_Temp = 5 ;
			pc = 0x0000 ; // next location
			start_Location = 0x0000 ;
			locTemp = 0x0000 ;
			locTempForEQU = 0x0000 ;
			digits = 0 ;
			literalNum = 0 ;
			ASCIICode = "" ;
			instructionSetEntry = 0 ;
			operandNumber = 0 ;
			balance = -1 ;
			
			isSTART = false ;
			isExcutable = false ;
			isBYTE = false ;
			isWORD = false ;
			isRESB = false ;
			isRESW = false ;
			isComment = false ;
			isPseudo = false ;
			isEND = false ;
			isEQU = false ;
			isBASE = false ;
			isLTORG = false ;
			isIndexRegister = false ;
			
			isXE = false ;
			isF4 = false ;
			r1IsDone = false ;
			
			isIndexRegister = false ;
			indexRegister = 0 ;
			BASERegister = 0 ;
			
			for ( int i = 0 ; i < 100 ; i++ ) {
				symbol_Table[i].tableNum = hash567[0][i].num_Of_Table ;
				symbol_Table[i].entryNum = hash567[0][i].num_Of_Column ;
				symbol_Table[i].name = hash567[0][i].token_Data ;
				symbol_Table[i].address = -1 ;
			} // for
			
			for ( int i = 0 ; i < 100 ; i++ ) {
				integer_Table[i].tableNum = hash567[1][i].num_Of_Table ;
				integer_Table[i].entryNum = hash567[1][i].num_Of_Column ;
				integer_Table[i].name = hash567[1][i].token_Data ;
				integer_Table[i].address = -1 ;
			} // for
			
			for ( int i = 0 ; i < 100 ; i++ ) {
				string_Table[i].tableNum = hash567[2][i].num_Of_Table ;
				string_Table[i].entryNum = hash567[2][i].num_Of_Column ;
				string_Table[i].name = hash567[2][i].token_Data ;
				string_Table[i].address = -1 ;
			} // for
				
			for ( int i = 0 ; i < 100 ; i ++ ) {
				Literal temp;
				temp.num_Of_Table = -1;
				temp.num_Of_Column = -1;
				temp.name = "-1";
				temp.address = -1;
				temp.serialNum = -1;
				literal_Table.push_back( temp );
			} // setup literal table
		}
// -----------------------pass1------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		
		void setInstructureSet() { // �]�w���O�� 
			InstructionSet set ;
			set.opcode = -1 ;
			set.operand = -1 ;
			set.format = -1 ;
			
			for ( int i = 0 ; i < 59 ; i++ )
				instructionSet.push_back( set ) ;
			
			instructionSet[0].operand = 1 ;
			instructionSet[0].format = 3 ;
			instructionSet[0].opcode = 0x18 ;
			instructionSet[0].isXE = 0 ;
			instructionSet[1].operand = 1 ;
			instructionSet[1].format = 3 ;
			instructionSet[1].opcode = 0x58 ;
			instructionSet[1].isXE = 1 ;		
			instructionSet[2].operand = 2 ;
			instructionSet[2].format = 2 ;
			instructionSet[2].opcode = 0x90 ;
			instructionSet[2].isXE = 1 ;	
			instructionSet[3].operand = 1 ;
			instructionSet[3].format = 3 ;
			instructionSet[3].opcode = 0x40 ;
			instructionSet[3].isXE = 0 ;
			instructionSet[4].operand = 1 ;
			instructionSet[4].format = 2 ;
			instructionSet[4].opcode = 0xB4 ;
			instructionSet[4].isXE = 1 ;
			instructionSet[5].operand = 1 ;
			instructionSet[5].format = 3 ;
			instructionSet[5].opcode = 0x28 ;
			instructionSet[5].isXE = 0 ;
			instructionSet[6].operand = 1 ;
			instructionSet[6].format = 3 ;
			instructionSet[6].opcode = 0x88 ;
			instructionSet[6].isXE = 1 ;
			instructionSet[7].operand = 2 ;
			instructionSet[7].format = 2 ;
			instructionSet[7].opcode = 0xA0 ;
			instructionSet[7].isXE = 1 ;
			instructionSet[8].operand = 1 ;
			instructionSet[8].format = 3 ;
			instructionSet[8].opcode = 0x24 ;
			instructionSet[8].isXE = 0 ;
			instructionSet[9].operand = 1 ;
			instructionSet[9].format = 3 ;
			instructionSet[9].opcode = 0x64 ;
			instructionSet[9].isXE = 1 ;
			instructionSet[10].operand = 2 ;
			instructionSet[10].format = 2 ;
			instructionSet[10].opcode = 0x9C ;
			instructionSet[10].isXE = 1 ;
			instructionSet[11].operand = 0 ;
			instructionSet[11].format = 1 ;
			instructionSet[11].opcode = 0xC4 ;
			instructionSet[11].isXE = 1 ;
			instructionSet[12].operand = 0 ;
			instructionSet[12].format = 1 ;
			instructionSet[12].opcode = 0xC0 ;
			instructionSet[12].isXE = 1 ;
			instructionSet[13].operand = 0 ;
			instructionSet[13].format = 1 ;
			instructionSet[13].opcode = 0xF4 ;
			instructionSet[13].isXE = 1 ;
			
			instructionSet[14].operand = 1 ;
			instructionSet[14].format = 3 ;
			instructionSet[14].opcode = 0x3C ;
			instructionSet[14].isXE = 0 ;
			instructionSet[15].operand = 1 ;
			instructionSet[15].format = 3 ;
			instructionSet[15].opcode = 0x30 ;
			instructionSet[15].isXE = 0 ;
			instructionSet[16].operand = 1 ;
			instructionSet[16].format = 3 ;
			instructionSet[16].opcode = 0x34 ;
			instructionSet[16].isXE = 0 ;
			instructionSet[17].operand = 1 ;
			instructionSet[17].format = 3 ;
			instructionSet[17].opcode = 0x38 ;
			instructionSet[17].isXE = 0 ;
			instructionSet[18].operand = 1 ;
			instructionSet[18].format = 3 ;
			instructionSet[18].opcode = 0x48 ;
			instructionSet[18].isXE = 0 ;
			instructionSet[19].operand = 1 ;
			instructionSet[19].format = 3 ;
			instructionSet[19].opcode = 0x00 ;
			instructionSet[19].isXE = 0 ;
			instructionSet[20].operand = 1 ;
			instructionSet[20].format = 3 ;
			instructionSet[20].opcode = 0x68 ;
			instructionSet[20].isXE = 1 ;
			instructionSet[21].operand = 1 ;
			instructionSet[21].format = 3 ;
			instructionSet[21].opcode = 0x50 ;
			instructionSet[21].isXE = 0 ;
			instructionSet[22].operand = 1 ;
			instructionSet[22].format = 3 ;
			instructionSet[22].opcode = 0x70 ;
			instructionSet[22].isXE = 1 ;
			instructionSet[23].operand = 1 ;
			instructionSet[23].format = 3 ;
			instructionSet[23].opcode = 0x08 ;
			instructionSet[23].isXE = 0 ;
			instructionSet[24].operand = 1 ;
			instructionSet[24].format = 3 ;
			instructionSet[24].opcode = 0x6C ;
			instructionSet[24].isXE = 1 ;
			instructionSet[25].operand = 1 ;
			instructionSet[25].format = 3 ;
			instructionSet[25].opcode = 0x74 ;
			instructionSet[25].isXE = 1 ;
			instructionSet[26].operand = 1 ;
			instructionSet[26].format = 3 ;
			instructionSet[26].opcode = 0x04 ;
			instructionSet[26].isXE = 0 ;
			instructionSet[27].operand = 1 ;
			instructionSet[27].format = 3 ;
			instructionSet[27].opcode = 0xD0 ;
			instructionSet[27].isXE = 1 ;
			instructionSet[28].operand = 1 ;
			instructionSet[28].format = 3 ;
			instructionSet[28].opcode = 0x20 ;
			instructionSet[28].isXE = 0 ;
			
			instructionSet[29].operand = 1 ;
			instructionSet[29].format = 3 ;
			instructionSet[29].opcode = 0x60 ;
			instructionSet[29].isXE = 1 ;
			instructionSet[30].operand = 2 ;
			instructionSet[30].format = 2 ;
			instructionSet[30].opcode = 0x98 ;
			instructionSet[30].isXE = 1 ;
			instructionSet[31].operand = 0 ;
			instructionSet[31].format = 1 ;
			instructionSet[31].opcode = 0xC8 ;
			instructionSet[31].isXE = 1 ;
			instructionSet[32].operand = 1 ;
			instructionSet[32].format = 3 ;
			instructionSet[32].opcode = 0x44 ;
			instructionSet[32].isXE = 0 ;
			instructionSet[33].operand = 1 ;
			instructionSet[33].format = 3 ;
			instructionSet[33].opcode = 0xD8 ;
			instructionSet[33].isXE = 0 ;
			instructionSet[34].operand = 2 ;
			instructionSet[34].format = 2 ;
			instructionSet[34].opcode = 0xAC ;
			instructionSet[34].isXE = 1 ;
			instructionSet[35].operand = 0 ;
			instructionSet[35].format = 3 ;
			instructionSet[35].opcode = 0x4C ;
			instructionSet[35].isXE = 0 ;
			instructionSet[36].operand = 2 ;
			instructionSet[36].format = 2 ;
			instructionSet[36].opcode = 0xA4 ;
			instructionSet[36].isXE = 1 ;
			instructionSet[37].operand = 2 ;
			instructionSet[37].format = 2 ;
			instructionSet[37].opcode = 0xA8 ;
			instructionSet[37].isXE = 1 ;
			
			instructionSet[38].operand = 0 ;
			instructionSet[38].format = 1 ;
			instructionSet[38].opcode = 0xF0 ;
			instructionSet[38].isXE = 1 ;
			instructionSet[39].operand = 1 ;
			instructionSet[39].format = 3 ;
			instructionSet[39].opcode = 0xEC ;
			instructionSet[39].isXE = 1 ;
			instructionSet[40].operand = 1 ;
			instructionSet[40].format = 3 ;
			instructionSet[40].opcode = 0x0C ;
			instructionSet[40].isXE = 0 ;
			instructionSet[41].operand = 1 ;
			instructionSet[41].format = 3 ;
			instructionSet[41].opcode = 0x78 ;
			instructionSet[41].isXE = 1 ;
			instructionSet[42].operand = 1 ;
			instructionSet[42].format = 3 ;
			instructionSet[42].opcode = 0x54 ;
			instructionSet[42].isXE = 0 ;
			instructionSet[43].operand = 1 ;
			instructionSet[43].format = 3 ;
			instructionSet[43].opcode = 0x80 ;
			instructionSet[43].isXE = 1 ;
			instructionSet[44].operand = 1 ;
			instructionSet[44].format = 3 ;
			instructionSet[44].opcode = 0xD4 ;
			instructionSet[44].isXE = 1 ;
			instructionSet[45].operand = 1 ;
			instructionSet[45].format = 3 ;
			instructionSet[45].opcode = 0x14 ;
			instructionSet[45].isXE = 0 ;
			instructionSet[46].operand = 1 ;
			instructionSet[46].format = 3 ;
			instructionSet[46].opcode = 0x7C ;
			instructionSet[46].isXE = 1 ;
			instructionSet[47].operand = 1 ;
			instructionSet[47].format = 3 ;
			instructionSet[47].opcode = 0xE8 ;
			instructionSet[47].isXE = 0 ;
			instructionSet[48].operand = 1 ;
			instructionSet[48].format = 3 ;
			instructionSet[48].opcode = 0x84 ;
			instructionSet[48].isXE = 1 ;
			instructionSet[49].operand = 1 ;
			instructionSet[49].format = 3 ;
			instructionSet[49].opcode = 0x10 ;
			instructionSet[49].isXE = 0 ;
			instructionSet[50].operand = 1 ;
			instructionSet[50].format = 3 ;
			instructionSet[50].opcode = 0x1C ;
			instructionSet[50].isXE = 0 ;
			instructionSet[51].operand = 1 ;
			instructionSet[51].format = 3 ;
			instructionSet[51].opcode = 0x5C ;
			instructionSet[51].isXE = 1 ;
			
			instructionSet[52].operand = 2 ;
			instructionSet[52].format = 2 ;
			instructionSet[52].opcode = 0x94 ;
			instructionSet[52].isXE = 1 ;
			instructionSet[53].operand = 1 ;
			instructionSet[53].format = 2 ;
			instructionSet[53].opcode = 0xB0 ;
			instructionSet[53].isXE = 1 ;
			instructionSet[54].operand = 1 ;
			instructionSet[54].format = 3 ;
			instructionSet[54].opcode = 0xE0 ;
			instructionSet[54].isXE = 0 ;
			instructionSet[55].operand = 0 ;
			instructionSet[55].format = 1 ;
			instructionSet[55].opcode = 0xF8 ;
			instructionSet[55].isXE = 1 ;
			instructionSet[56].operand = 1 ;
			instructionSet[56].format = 3 ;
			instructionSet[56].opcode = 0x2C ;
			instructionSet[56].isXE = 0 ;
			instructionSet[57].operand = 1 ;
			instructionSet[57].format = 2 ;
			instructionSet[57].opcode = 0xB8 ;
			instructionSet[57].isXE = 1 ;
			instructionSet[58].operand = 1 ;
			instructionSet[58].format = 3 ;
			instructionSet[58].opcode = 0xDC ;
			instructionSet[58].isXE = 0 ;
		} // setInstructureSet()
		
		void pass1() {
			for ( int i = 0 ; i < token_List.size() ; i++ ) {
				if ( !isSTART )
					locTemp = pc ;
				else
					isSTART = false ;
					
				for ( int j = 0 ; j < token_List[i].size() ; j++ ) {
					if ( isXE )
				    	analysis_1_XE( i, j ) ;
					else
						analysis_1( i, j ) ;
				}
				
				if ( isSTART ) {
					locTemp = start_Location;
					pc = start_Location;
				}
				
				AssemblerList row;
				row.line = -1;
				row.loc = -1;
				row.objectCode = "-1";
				row.format = -1;
				
				if ( !isXE ) {
					setARow_1( row, i );
					assemblerList.push_back( row );
				}
				else {
					if ( xeType.disp == -1 )
						xeType.disp = 0 ;
					setARow_1_XE( row, i ) ;
					assemblerList.push_back( row ) ;
				}
				
				if ( !token_List[i].empty() )
					line_Counter_Temp = line_Counter_Temp + 5;
				
				// initialize	
				isExcutable = false ;
				isBYTE = false ;
				isWORD = false ;
				isRESB = false ;
				isRESW = false ;
				isPseudo = false ;
				isComment = false ;
				isEND = false ;
				isEQU = false ;
				isBASE = false ;
				isLTORG = false ;
				// XE
				xeType.format = 0 ;
				xeType.opcode = 0 ;
				xeType.r1 = 0 ;
				xeType.r2 = 0 ;
				xeType.n = 0 ;
				xeType.i = 0 ;
				xeType.x = 0 ;
				xeType.b = 0 ;
				xeType.p = 0 ;
				xeType.e = 0 ;
				xeType.disp = -1 ;
				xeType.address = -1 ;
				isF4 = false ;
			}
		}
		
		void setARow_1( AssemblerList & row , int i ) {
			row.line = line_Counter_Temp ; // �����@��Row��line 
			if ( isComment && !isExcutable )
				row.loc = -1 ;	
			else if ( token_List[i].empty() ) {
				row.loc = -1 ;
				row.line = -1 ;
			} // else if
			else if ( isEQU )
				row.loc = locTempForEQU ;
			else if ( isEND || isLTORG )
				row.loc = -1 ;
			else
				row.loc = locTemp ;
		} // setARow_pass1()
		
		void setARow_1_XE( AssemblerList & row , int i ) {
			row.line = line_Counter_Temp ;
			row.format = xeType.format ;
			if ( isBASE )
				row.loc = -1 ;
			else if ( isComment && !isExcutable )
				row.loc = -1 ;
			else if ( token_List[i].empty() ) {
				row.loc = -1 ;
				row.line = -1 ;
			} // else if
			else if ( isEQU )
				row.loc = locTempForEQU ;
			else if ( isEND || isLTORG )
				row.loc = -1 ;
			else
				row.loc = locTemp ;
		} // setARow_pass1_XE()
		
		void analysis_1_XE( int line_Number, int column_Number ) {
			string token = token_List[line_Number][column_Number].token_Data;
			token = fixStringToBeLikeTables( token, token_List[line_Number][column_Number].num_Of_Table ); // ���j�p�g�Mtable�@�� 
			if ( token_List[line_Number][column_Number].num_Of_Table == 1 ) {
				isExcutable = true;
				int opcodeTemp = 0;
				xeType.opcode = instructionSet[token_List[line_Number][column_Number].num_Of_Column-1].opcode ;
				xeType.format = instructionSet[token_List[line_Number][column_Number].num_Of_Column-1].format ;
				if ( isFormat4( token, line_Number, column_Number ) )
					xeType.format = 4 ;		

				if ( xeType.format == 1 )
					pc = pc + 1 ;
				else if ( xeType.format == 2 )
					pc = pc + 2 ;
				else if ( xeType.format == 3 )
					pc = pc + 3 ;
				else if ( xeType.format == 4 )
					pc = pc + 4 ;
			} // ������O
			else if ( token_List[line_Number][column_Number].num_Of_Table == 2 ) {
				if ( token == "START" ) {
					isSTART = true;
					start_Location = stringToInterger_hex( token_List[line_Number][column_Number+1].token_Data ); // �ǰ_�l��m 
				}
				else if ( token == "BYTE" ) {
					isBYTE = true;
					pc = pc + getSizeOfByte( token_List[line_Number][column_Number+2].token_Data, token_List[line_Number][column_Number+2].num_Of_Table );
				}
				else if ( token == "WORD" ) {
					isWORD = true;
					pc = pc + 3;
				}
				else if ( token == "RESB" ) {
					isRESB = true;
					pc = pc + stringToInterger( token_List[line_Number][column_Number+1].token_Data );
				}
				else if ( token == "RESW" ) {
					isRESW = true;
					pc = pc + stringToInterger( token_List[line_Number][column_Number+1].token_Data ) * 3;
				}
				else if ( token == "END" ) {
					isEND = true;
					setLiteralTableAddress();
				}
				else if ( token == "EQU" ) {
					locTempForEQU = stringToInterger( token_List[line_Number][column_Number+1].token_Data ) ;
					isEQU = true ;
				}
				else if ( token == "BASE" )
					isBASE = true ;
				else if ( token == "LTORG" ) {
					isLTORG = true ;
					setLiteralTableAddress() ;
				}
			} 
			else if ( token_List[line_Number][column_Number].num_Of_Table == 3 )
				;
			else if ( token_List[line_Number][column_Number].num_Of_Table == 4 ) {
				if ( token == "." )
					isComment = true;
			}
			else if ( token_List[line_Number][column_Number].num_Of_Table == 5 ) {
				if ( column_Number == 0 ) {
					if ( symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address != -1 ) 
						cout << "ERROR: �h���w�q !!" << endl;
					else {
						if ( ( column_Number < token_List[line_Number].size() ) && ( token_List[line_Number][column_Number+1].token_Data == "EQU" ) ) {
							if ( token_List[line_Number][column_Number+2].token_Data != "*" && token_List[line_Number][column_Number+2].num_Of_Table != 5 ) 
								symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address = stringToInterger( token_List[line_Number][column_Number+2].token_Data );
							else if ( token_List[line_Number][column_Number+2].token_Data == "*" )
								symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address = locTemp;
							else {
								if ( column_Number + 3 >= token_List[line_Number].size() ) {
									if ( token_List[line_Number][column_Number+2].num_Of_Table != 5 )
										symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address = stringToInterger( token_List[line_Number][column_Number+2].token_Data );
									else
										symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address = symbol_Table[token_List[line_Number][column_Number+2].num_Of_Column].address;
								}
								else {
									if ( column_Number + 4 >= token_List[line_Number].size() )
										cout << "Syntax Error !!!" << endl;
									else if ( token_List[line_Number][column_Number+3].token_Data == "+" ) 
										symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address = symbol_Table[token_List[line_Number][column_Number+2].num_Of_Column].address + symbol_Table[token_List[line_Number][column_Number+4].num_Of_Column].address;
									else if ( token_List[line_Number][column_Number+3].token_Data == "-" ) 
										symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address = symbol_Table[token_List[line_Number][column_Number+2].num_Of_Column].address - symbol_Table[token_List[line_Number][column_Number+4].num_Of_Column].address;
								}
								
							}
						}
						else
							symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address = locTemp;
					}
				}
			}
			else if ( token_List[line_Number][column_Number].num_Of_Table == 6 ) {
				if ( ( ( column_Number > 1 ) && ( token_List[line_Number][column_Number-2].token_Data == "=" ) ) || ( ( column_Number > 0 ) && ( token_List[line_Number][column_Number-1].token_Data == "=" ) ) )
					hashInLiteralTable( token_List[line_Number][column_Number].token_Data , token_List[line_Number][column_Number].num_Of_Table ) ;
			}
			else if ( token_List[line_Number][column_Number].num_Of_Table == 7 ) {
				if ( ( column_Number > 1 ) && ( token_List[line_Number][column_Number-2].token_Data == "=" ) )
					hashInLiteralTable( token_List[line_Number][column_Number].token_Data , token_List[line_Number][column_Number].num_Of_Table ) ;
			}
		}
		
		bool isFormat4( string s, int i, int j ) {
			bool decide = false ;
			if ( j > 0 ) {
				if ( token_List[i][j-1].token_Data == "+" ) {
					decide = true ;
					isF4 = true ;
				}  // if
			} // if
			
			return decide ;
		} // isFormat4()
		
		void analysis_1( int line_Number, int column_Number ) {
			string token = token_List[line_Number][column_Number].token_Data;
			token = fixStringToBeLikeTables( token, token_List[line_Number][column_Number].num_Of_Table ); // ���j�p�g�Mtable�@�� 
			
			if ( token_List[line_Number][column_Number].num_Of_Table == 1 ) {
				isExcutable = true;
				pc = pc + 3;
			} // ������O
			else if ( token_List[line_Number][column_Number].num_Of_Table == 2 ) {
				if ( token == "START" ) {
					isSTART = true;
					start_Location = stringToInterger_hex( token_List[line_Number][column_Number+1].token_Data ); // �ǰ_�l��m 
				}
				else if ( token == "BYTE" ) {
					isBYTE = true;
					pc = pc + getSizeOfByte( token_List[line_Number][column_Number+2].token_Data, token_List[line_Number][column_Number+2].num_Of_Table );
				}
				else if ( token == "WORD" ) {
					isWORD = true;
					pc = pc + 3;
				}
				else if ( token == "RESB" ) {
					isRESB = true;
					pc = pc + stringToInterger( token_List[line_Number][column_Number+1].token_Data );
				}
				else if ( token == "RESW" ) {
					isRESW = true;
					pc = pc + stringToInterger( token_List[line_Number][column_Number+1].token_Data ) * 3;
				}
				else if ( token == "END" ) {
					isEND = true;
					setLiteralTableAddress();
				}
				else if ( token == "EQU" ) {
					locTempForEQU = stringToInterger( token_List[line_Number][column_Number+1].token_Data ) ;
					isEQU = true ;
				}
			} 
			else if ( token_List[line_Number][column_Number].num_Of_Table == 3 )
				;
			else if ( token_List[line_Number][column_Number].num_Of_Table == 4 ) {
				if ( token == "." )
					isComment = true;
			}
			else if ( token_List[line_Number][column_Number].num_Of_Table == 5 ) {
				if ( column_Number == 0 ) {
					if ( symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address != -1 ) 
						cout << "ERROR: �h���w�q !!" << endl;
					else {
						if ( ( column_Number < token_List[line_Number].size() ) && ( token_List[line_Number][column_Number+1].token_Data == "EQU" ) ) {
							if ( token_List[line_Number][column_Number+2].token_Data != "*" )
								symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address = stringToInterger( token_List[line_Number][column_Number+2].token_Data );
							else
								symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address = locTemp;
						}
						else
							symbol_Table[token_List[line_Number][column_Number].num_Of_Column].address = locTemp;
					}
				}
			}
			else if ( token_List[line_Number][column_Number].num_Of_Table == 6 ) {
				if ( ( ( column_Number > 1 ) && ( token_List[line_Number][column_Number-2].token_Data == "=" ) ) || ( ( column_Number > 0 ) && ( token_List[line_Number][column_Number-1].token_Data == "=" ) ) )
					hashInLiteralTable( token_List[line_Number][column_Number].token_Data , token_List[line_Number][column_Number].num_Of_Table ) ;
			}
			else if ( token_List[line_Number][column_Number].num_Of_Table == 7 ) {
				if ( ( column_Number > 1 ) && ( token_List[line_Number][column_Number-2].token_Data == "=" ) )
					hashInLiteralTable( token_List[line_Number][column_Number].token_Data , token_List[line_Number][column_Number].num_Of_Table ) ;
			}
		} // aly pass1
		
		string fixStringToBeLikeTables( string token, int table_Number ) {
			string lower_Type = token;
			string upper_Type = token;
			
			for ( int i = 0 ; i < token.size() ; i ++ ) {
				if ( ( token[i] >= 65 ) && ( token[i] <= 90 ) )
					lower_Type[i] = token[i] + 32;
			}
			
			for ( int i = 0 ; i < token.size() ; i ++ ) {
				if ( ( token[i] >= 97 ) && ( token[i] <= 122 ) )
					upper_Type[i] = token[i] - 32;
			}
			
			if ( table_Number == 1 )
				return lower_Type;
			else if ( table_Number == 2 )
				return upper_Type;
			else if ( table_Number == 3 )
				return upper_Type;
			
			return token;
		}
		
		int getSizeOfByte( string token, int table_Number ) {
			int size = 0 ;
			if ( table_Number == 6 )
				size = token.size() / 2 ;			
			else if ( table_Number == 7 )
				size = token.size() ;
			else
				cout << "ERROR" << endl ; 	
			return size ;
		}
		
		int stringToInterger_hex( string token ) { // 16->10
			int sum = 0 ;
			for ( int i = 0 ; i < token.size() ; i++ ) {
				sum = sum * 16 + token[i] - 48 ;
			} // for
			
			return sum ;
		} // stringToInteger()
		
		int stringToInterger( string token ) { // 10->10
			int sum = 0 ;
			for ( int i = 0 ; i < token.size() ; i++ ) {
				sum = sum * 10 + token[i] - 48 ;
			} // for
			
			return sum ;
		} // stringToInteger()
		
		void setLiteralTableAddress() {
			int literal_Num = 1;
			for ( int i = 0 ; i < 100 ; i ++ ) {
				if ( ( literal_Table[i].name != "-1" ) && ( literal_Table[i].num_Of_Table == 6 ) ) {
					integer_Table[literal_Table[i].num_Of_Column].address = pc;
					literal_Table[i].address = pc;
					pc = pc + 3;
					setLiteralBufferTable( line_Counter_Temp+5*literal_Num, locTemp+3*(literal_Num-1), literal_Num, 6, literal_Table[i].name );
					literal_Num ++;
				}
				
				if ( ( literal_Table[i].name != "-1" ) && ( literal_Table[i].num_Of_Table == 7 ) ) {
					string_Table[literal_Table[i].num_Of_Column].address = pc;
					literal_Table[i].address = pc;
					pc = pc + 3;
					setLiteralBufferTable( line_Counter_Temp+5*literal_Num, locTemp+3*(literal_Num-1), literal_Num, 7, literal_Table[i].name );
					literal_Num ++;
				}
			}
		}
		
		void setLiteralBufferTable( int line, int loc, int serialNum, int tableNum, string name ) {
			LiteralBuffer temp;
			temp.name = name;
			temp.line = line;
			temp.loc = loc;
			temp.serialNum = serialNum;
			if ( tableNum == 6 ) {
				temp.statement = "=x'" + name + "'" ;
				temp.ASCIICode = getByteASCIICode( temp.name, 6 ) ; 
				for ( int i = 0 ; i < temp.ASCIICode.size() ; i++ ) {
					if ( ( temp.ASCIICode[i] > 96 ) && ( temp.ASCIICode[i] < 123 ) )
						temp.ASCIICode[i] = temp.ASCIICode[i] - 32 ;
				} // for
			} // if		
			else if ( tableNum == 7 ) {
				temp.statement = "=c'" + name + "'" ;
				temp.ASCIICode = getByteASCIICode( temp.name, 7 ) ;
				temp.ASCIICode = decimalObjectCodeToHexadecimal_NbitString( temp.ASCIICode ) ;
			} // else if
			
			if ( hasNoNameInLiteralBufferTable( temp.name ) )
				literal_Buffer_Table.push_back( temp ) ;
		}
		
		string getByteASCIICode( string s, int tableNum ) {
			int temp = 0 ;
			string code = "" ;
			if ( tableNum == 6 )
				code = s ;	
			else if ( tableNum == 7 ) {
				for ( int i = 0 ; i < s.size() ; i++ ) {
					temp = s[i] ;
					howManyDigits( temp, digits, digitsVector ) ;
					code = code + integerToString( temp ) ;
				} // for
			} // else if
			
			return code ;
		} // getByteASCIICode()
		
		void howManyDigits( int temp, int & digits, vector<int> & digitsVector ) {
			int times = 0 ;
			while ( temp != 0 ) {
				temp = temp / 10 ;
				times++ ;
			} // while
			
			digits = times ;
			digitsVector.push_back( digits ) ;
		} // howManyDigits()
		
		string integerToString(int x) {
	    	stringstream ss ;
	    	ss << x ;
	   		return ss.str() ;
		} // integerToString
		
		string decimalObjectCodeToHexadecimal_NbitString( string s ) {
			string final = "" ;
			int index = 0 ;
			int dividend = 0 ;
			string temp = "" ;
			for ( int i = 0 ; i < s.size() ; i = i + 2 ) {
				temp.push_back( s[i] ) ;
				temp.push_back( s[i+1] ) ;
				if ( digitsVector[index] == 3 ) {
					temp.push_back( s[i+2] ) ;
					i++ ;
				} // if
					
				//cout << temp << "    " << i << "    " ;
				dividend = stringToInterger( temp ) ;
				final = final + decimalNumberToHexadecimalString( dividend ) ;
				temp = "" ;
				index++ ;
			} // for
			
			while ( !digitsVector.empty() ) {
				digitsVector.pop_back() ;
			} // while
			
			vector<int>(digitsVector).swap(digitsVector) ;
			return final ;
		} // decimalObjectCodeToHexadecimal_Nbit()
		
		string decimalObjectCodeToHexadecimal( string s, int tableNum ) {
			string final = "" ;
			
			int dividend = 0, remainder = 0 ;
			if ( ( tableNum == 6 ) && isBYTE ) {
				final = s ;
			} // if
			
			else if ( ( tableNum == 6 ) && isWORD ) {
				dividend = stringToInterger( s ) ;
				final = decimalNumberToHexadecimalString( dividend ) ;
			} // else if
			
			else if ( tableNum == 7 ) {
				string temp = "" ;
				for ( int i = 0 ; i < s.size() ; i = i + 2 ) {
					temp.push_back( s[i] ) ;
					temp.push_back( s[i+1] ) ;
					//cout << temp << "    " << i << "    " ;
					dividend = stringToInterger( temp ) ;
					final = final + decimalNumberToHexadecimalString( dividend ) ;
					temp = "" ;
				} // for
			} // else if
			
			return final ;
		} // decimalObjectCodeToHexadecimal()
		
		string decimalNumberToHexadecimalString( int dividend ) { // 10->16
			string final = "" ;
			string temp = "" ;
			int remainder = 0 ;
			while ( dividend != 0 ) {
				remainder = dividend % 16 ;
				temp = integerToString( remainder ) ;
				if ( remainder == 10 )
					temp = "A" ;
				else if ( remainder == 11 )
					temp = "B" ;
				else if ( remainder == 12 )
					temp = "C" ;
				else if ( remainder == 13 )
					temp = "D" ;
				else if ( remainder == 14 )
					temp = "E" ;
				else if ( remainder == 15 )
					temp = "F" ;
				final = temp + final ;
				dividend = dividend / 16 ;
				
			} // while
			
			return final ;
		} // decimalNumberToHexadecimal()

		bool hasNoNameInLiteralBufferTable( string name ) {
			bool decide = true ;
			for ( int i = 0 ; i < literal_Buffer_Table.size() ; i++ ) {
				if ( literal_Buffer_Table[i].name == name ) {
					decide = false ;
				} // if
			} // for
			
			return decide ;
		} // hasNoNameInLiteralBufferTable()
		
		void hashInLiteralTable( string s, int tableNum ) {
			int hashValue = 0 ;
			while ( ( literal_Table[hashValue].name != "-1" ) && ( literal_Table[hashValue].name != s ) )				hashValue++ ;
			
			literal_Table[hashValue].num_Of_Column = hashValue ;
			literal_Table[hashValue].num_Of_Table = tableNum ;
			literal_Table[hashValue].name = s ;
			literal_Table[hashValue].serialNum = literalNum ;
			literalNum++ ;
		} // hashInLiteralTable()
		
// -----------------------pass2 below------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		void pass2() {
			for ( int i = 0 ; i < token_List.size() ; i ++ ) {
				if( !isSTART )
					locTemp = pc;
				else
					isSTART = false;
					
				for ( int j = 0 ; j < token_List[i].size() ; j ++ ) {
					if ( isXE )
						analysis_2_XE( i, j ) ;
					else
						analysis_2( i, j ) ;
				} 
				
				if ( isSTART ) {
					locTemp = start_Location;
					pc = start_Location;
				} 
				
				if ( !isXE ) {
					if ( operandNumber > 1 && isIndexRegister ) {
						standardType.x = 1;
						standardType.address = standardType.address + indexRegister;
					}
					
					AssemblerList row ;
					setARow_pass2( row, i ) ;
					assemblerList[i].objectCode = row.objectCode ;
					if ( isEQU )
						assemblerList[i].loc = row.loc;
				}
				else {
					if ( operandNumber == ( instructionSet[instructionSetEntry].operand + 1 ) && isIndexRegister && ( ( xeType.format == 3 ) || ( xeType.format == 4 ) ) )
						xeType.x = 1 ;
					else
						xeType.x = 0 ;
					
					if ( xeType.disp == -1 )
						xeType.disp = 0 ;
						
					AssemblerList row ;
					setARow_pass2_XE( row, i ) ;
					assemblerList[i].objectCode = row.objectCode ;
					if ( isEQU )
						assemblerList[i].loc = row.loc;
				}
				
				if ( !token_List[i].empty() )
					line_Counter_Temp += 5;
				
				// initialize
				instructionSetEntry = 0 ;
				operandNumber = 0 ;
				balance = -1 ;
				isBYTE = false ;
				isWORD = false ;
				isRESB = false ;
				isRESW = false ;
				isPseudo = false ;
				isComment = false ;
				isExcutable = false ;
				isEND = false ;
				isEQU = false ;
				isBASE = false ;
				isLTORG = false ;
				standardType.address = 0 ;
				standardType.opcode = 0 ;
				standardType.x = 0 ;
				isIndexRegister = false ;
				// XE
				isIndexRegister = false ;
				xeType.format = 0 ;
				xeType.opcode = 0 ;
				xeType.r1 = 0 ;
				xeType.r2 = 0 ;
				xeType.n = 0 ;
				xeType.i = 0 ;
				xeType.x = 0 ;
				xeType.b = 0 ;
				xeType.p = 0 ;
				xeType.e = 0 ;
				xeType.disp = -1 ;
				xeType.address = -1 ;
				isF4 = false ;
				r1IsDone = false ;
			}
		}
		
		void analysis_2( int line, int column ) {
			string token = token_List[line][column].token_Data;
			fixStringToBeLikeTables( token, token_List[line][column].num_Of_Table );
			if ( token_List[line][column].num_Of_Table == 1 ) {
				standardType.opcode = instructionSet[token_List[line][column].num_Of_Column-1].opcode;
				instructionSetEntry = token_List[line][column].num_Of_Column - 1;
				pc = pc + 3;
			} 
			else if( token_List[line][column].num_Of_Table == 2 ) {
				isPseudo = true;
				if ( token == "START" ) {
					isSTART = true;
					start_Location = stringToInterger_hex( token_List[line][column+1].token_Data );
				}
				else if ( token == "BYTE" ) {
					isBYTE = true;
					pc = pc + getSizeOfByte( token_List[line][column+2].token_Data, token_List[line][column+2].num_Of_Table );
					ASCIICode = getByteASCIICode( token_List[line][column+2].token_Data, token_List[line][column+2].num_Of_Table );
					ASCIICode = decimalObjectCodeToHexadecimal( ASCIICode, token_List[line][column+2].num_Of_Table );
				}
				else if ( token == "WORD" ) {
					isWORD = true;
					pc = pc + 3;
					ASCIICode = token_List[line][column+1].token_Data;
					ASCIICode = decimalObjectCodeToHexadecimal( ASCIICode, token_List[line][column+1].num_Of_Table );
					ASCIICode = string( 6 - ASCIICode.size(), '0' ) + ASCIICode;
				}
				else if ( token == "RESB" ) {
					isRESB = true;
					pc = pc + stringToInterger( token_List[line][column+1].token_Data );
				}
				else if ( token == "RESW" ) {
					isRESW = true;
					pc = pc + stringToInterger( token_List[line][column+1].token_Data ) * 3;
				}
				else if ( token == "END" ) {
					isEND = true;
					setLiteralTableAddress();
				}
				else if ( token == "EQU" ) {
					isEQU = true;
					if ( token_List[line][column+1].token_Data == "*" )
						locTempForEQU = locTemp - 3;
					else if ( column + 2 >= token_List[line].size() ) {
						if ( token_List[line][column+1].num_Of_Table != 5 )
							locTempForEQU = stringToInterger( token_List[line][column+1].token_Data );
						else
							locTempForEQU = symbol_Table[token_List[line][column+1].num_Of_Column].address;
					}
					else {
						if ( column + 3 >= token_List[line].size() ) 
							cout << "Syntax Error !!!" << endl;
						else if ( token_List[line][column+2].token_Data == "+" )
							locTempForEQU = symbol_Table[token_List[line][column+1].num_Of_Column].address + symbol_Table[token_List[line][column+3].num_Of_Column].address;
						else if ( token_List[line][column+2].token_Data == "-" ) 
							locTempForEQU = symbol_Table[token_List[line][column+1].num_Of_Column].address - symbol_Table[token_List[line][column+3].num_Of_Column].address;
					}
				}
			}
			else if ( token_List[line][column].num_Of_Table == 3 ) {
				if ( operandNumber == instructionSet[instructionSetEntry].operand ) {
					if ( token == "X" )
						isIndexRegister = true;
					else
						cout << "ERROR : Index Register use Error !!!" << endl;
				}
				
				operandNumber ++;
				balance ++;
			}
			else if ( token_List[line][column].num_Of_Table == 4 ) {
				if ( token == "." )
					isComment = true;
				if ( token == "," )
					balance = balance - 2;
			}
			else if ( token_List[line][column].num_Of_Table == 5 ) {
				if ( ( symbol_Table[token_List[line][column].num_Of_Column].address == -1 ) && !isEND )
					cout << "Error : Symbol undefined !!!" << endl;
				if ( column != 0 ) {
					standardType.address = symbol_Table[token_List[line][column].num_Of_Column].address;
					operandNumber ++;
					balance ++;
				}
			}
			else if ( token_List[line][column].num_Of_Table == 6 ) {
				if ( ( column > 0 ) && ( token_List[line][column-1].token_Data == "'/" ) ) {
					if ( ( column > 1 ) && ( token_List[line][column-2].token_Data == "=" ) )
						standardType.address = getAddressInLiteralTable( token_List[line][column].token_Data );
					else
						standardType.address = stringToInterger_hex( token_List[line][column].token_Data );
				}
				else {
					if ( ( column > 1 ) && ( token_List[line][column-2].token_Data == "=" ) )
						standardType.address = getAddressInLiteralTable( token_List[line][column].token_Data );
					else
						standardType.address = stringToInterger( token_List[line][column].token_Data );
				}
				
				operandNumber ++;
				balance ++;
			}
			else if ( token_List[line][column].num_Of_Table == 7 ) {
				operandNumber ++;
				balance ++;
			}
		} // analysis_2
		
		void analysis_2_XE( int line, int column ) {
			if ( token_List[line][column].token_Data == "LDT" ) {
				int a = 0;
				a = 1;
			}
			
			string token = token_List[line][column].token_Data;
			fixStringToBeLikeTables( token, token_List[line][column].num_Of_Table );
			if ( token_List[line][column].num_Of_Table == 1 ) {
				xeType.opcode = instructionSet[token_List[line][column].num_Of_Column-1].opcode;
				xeType.format = instructionSet[token_List[line][column].num_Of_Column-1].format;
				instructionSetEntry = token_List[line][column].num_Of_Column - 1;
				if ( isFormat4( token, line, column ) )
					xeType.format = 4;
				if ( xeType.format == 1 )
					pc = pc + 1;
				else if ( xeType.format == 2 ) 
					pc = pc + 2;
				else if ( xeType.format == 3 ) {
					xeType.n = 1;
					xeType.i = 1;
					xeType.e = 0;
					setN( xeType.opcode, xeType.n );
					setI( xeType.opcode, xeType.i );
					pc = pc + 3;
				}
				else if ( xeType.format == 4 ) {
					xeType.n = 1 ;
					xeType.i = 1 ;
					xeType.b = 0 ;
					xeType.p = 0 ;
					xeType.e = 1 ;
					setN( xeType.opcode, xeType.n ) ;
					setI( xeType.opcode, xeType.i ) ;
					pc = pc + 4 ;
				}
			}
			else if ( token_List[line][column].num_Of_Table == 2 ) {
				isPseudo = true;
				if ( token == "START" ) {
					isSTART = true;
					start_Location = stringToInterger_hex( token_List[line][column+1].token_Data );
				}
				else if ( token == "BYTE" ) {
					isBYTE = true;
					pc = pc + getSizeOfByte( token_List[line][column+2].token_Data, token_List[line][column+2].num_Of_Table );
					ASCIICode = getByteASCIICode( token_List[line][column+2].token_Data, token_List[line][column+2].num_Of_Table );
					ASCIICode = decimalObjectCodeToHexadecimal( ASCIICode, token_List[line][column+2].num_Of_Table );
				}
				else if ( token == "WORD" ) {
					isWORD = true;
					pc = pc + 3;
					ASCIICode = token_List[line][column+1].token_Data;
					ASCIICode = decimalObjectCodeToHexadecimal( ASCIICode, token_List[line][column+1].num_Of_Table );
					ASCIICode = string( 6 - ASCIICode.size(), '0' ) + ASCIICode;
				}
				else if ( token == "RESB" ) {
					isRESB = true;
					pc = pc + stringToInterger( token_List[line][column+1].token_Data );
				}
				else if ( token == "RESW" ) {
					isRESW = true;
					pc = pc + stringToInterger( token_List[line][column+1].token_Data ) * 3;
				}
				else if ( token == "END" ) {
					isEND = true;
					setLiteralTableAddress();
					for ( int g = 0 ; g < 100 ; g ++ ) {
						literal_Table[g].address = -1 ;
						literal_Table[g].num_Of_Column = -1 ;
						literal_Table[g].name = "-1" ;
						literal_Table[g].serialNum = -1 ;
						literal_Table[g].num_Of_Table = -1 ;
					}
					
				}
				else if ( token == "EQU" ) {
					isEQU = true;
					if ( token_List[line][column+1].token_Data == "*" )
						locTempForEQU = locTemp - 3;
					else if ( column + 2 >= token_List[line].size() ) {
						if ( token_List[line][column+1].num_Of_Table != 5 )
							locTempForEQU = stringToInterger( token_List[line][column+1].token_Data );
						else
							locTempForEQU = symbol_Table[token_List[line][column+1].num_Of_Column].address;
					}
					else {
						if ( column + 3 >= token_List[line].size() )
							cout << "Syntax Error !!!" << endl;
						else if ( token_List[line][column+2].token_Data == "+" ) 
							locTempForEQU = symbol_Table[token_List[line][column+1].num_Of_Column].address + symbol_Table[token_List[line][column+3].num_Of_Column].address;
						else if ( token_List[line][column+2].token_Data == "-" ) 
							locTempForEQU = symbol_Table[token_List[line][column+1].num_Of_Column].address - symbol_Table[token_List[line][column+3].num_Of_Column].address;
					}
				}
				else if ( token == "BASE" ) {
					isBASE = true;
					BASERegister = stringToInterger( token_List[line][column+1].token_Data );
				}
				else if ( token == "LTORG" ) {
					isLTORG = true;
					setLiteralTableAddress();
					for ( int g = 0 ; g < 100 ; g ++ ) {
						literal_Table[g].address = -1 ;
						literal_Table[g].num_Of_Column = -1 ;
						literal_Table[g].name = "-1" ;
						literal_Table[g].serialNum = -1 ;
						literal_Table[g].num_Of_Table = -1 ;
					}  
				}
			}
			else if ( token_List[line][column].num_Of_Table == 3 ) {
				if ( xeType.format == 2 ) {
					if ( ( xeType.r1 == 0 ) && !r1IsDone ) {
						xeType.r1 = token_List[line][column].num_Of_Column - 1;
						r1IsDone = true;
					}
					else
						xeType.r2 = token_List[line][column].num_Of_Column - 1;
				}
				else if ( xeType.format == 3 ) {
					if ( ( operandNumber == instructionSet[instructionSetEntry].operand ) && token == "X" )
						isIndexRegister = true;
					else
						cout << "ERROR : Index Register use Error !!!" << endl; 
				}
				
				operandNumber ++;
				balance ++;
			}
			else if ( token_List[line][column].num_Of_Table == 4 ) {
				if ( token == "." )
					isComment = true;
				else
					balance = balance - 2;
			}
			else if ( token_List[line][column].num_Of_Table == 5 ) {
				if ( ( symbol_Table[token_List[line][column].num_Of_Column].address == -1 ) && !isEND )
					cout << "Error : Symbol undefined !!!" << endl;
				
				if ( isBASE )
					BASERegister = symbol_Table[token_List[line][column].num_Of_Column].address;
				if ( column != 0 ) {
					if ( token_List[line][column-1].token_Data == "#" ) {
						xeType.i = 1;
						xeType.n = 0;
						setN( xeType.opcode, xeType.n );
						setI( xeType.opcode, xeType.i );
					}
					else if ( token_List[line][column-1].token_Data == "@" ) {
						xeType.i = 0;
						xeType.n = 1;
						setN( xeType.opcode, xeType.n );
						setI( xeType.opcode, xeType.i );
					}

					if ( xeType.format == 3 ) {
						xeType.b = 0;
						xeType.p = 1;
						int dispTemp = symbol_Table[token_List[line][column].num_Of_Column].address - pc;
						if ( dispTemp < -2048 ) {
							xeType.b = 1;
							xeType.p = 0;
							dispTemp = symbol_Table[token_List[line][column].num_Of_Column].address - BASERegister;
						}
						
						xeType.disp = dispTemp;
					}
					else if ( xeType.format == 4 )
						xeType.address = symbol_Table[token_List[line][column].num_Of_Column].address;
					
					operandNumber ++;
					balance ++;
				}
			}
			else if ( token_List[line][column].num_Of_Table == 6 ) {
				if ( xeType.format == 2 ) {
					if ( ( xeType.r1 == 0 ) && !r1IsDone )
						xeType.r1 = stringToInterger( token );
					else
						xeType.r2 = stringToInterger( token );
				}
				else if ( ( token_List[line][column-1].num_Of_Table == 1 ) && ( xeType.format == 3 ) ) {
					xeType.i = 0 ;
					xeType.n = 0 ;
					xeType.b = 0 ;
					xeType.p = 0 ;
					int dispTemp = stringToInterger( token ) ;
					xeType.disp = dispTemp ;
				} 
				else if ( ( token_List[line][column-1].token_Data == "#" ) && ( xeType.format == 3 ) ) {
					xeType.i = 1 ;
					xeType.n = 0 ;
					xeType.b = 0 ;
					xeType.p = 0 ;
					setN( xeType.opcode, xeType.n ) ;
					setI( xeType.opcode, xeType.i ) ;
					int dispTemp = stringToInterger( token ) ;
					xeType.disp = dispTemp ;
				}
				else if ( ( xeType.format == 3 ) && ( token_List[line][column-1].token_Data != "#" ) ) {
					if ( ( column > 0 ) && ( token_List[line][column-1].token_Data == "/'" ) ) {
						xeType.b = 0;
						xeType.p = 1;
						if ( ( column > 1 ) && ( token_List[line][column-2].token_Data == "=" ) ) {
							int dispTemp = getAddressInLiteralTable( token ) - pc;
							xeType.disp = dispTemp;
						}
						else {
							int dispTemp = stringToInterger_hex( token );
							xeType.disp = dispTemp;
						}
					}
					else {
						xeType.b = 0;
						xeType.p = 1;
						if ( ( column > 1 ) && ( token_List[line][column-2].token_Data == "=" ) ) {
							int dispTemp = getAddressInLiteralTable( token ) - pc;
							xeType.disp = dispTemp;
						}
						else {
							int dispTemp = stringToInterger( token );
							xeType.disp = dispTemp;
						}
					}
				}
				else if ( ( token_List[line][column-1].token_Data == "#" ) && ( xeType.format == 4 ) ) {
					xeType.i = 1 ;
					xeType.n = 0 ;
					xeType.b = 0 ;
					xeType.p = 0 ;
					setN( xeType.opcode, xeType.n ) ;
					setI( xeType.opcode, xeType.i ) ;
					int addressTemp = stringToInterger( token ) ;
					xeType.address = addressTemp ;
				}
				else if ( ( xeType.format == 4 ) && ( token_List[line][column-1].token_Data != "#" ) ) {
					if ( ( column > 0 ) && ( token_List[line][column-1].token_Data == "/'" ) ) {
						if ( ( column > 1 ) && ( token_List[line][column-2].token_Data == "=" ) ) {
							xeType.b = 0 ;
							xeType.p = 0 ;
							int addressTemp = getAddressInLiteralTable( token ) - pc;
							xeType.address = addressTemp;
						}
						else {
							xeType.b = 0 ;
							xeType.p = 0 ;
							int addressTemp= stringToInterger_hex( token );
							xeType.address = addressTemp;
						}
					}
					else {
						if ( ( column > 1 ) && ( token_List[line][column-2].token_Data == "=" ) ) {
							xeType.b = 0 ;
							xeType.p = 0 ;
							int addressTemp = getAddressInLiteralTable( token );
							xeType.address = addressTemp;
						}
						else {
							xeType.b = 0 ;
							xeType.p = 0 ;
							int addressTemp= stringToInterger( token );
							xeType.address = addressTemp;
						}
					}
				}
				
				operandNumber ++;
				balance ++;
			}
			else if ( token_List[line][column].num_Of_Table == 7 ) {
				if ( xeType.format == 3 ) {
					if ( ( column > 1 ) && ( token_List[line][column-2].token_Data == "=" ) ) {
						xeType.b = 0 ;
						xeType.p = 1 ;
						int dispTemp = getAddressInLiteralTable( token ) - pc;
						xeType.disp = dispTemp;
					}
				}
				else if ( xeType.format == 4 ) {
					if ( ( column > 1 ) && ( token_List[line][column-2].token_Data == "=" ) ) {
						xeType.b = 0 ;
						xeType.p = 0 ;
						int addressTemp = getAddressInLiteralTable( token );
						xeType.address = addressTemp;
					}
				}
				
				operandNumber ++;
				balance ++;
			}
		} // analysis_2_XE
		
		int getAddressInLiteralTable( string name ) {
			int num = 0 ;
			for ( int i = 0 ; i < literal_Buffer_Table.size() ; i++ ) {
				if ( name == literal_Buffer_Table[i].name ) {
					num = literal_Buffer_Table[i].loc ;
				} // if
			} // for
			
			return num ;
		} // getAddressInLitteralTable()
		
		void setN( int & opcode, int n ) {
			int bit = opcode / 2  ; 
			bit = opcode % 2  ; 
			if ( ( bit == 0 ) && ( n == 1 ) ) {
				opcode = opcode + 2 ;
			} // if
			else if ( ( bit == 1 ) && ( n == 0 ) ) {
				opcode = opcode - 2 ;
			} // else if
		} // setN()
		
		void setI( int & opcode, int i ) {
			int bit = opcode % 2  ; 
			if ( ( bit == 0 ) && ( i == 1 ) ) {
				opcode = opcode + 1 ;
			} // if
			else if ( ( bit == 1 ) && ( i == 0 ) ) {
				opcode = opcode - 1 ;
			} // else if
		} // setI()
		
		void setARow_pass2( AssemblerList & row, int i ) {
			row.line = line_Counter_Temp ; // �����@��Row��line 
			if ( isBYTE )
				row.objectCode = ASCIICode ;		
			else if ( isWORD )
				row.objectCode = ASCIICode ;	
			else if ( isPseudo ) {
				row.objectCode = "-1" ;
				if ( isEQU )
					row.loc = locTempForEQU;
			}
			else if ( token_List[i].empty() )
				row.objectCode = "-1" ;
			else if ( isComment && !isExcutable )
				row.objectCode = "-1" ;
			else
				row.objectCode = standardTypeToObjectCode( standardType );
		} // setARow_pass2()
		
		void setARow_pass2_XE( AssemblerList & row, int i ) {
			row.line = line_Counter_Temp ; // �����@��Row��line 
			if ( isBYTE )
				row.objectCode = ASCIICode ;	
			else if ( isWORD )
				row.objectCode = ASCIICode ;
			else if ( isPseudo ) {
				row.objectCode = "-1" ;
				if ( isEQU )
					row.loc = locTempForEQU;
			}		
			else if ( token_List[i].empty() )
				row.objectCode = "-1" ;
			else if ( isComment && !isExcutable )
				row.objectCode = "-1" ;	
			else {
				if ( xeType.format == 1 ) 
					row.objectCode = XETypeToObjectCode_1( xeType ) ;
				else if ( xeType.format == 2 )
					row.objectCode = XETypeToObjectCode_2( xeType ) ;
				else if ( xeType.format == 3 )
					row.objectCode = XETypeToObjectCode_3( xeType ) ;
				else if ( xeType.format == 4 )
					row.objectCode = XETypeToObjectCode_4( xeType ) ;			
			} // else
		} // setARow_pass2_XE()
		
		
		string standardTypeToObjectCode( StandardType standardType ) { // �ন�����X 
			string s1 = "" ;
			string s2 = "" ;
			string s3 = "" ;
			int a = standardType.opcode; 
			int b = standardType.x ;
			int c = standardType.address ;
			
			char opcode[8]; 
			char x[1] ;
			char address[15] ;
			
			for ( int i = 0 ; i < 8 ; i++ )
				opcode[i] = '\0' ;
			
			x[0] = '\0' ;
			
			for ( int i = 0 ; i < 15 ; i++ )
				address[i] = '\0' ;
			
			itoa(a, opcode, 2); 
			for ( int i = 0 ; i < 8 ; i++ ) {
				if ( opcode[i] != '\0' ) {
					s1.push_back(opcode[i]) ;
				} // if
			} // for
			string new_opcode = string(8 - s1.size(), '0') + s1;
			//cout << new_opcode << endl ;
			itoa(b, x, 2); 
			s2.push_back(x[0]) ;
			string new_x = s2 ;
			//cout << new_x << endl ;
			//cout << c << endl;
			itoa(c, address, 2); 
			//cout << "YES";
			for ( int i = 0 ; i < 15 ; i++ ) {
				if ( address[i] != '\0' ) {
					s3.push_back(address[i]) ;
				} // if
			} // for
			
			string new_address = string(15 - s3.size(), '0') + s3;
			//cout << "YES";
			string final = new_opcode + new_x + new_address ;
			final = binaryObjectCodeToHexadecimal( final ) ;
			return final ;
		} // standardTypeToObjectCode()
		
		string binaryObjectCodeToHexadecimal( string s ) {
			string final = "" ;
			string nibble = "" ;
			for ( int i = 0 ; i < s.size() / 4 ; i++ ) {
				nibble = s.substr( 4 * i, 4 ) ; 
				setNibble( nibble ) ;
				final = final + nibble ;	
			} // for
			return final ;
		} // binaryObjectCodeToHexadecimal()
		
		void setNibble( string & nibble ) {
			int sum = 0 ;
			for ( int i = 0 ; i < 4 ; i++ ) {
				if ( nibble[i] == '1' ) {
					sum =  sum + pow( 2, 3 - i ) ;
				} // if
			} // for
			
			nibble = intergerToString( sum ) ;
			
			if ( sum == 10 )
				nibble = "A" ;
			else if ( sum == 11 )
				nibble = "B" ;
			else if ( sum == 12 )
				nibble = "C" ;
			else if ( sum == 13 )
				nibble = "D" ;
			else if ( sum == 14 )
				nibble = "E" ;
			else if ( sum == 15 )
				nibble = "F" ;
		} // setNibble()
		
		string intergerToString(int x) {
	    	stringstream ss ;
	    	ss << x ;
	   		return ss.str() ;
		} // integerToString
		
		string XETypeToObjectCode_1( XEType xeType ) {
			string s1 = "" ;
			int a = xeType.opcode ; 
			char opcode[8]; 
			for ( int i = 0 ; i < 8 ; i++ ) // �i���l�� 
				opcode[i] = '\0' ;
	
			itoa(a, opcode, 2); // �N�Ʀr�ഫ��2�i�쪺�r���}�C 
			for ( int i = 0 ; i < 8 ; i++ ) { // �N�r���}�C�ഫ��2�i��r�� 
				if ( opcode[i] != '\0' ) {
					s1.push_back(opcode[i]) ;
				} // if
			} // for
			
			string new_opcode = string(8 - s1.size(), '0') + s1; // ��r��ɤW0 
			string final = new_opcode ; // �̫ᵲ�G (2�i��) 
			final = binaryObjectCodeToHexadecimal( final ) ; // �N2�i�쪺���G�ഫ��16�i�� 
			return final ;
		} // XETypeToObjectCode_1()
		
		string XETypeToObjectCode_2( XEType xeType ) {
			string s1 = "" ;
			string s2 = "" ;
			string s3 = "" ;
			
			int a = xeType.opcode ; 
			int b = xeType.r1 ;
			int c = xeType.r2 ;
			
			char opcode[8]; 
			char r1[4] ;
			char r2[4] ;
			for ( int i = 0 ; i < 8 ; i++ ) { // �i���l�� 
				opcode[i] = '\0' ;
			} // for
			for ( int i = 0 ; i < 4 ; i++ ) { // �i���l�� 
				r1[i] = '\0' ;
			} // for
			for ( int i = 0 ; i < 4 ; i++ ) { // �i���l�� 
				r2[i] = '\0' ;
			} // for
	
			itoa(a, opcode, 2); // �N�Ʀr�ഫ��2�i�쪺�r���}�C 
			for ( int i = 0 ; i < 8 ; i++ ) { // �N�r���}�C�ഫ��2�i��r�� 
				if ( opcode[i] != '\0' ) {
					s1.push_back(opcode[i]) ;
				} // if
			} // for
			itoa(b, r1, 2); // �N�Ʀr�ഫ��2�i�쪺�r���}�C 
			for ( int i = 0 ; i < 4 ; i++ ) { // �N�r���}�C�ഫ��2�i��r�� 
				if ( r1[i] != '\0' ) {
					s2.push_back(r1[i]) ;
				} // if
			} // for
			itoa(c, r2, 2); // �N�Ʀr�ഫ��2�i�쪺�r���}�C 
			for ( int i = 0 ; i < 4 ; i++ ) { // �N�r���}�C�ഫ��2�i��r�� 
				if ( r2[i] != '\0' ) {
					s3.push_back(r2[i]) ;
				} // if
			} // for
			
			
			string new_opcode = string(8 - s1.size(), '0') + s1; // ��r��ɤW0 
			string new_r1 = string(4 - s2.size(), '0') + s2; // ��r��ɤW0 
			string new_r2 = string(4 - s3.size(), '0') + s3; // ��r��ɤW0 
			
			string final = new_opcode + new_r1 + new_r2 ; // �̫ᵲ�G (2�i��) 
			final = binaryObjectCodeToHexadecimal( final ) ; // �N2�i�쪺���G�ഫ��16�i�� 
			return final ;
		} // XETypeToObjectCode_2()
		
		string XETypeToObjectCode_3( XEType xeType ) {
			string s1 = "" ;
			string s2 = "" ;
			
			int a = xeType.opcode ; 
			int b = xeType.disp ;
			
			char opcode[8]; 
			char disp[32] ;
	
			for ( int i = 0 ; i < 8 ; i++ ) { // �i���l�� 
				opcode[i] = '\0' ;
			} // for
			for ( int i = 0 ; i < 32 ; i++ ) { // �i���l�� 
				disp[i] = '\0' ;
			} // for
			
			
			itoa(a, opcode, 2); // �N�Ʀr�ഫ��2�i�쪺�r���}�C 
			for ( int i = 0 ; i < 8 ; i++ ) { // �N�r���}�C�ഫ��2�i��r�� 
				if ( opcode[i] != '\0' ) {
					s1.push_back(opcode[i]) ;
				} // if
			} // for
			if ( b > 0 ) {
				itoa(b, disp, 2); // �N�Ʀr�ഫ��2�i�쪺�r���}�C 
				for ( int i = 0 ; i < 12 ; i++ ) { // �N�r���}�C�ഫ��2�i��r�� 
					if ( disp[i] != '\0' ) {
						s2.push_back(disp[i]) ;
					} // if
				} // for
			} // if
			else {
				itoa(b, disp, 2); // �N�Ʀr�ഫ��2�i�쪺�r���}�C
				for ( int i = 20 ; i < 32 ; i++ ) { // �N�r���}�C�ഫ��2�i��r�� 
					if ( disp[i] != '\0' ) {
						s2.push_back(disp[i]) ;
					} // if
				} // for
			} // else
			
			
			string new_opcode = string(8 - s1.size(), '0') + s1; // ��r��ɤW0 
			string new_disp = string(12 - s2.size(), '0') + s2; // ��r��ɤW0 
			
			string final = new_opcode + integerToString( xeType.x ) + integerToString( xeType.b ) + integerToString( xeType.p ) + integerToString( xeType.e ) + new_disp ; // �̫ᵲ�G (2�i��) 
			final = binaryObjectCodeToHexadecimal( final ) ; // �N2�i�쪺���G�ഫ��16�i�� 
			return final ;
		} // XETypeToObjectCode_3()
		
		string XETypeToObjectCode_4( XEType xeType ) {
			string s1 = "" ;
			string s2 = "" ;
			
			int a = xeType.opcode ; 
			int b = xeType.address ;
			
			char opcode[8]; 
			char address[20] ;
	
			for ( int i = 0 ; i < 8 ; i++ ) { // �i���l�� 
				opcode[i] = '\0' ;
			} // for
			for ( int i = 0 ; i < 20 ; i++ ) { // �i���l�� 
				address[i] = '\0' ;
			} // for
			
			itoa(a, opcode, 2); // �N�Ʀr�ഫ��2�i�쪺�r���}�C 
			for ( int i = 0 ; i < 8 ; i++ ) { // �N�r���}�C�ഫ��2�i��r�� 
				if ( opcode[i] != '\0' ) {
					s1.push_back(opcode[i]) ;
				} // if
			} // for
			itoa(b, address, 2); // �N�Ʀr�ഫ��2�i�쪺�r���}�C 
			for ( int i = 0 ; i < 20 ; i++ ) { // �N�r���}�C�ഫ��2�i��r�� 
				if ( address[i] != '\0' ) {
					s2.push_back(address[i]) ;
				} // if
			} // for
			
			string new_opcode = string(8 - s1.size(), '0') + s1; // ��r��ɤW0 
			string new_address = string(20 - s2.size(), '0') + s2; // ��r��ɤW0 
			
			//new_opcode[6] = '1' ;
			//new_opcode[7] = '1' ;
			string final = new_opcode + integerToString( xeType.x ) + integerToString( xeType.b ) + integerToString( xeType.p ) + integerToString( xeType.e ) + new_address ; // �̫ᵲ�G (2�i��) 
			final = binaryObjectCodeToHexadecimal( final ) ; // �N2�i�쪺���G�ഫ��16�i�� 
			return final ;
		} // XETypeToObjectCode_4()
	
// -------------------------write file-------------------------------------------------------------------------
		void writeFile( string filename, vector<string> result ) { // �g�ɿ�X 
			fstream outFile ;
			string name = "" ;
			filename.erase( filename.end() - 4, filename.end() ); 
			if ( isXE == 0 )
				name = "_output.txt" ;
			else
				name = "_outputXE.txt" ;
			outFile.open( ( filename + name ).c_str() , fstream::out ) ;
			if ( !outFile.is_open() ) {
				cout << "Error" << endl ;
				return ;
			} // if

			outFile << "Line\tLoc\tSource statement\t\tObject code" << endl ;
			for ( int i = 0 ; i < result.size() ; i++ ) {
				if ( assemblerList[i].line != -1 )
					outFile << assemblerList[i].line << "\t" ;
				else
					outFile << "\t" ;
				
				if ( assemblerList[i].loc != -1 ) {
					outFile << setiosflags(ios::uppercase) << hex ;
					outFile << setfill('0') << setw(4) << assemblerList[i].loc << "\t" ;
					outFile << dec ;
				} // if
				else
					outFile << "\t" ;
					
				outFile << result[i] << "\t\t" ;
				if ( assemblerList[i].objectCode != "-1" )
					outFile << assemblerList[i].objectCode << endl ;
				else
					outFile << endl ;
				
				if ( hasENDOrLTORG( i ) ) {
					int times = 0 ;
					for ( int j = 0 ; j < literal_Buffer_Table.size() ; j++ ) {
						if ( literal_Buffer_Table[j].loc <= assemblerList[i+1].loc ) {
							outFile << literal_Buffer_Table[j].line << "\t" << setfill('0') << setw(4) << hex << literal_Buffer_Table[j].loc << dec ;
							outFile << "\t" << literal_Buffer_Table[j].statement << "\t" << literal_Buffer_Table[j].ASCIICode << endl ;
							times++ ;
							literal_Buffer_Table.erase( literal_Buffer_Table.begin() + j );
						}
					} // for
					if ( ( i + 1 ) < result.size() ) {
						assemblerList[i+1].line = assemblerList[i+1].line + times*5 ;
					} // if
				} // if
			} // for
			
			outFile.close() ;
		} // writeFile()
		
		bool hasENDOrLTORG( int i ) {
			bool decide = false ;
			for ( int j = 0 ; j < token_List[i].size() ; j ++ ) {
				if ( token_List[i][j].token_Data == "END" || token_List[i][j].token_Data == "LTORG" ) {
					decide = true;
					break;
				}
			}
			
			return decide ;
		} // hasEndOrLiteral()
		
		bool isWhiteSpace( char temp_Ch ) {
			if ( temp_Ch == '\t' || temp_Ch == ' ' || temp_Ch == '\n' )
				return true;
			return false;
		}
		
	};

string getFileName() {
	string file_Name;
	while ( true ) {
        cout << "Input file name of SIC or SICXE datas ( 0->quit ) : " ;
        cin >> file_Name;
        if ( file_Name == "0" )
            return "0";        
        else {
			fstream fs(file_Name.c_str());
		    if ( fs.is_open() ) {
		    	cout << "Got the File!!" << endl;
		    	cout << file_Name << endl; 
		    	break;
			}
		    else
				cout << "File does not exist!!" << endl;
			
			fs.close();
		} // else
	} // while ����Ū�J���X�����ɮ׶}�ҥ��T  
	
	return file_Name;
}

int main() {
start:
	Table tb;
	tb.readTable();
    Lexer lx( tb.getTable4() );
    lx.analysis( getFileName(), tb.getAllTable() );
    //lx.writeFile();
    Assembler ab( lx.changeTkFormat(), tb.getTkTable() , lx.get567Table() );
    cout << "SIC(Input: 0) or SICXE(Input: 1) : " ;
	cin >> ab.isXE ;
    ab.setInstructureSet();
    ab.pass1();
    ab.line_Counter_Temp = 0 ;
	ab.pc = 0x0000 ;
	ab.start_Location = 0x0000 ;
	ab.locTemp = 0x0000 ;
	ab.locTempForEQU = 0x0000 ;
	ab.pass2();
	ab.writeFile( lx.getNameOFFile(), lx.getTokenResult() );
	cout << "Assembler Done !! Keep work or not ? ( 1 work, 0 quit ) : ";
	string ans;
	cin >> ans;
	if ( ans == "1" ) {
		cout << endl;
		goto start;
	}
	
    system( "pause" ); // pause the display
    return 0;
}

