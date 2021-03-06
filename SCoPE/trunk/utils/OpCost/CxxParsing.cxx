//==========================================================================
//	CxxParsing.cxx
//	Author: Hector Posadas, Juan Castillo, David Quijano
//	Date: 12-12-2007
//	Description: Static Parser for execution time estimation and cache modeling.
//  Original grammar obtained from http://willink.me.uk/projects/fog/index.html
//  These files comprises a pre-built demonstration of the superset C++ grammar
//  from FOG.
//==========================================================================
//  Copyright (C) 2008 Design of Systems on Silicon(DS2)
//  Main developer: University of Cantabria
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License (GPL) or GNU Lesser General Public License(LGPL) as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License (GPL) or GNU Lesser General Public License(LGPL) for more details.
//
//  You should have received a copy of the GNU General Public License (GPL) or GNU Lesser General Public License(LGPL)
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  For more information about SCoPE you can visit
//  http://www.teisa.unican.es/scope or write an e-mail to
//  scope@teisa.unican.es or a letter to SCoPE, GIM - TEISA, University
//  of Cantabria, AV. Los Castros s/n, ETSIIT, 39005, Santander, Spain
//==========================================================================

/*!
 * \file CxxParsing.cxx
 * \brief Static Parser for execution time estimation and cache modeling
 */

#include <iostream>
#include <memory.h>
#include <vector>
#include <time.h>
#include "CxxAnnotation.h"
#include "types.h"

using namespace std;

extern CxxToken *yylex_token();

#ifdef BISON_PP_CLASS
	BISON_PP_CLASS theParser;
	#define PARSE_DOT theParser .
	#define PARSE_SCOPE BISON_PP_CLASS ::
#else
	#define PARSE_DOT
	#define PARSE_SCOPE
	extern int yydebug;
	#ifndef YYEMPTY
		#define YYEMPTY -1
	#endif
#endif

std::vector<char *> syscalls;
std::vector<char *> uc_syscalls;

size_t bang_depth = 0;
size_t error_count = 0;
size_t warning_count = 0;
size_t marked_error_count = 0;

static CxxToken **tokenBuffer = NULL;						// Allocated buffer
static YACC_MARK_TYPE tokenReadIndex = 0;					// Read index
static size_t tokenSize = 0;								// Allocate buffer size
static YACC_MARK_TYPE tokenWriteIndex = 0;					// Write index
int tokenMarkDepth = 0;			        					// Write index
CxxToken *primed_tokens[3] = {NULL, NULL, NULL};			// Restarting sequence
static void token_put(CxxToken *aToken);

void yyset_in(FILE *fd);
void yyset_out(FILE *fd);
FILE * yyget_out();
int yylex_destroy(void);
void get_filename(char *arg);

CxxToken *angleToken;
CxxToken *colonToken;
CxxToken *hashToken;
CxxToken *plusToken;
CxxToken *minusToken;

parsing_struct_t parsing_struct;
parsing_options_t parsing_options;
bool in_type1 = false;

extern int searching_time[20];
extern int searching_instr[20];
extern int searching_nested_loops[20];
extern int nested_for_ind;
extern int nested_search_ind;

/*!
 * \fn main(int argc, char *argv[])
 * \brief Parser entry point
 *
 * Parser options:
 *   -a: activate parsing from the beginning. If not, only detected user code is parsed
 *   -s: replace syscalls by an equivalent 'uc_'syscall
 *   -t: add timing annotation information
 *   -c processor: define the processor where the code will run
 *   -i: prepare the code to model instruction caches
 *   -k: print each keywork in stdout
 *   -l: print the number of the line analyzed
 *   -m: print marks when gramatical rules are discarded during checking
 *   -v: verbose mode
 *   -y: print all gramatical operations (shifts and reductions)
 */
int main(int argc, char *argv[]) {
	int c;
	char *file_f = NULL;
	char *aux;
	int int_f;
	bool valid_file = false;
	struct timespec tp;
	FILE *uc_for_header;

	//Initialize parsing options:
	parsing_options.compiler_path = (char *)malloc(MAX_FILENAME_SIZE);
	aux = getenv("SCOPE_HOME");
	if (aux == NULL) {
		cerr << "#ERROR: SCOPE_HOME not set" << endl;
		exit(1);
	}
	strcpy(parsing_options.compiler_path, aux);
	parsing_options.proc_type = (char *)malloc(MAX_FILENAME_SIZE);
	strcpy(parsing_options.proc_type, "default");
	parsing_options.input_file = (char *)malloc(MAX_FILENAME_SIZE);
	parsing_options.output_file = (char *)malloc(MAX_FILENAME_SIZE);
	parsing_options.verbose_mode = false;
	parsing_options.instruction_cache = false;
	parsing_options.echo_line_text = false;
	parsing_options.current_pass = DEFAULT_PASS;	//single annotation with operators costs
	parsing_options.provide_out_file = false;
	parsing_options.parse_syscalls = false;
	parsing_options.annotate_time = false;

	//Initialize parsing struct:
	parsing_struct.gramm_status = 0;
	parsing_struct.block_id = 0;
	parsing_struct.for_id = 0;
	parsing_struct.segment_time = 0;
	parsing_struct.segment_instr = 0;
	parsing_struct.total_instr = 0;
	for (int i=0 ; i < MAX_NESTED_BLOCKS ; i++) {
		parsing_struct.block[i].block_type = OTHER_BLOCK;
		parsing_struct.block[i].init_id = 0;
		parsing_struct.block[i].inc_id = 0;
	}
	parsing_struct.current_block = 0;
	parsing_struct.current_address = 0;

	//Get command line options:
	while ((c = getopt (argc, argv, "-atc:iklmo:p:svy")) != -1) {
		switch (c) {
			case 'a':
				parsing_options.analyze_section = true;
				break;
			case 't':
				parsing_options.annotate_time = true;
				break;
			case 'c':
				strcpy(parsing_options.proc_type, optarg);
				break;
			case 'i':
				parsing_options.instruction_cache = true;
				break;
			case 'k':
				parsing_options.c_keywords = true;
				break;
			case 'l':
				parsing_options.echo_line_number = true;
				break;
			case 'm':
				parsing_options.show_marked = true;
				break;
			case 'o':
				strcpy(parsing_options.output_file, optarg);
				parsing_options.provide_out_file = true;
				break;
			case 's':
				parsing_options.parse_syscalls = true;
				break;
			case 'v':
				parsing_options.verbose_mode = true;
				break;
			case 'y':
				PARSE_DOT yydebug = true;
				break;
			case 1:
				strcpy(parsing_options.input_file, optarg);
				valid_file = true;
				break;
			case '?':
				if (isprint(optopt)) {
					cerr << "#ERROR: Unknown option -" << (char)optopt << endl;
				}
				else {
					fprintf(stderr, "#ERROR: Unknown option character '\\x%x'.\n", optopt);
				}
				exit(-1);
				break;
			default:
				cerr << "#ERROR: Option -" << (char)c << " not registered" << endl;
				exit(-1);
		}
	} //end while

	if (valid_file == false) {
		cerr << "#ERROR: no input file" << endl;
		return -1;
	}
	
	if (parsing_options.provide_out_file == false) {
		//output to stdout:
		strcpy(parsing_options.output_file, "/dev/stdout");
	}
	
	//Open input and output files:
	parsing_options.fd_in = fopen(parsing_options.input_file, "r");
	if (parsing_options.fd_in == 0) {
		cerr << "#ERROR: could not open " << parsing_options.input_file << endl;
		return -1;
	}
	parsing_options.fd_out = fopen(parsing_options.output_file, "w"); // "w+"??
	if (parsing_options.fd_out == 0) {
		cerr << "#ERROR: could not open " << parsing_options.output_file << endl;
		return -1;
	}
	
	if (parsing_options.verbose_mode == true) {
		cout << " ************************************************ " << endl;
		cout << " * Parsing options: " << endl;
		cout << " *     Source file: " << parsing_options.input_file << endl;
		cout << " *     Current pass: " << parsing_options.current_pass << endl;
		cout << " *     Processor: " << parsing_options.proc_type << endl;
		cout << " ************************************************ " << endl;
	}
	
	angleToken = new CxxToken('<', no_type, NULL, -1);
	colonToken = new CxxToken(':', no_type, NULL, -1);
	hashToken = new CxxToken('#', no_type, NULL, -1);
	plusToken = new CxxToken('+', no_type, NULL, -1);
	minusToken = new CxxToken('-', no_type, NULL, -1);

	// Load the processor parameters:
	load_processor(parsing_options.proc_type);

	//Read the syscalls to change from file:
	read_syscalls_file();
	
	buffer = (char *)malloc(1000000);

	//Parse file
	yyset_out(parsing_options.fd_out);
	yyset_in(parsing_options.fd_in);
	if (PARSE_DOT yyparse() != 0) {
//		ERRMSG("#ERROR: Failed to parse to end of file\n");
		cerr<<"#WARNING: Failed to parse to end of file"<<endl;
	}

	//Write the remaining data of buffer:
	if (strlen(buffer) > 0) {
		fprintf(parsing_options.fd_out, "%s", buffer);
	}
	fprintf(parsing_options.fd_out, "\n");

	// End parsing
	yylex_destroy();
	
	if (parsing_struct.current_block != 0) {
		cerr << "#WARNING: index of nested blocks has finished in value " << parsing_struct.current_block << endl;
	}
	
	//Create the header file for void uc_for_X(void) declarations
	if (parsing_options.current_pass == FIRST_PASS) {
		uc_for_header = fopen("uc_for_header.h", "w");
		if (uc_for_header == 0) {
			cerr << "#ERROR: could not open uc_for_header.h file for write" << endl;
			return -1;
		}
		for (int i=0 ; i < parsing_struct.for_id ; i++) {
			fprintf(uc_for_header, "void for_mark_%d_(void);\n", i);
		}
		fclose(uc_for_header);
	}

	if (parsing_options.verbose_mode == true) {
		cout << " * Parsing finished..." << endl;
		cout << " *    Error count = " << error_count << endl;
		cout << " *    Warning count = " << warning_count << endl;
		cout << " *    Lines analysed = " << line_number << endl;
		cout << " *    Blocks analysed = " << parsing_struct.block_id << endl;
		cout << " ************************************************ " << endl;
	}
	
	delete angleToken;
	delete colonToken;
	delete hashToken;
	delete plusToken;
	delete minusToken;
	free(parsing_options.proc_type);
	free(parsing_options.input_file);
	free(parsing_options.output_file);
	free(parsing_options.compiler_path);
	free(buffer);
	
	return error_count;
}


void PARSE_SCOPE yyerror(const char *s) {
	if (!bang_depth && (tokenMarkDepth == 0)) {
		cout << s << endl;
		increment_error_count();
	}
	else {
		if (parsing_options.show_marked) {
			cout << "Marked " << s << endl;
		}
		marked_error_count++;
	}
}


/*!
 * \fn get_filename(char *arg)
 * \brief Forms the file name with full path
 * This function takes the file name passed from command line and forms the
 * full path, without dots.
 * \param arg The command line file name
 */
void get_filename(char *arg) {
	char *dot, *parent_dir;
	char *file;
	char aux;
	
	if (*arg == '/') {
		//Source file is provided with full path:
		strcpy(parsing_options.input_file, arg);
	}
	else {
		//Source file name is relative path to PDW:
		strcpy(parsing_options.input_file, getenv("PWD"));
		strcat(parsing_options.input_file, "/");
		strcat(parsing_options.input_file, arg);
	}
	//Remove "./" dirs within path:
	while ((dot = strstr(parsing_options.input_file, "/./")) != NULL) {
		strcpy(dot+1, dot+3);
	}
	//Remove "../" dirs within path:
	while ((dot = strstr(parsing_options.input_file, "../")) != NULL) {
		aux = *(dot-1);
		*(dot-1) = '\0';
		parent_dir = strrchr(parsing_options.input_file, '/');
		*(dot-1) = aux;
		strcpy(parent_dir, dot+2);
	}
	
	//Form the name of output file: "prsd_<input_file>"
	file = strrchr(parsing_options.input_file, '/');
	if (file == NULL) {
		//file has been introduced without path
		strcpy(parsing_options.output_file, "prsd_");
		strcat(parsing_options.output_file, parsing_options.input_file);
	}
	else {
		//file has been introduced with path
		size_t path_size = (size_t)(file - parsing_options.input_file);
		strncpy(parsing_options.output_file, parsing_options.input_file, path_size);
		parsing_options.output_file[path_size] = '\0';
		strcat(parsing_options.output_file, "/prsd_");
		strcat(parsing_options.output_file, file+1);
	}
}


/*!
 * \fn read_syscalls_file(void)
 * \brief Reads the system calls that must be overloaded
 * This function reads from a text file the names of the syscalls that
 * must be overloaded by adding the prefix "uc_...()"
 */
void read_syscalls_file(void) {
	FILE *fd;
	char *syscalls_file, *syscall, *uc_syscall;
	syscalls_file = (char *)malloc(256);
	strcpy(syscalls_file, parsing_options.compiler_path);
	strcat(syscalls_file, "/config/syscalls.cfg");
	fd = fopen(syscalls_file, "r");
	if (fd == NULL) {
		fprintf(stderr, "Syscalls file could not be opened\n");
		return;
	}
	while (feof(fd) == 0) {
		syscall = (char *)malloc(128);
		uc_syscall = (char *)malloc(128);
		fgets(syscall, 124, fd);
		syscall[strlen(syscall)-1] = '\0';
		if (syscall[0] == '\n' || syscall[0] == '\0' || syscall[0] == '#') { continue; }
		strcpy(uc_syscall, "uc_");
		strcat(uc_syscall, syscall);
		syscalls.push_back(syscall);
		uc_syscalls.push_back(uc_syscall);
	}
	fclose(fd);
	free(syscalls_file);
}


/*!
 * \fn yylex()
 * \brief Get next token
 * Get the next token for the parser, invoking yylex_token() to get the next token
 * from the lexer. This routine is renamed to buffered_yylex() by a #define when using
 * yacc so that the two purposes above are split allowing lookahead buffering and
 * primimimg to occur.
 *
 * \return token id
 */
int PARSE_SCOPE yylex() {
	CxxToken *aToken = primed_tokens[0];
	if (aToken) {
		//Return the first token of the buffer
		primed_tokens[0] = primed_tokens[1];
		primed_tokens[1] = primed_tokens[2];
		primed_tokens[2] = 0;
	}
	else if (tokenReadIndex < tokenWriteIndex) {
		aToken = tokenBuffer[tokenReadIndex++];
	}
	else {
		//yylex_token() is implemented in CxxLexing.cxx
		aToken = yylex_token();
		if (aToken == NULL) {
			return 0;
		}
		if (tokenMarkDepth > 0) {
			token_put(aToken);
		}
		else {
			tokenWriteIndex = 0;
			tokenReadIndex = 0;
		}
	}
	yylval.token = aToken;
	return aToken->value();
}


/*!
 * \fn start_search(bool enableType1)
 * \brief Start a new binary search over the template/arithmetic alternative parses of a statement.
 * Marks the current position and saves it in a binary search context maintained on a private stack.
 * \param enableType1
 */
void start_search(bool enableType1) {
	//Backup current parameters:
	searching_time[nested_search_ind] = parsing_struct.segment_time;
	searching_instr[nested_search_ind] = parsing_struct.segment_instr;
	searching_nested_loops[nested_search_ind] = nested_for_ind;
	nested_search_ind++;
	bool type1Enabled = !CxxSearchContext::current() || CxxSearchContext::current()->enable_type1() ? true : false;
	CxxSearchContext::start(mark(), enableType1 && type1Enabled ? true : false);
}


/*!
 * \fn advance_search()
 * \brief Advance the binary search of template attempts.
 * Rewinds and forces true into the input sequence to proceed with the search. Rewinds and forces
 * false to terminate it. Also forces a # that may then be used to initiate error propagation.
 */
void advance_search() {
	//Restore last parameters:
	parsing_struct.segment_time = searching_time[nested_search_ind - 1];
	parsing_struct.segment_instr = searching_instr[nested_search_ind - 1];
	nested_for_ind = searching_nested_loops[nested_search_ind - 1];
	CxxSearchContext::search_advances++;
	remark(CxxSearchContext::current()->mark());
	if (CxxSearchContext::current() && CxxSearchContext::current()->advance()) {
		primed_tokens[0] = plusToken;
		primed_tokens[1] = 0;
	}
	else {
		primed_tokens[0] = minusToken;
		primed_tokens[1] = hashToken;
	}
}


/*!
 * \fn end_search(CxxToken *aToken)
 * \brief Complete a search, releasing the search context object and popping a mark off the stack.
 *
 * \param aToken
 */
void end_search(CxxToken *aToken) {
	nested_search_ind--;
	CxxSearchContext::release();
	unmark(aToken);
}


/*!
 * \fn increment_error_count()
 * \brief Notch up an error and establish a good break point.
 */
void increment_error_count() {
	error_count++;
}


/*!
 * \fn increment_warning_count()
 * \brief Notch up a warning.
 */
void increment_warning_count() {
	warning_count++;
}


/*!
 * \fn mark()
 * \brief Push a new marked context onto the stack, returning its identity for use by remark().
 * Any parser readahead is incorporated within the marked region.
 */
YACC_MARK_TYPE mark() {
	if (primed_tokens[0]) { ERRMSG("Unexpected primed_tokens[0] in mark."); }
	YACC_MARK_TYPE markIndex = tokenReadIndex;
	if (PARSE_DOT yychar != YYEMPTY) {
//		if (primed_tokens[2])
//			token_put(primed_tokens[2]);
//		if (primed_tokens[1])
//			token_put(primed_tokens[1]);
//		if (primed_tokens[0])
//			token_put(primed_tokens[0]);
//		if (!tokenMarkDepth)
		if (!tokenReadIndex && !tokenWriteIndex) {
			token_put(PARSE_DOT yylval.token);
			tokenReadIndex = 0;
		}
		else if (!tokenReadIndex) {
			ERRMSG("Unexpected 0 read index in mark.");
		}
		else if (tokenBuffer[--tokenReadIndex] != PARSE_DOT yylval.token) {
			ERRMSG("Unexpected unget in mark.");
		}
		markIndex = tokenReadIndex;
		yyclearin;
		primed_tokens[0] = 0;
		primed_tokens[1] = 0;
	}
	tokenMarkDepth++;
	bang_depth++;
	return markIndex;
}


//	Reposition the input to restart at the position returned by a mark().
void remark(YACC_MARK_TYPE anIndex) {
	tokenReadIndex = anIndex;
	yyclearin; //Remove remaining tokens
}


/*!
 * \fn unmark(const CxxToken *aToken)
 * \brief Pop a marked context off the stack.
 */
void unmark(const CxxToken *aToken) {
	if (bang_depth) { bang_depth--; }
	else { ERRMSG("BUG - should not unmark with 0 bang."); }
	if (tokenMarkDepth <= 0) { ERRMSG("Unexpected unmark."); }
	else { tokenMarkDepth--; }
}


//	If it is appropriate to do type I function parameter parsing perform a mark and force a rrue token
//	into the input stream. Otherwise just force a false token in.
YACC_MARK_TYPE mark_type1() {
	if (!in_type1 && CxxSearchContext::current() && CxxSearchContext::current()->enable_type1()) {
		YACC_MARK_TYPE markIndex = mark();
		primed_tokens[0] = plusToken;
		primed_tokens[1] = 0;
		in_type1 = true;
        yyclearin;
		return markIndex;
	}
	else {
		primed_tokens[0] = minusToken;
		primed_tokens[1] = PARSE_DOT yychar != YYEMPTY ? PARSE_DOT yylval.token : 0;
        yyclearin;
		return 0;			// Never used.
	}
}


//	Push a bang context onto the error suppression stack, returning the context for restoration by pop_bang.
void pop_bang(YACC_BANG_TYPE bangValue) {
	bang_depth = bangValue;
}


//	Push a bang context onto the error suppression stack, returning the context for restoration by pop_bang.
YACC_BANG_TYPE push_bang() {
	return bang_depth++;
}


//	Reposition the input to restart at the position returned by a mark().
void remark_type1(YACC_MARK_TYPE anIndex) {
	remark(anIndex);
	in_type1 = false;
}


//	Rewind the input stream back to anIndex and force a : prior to resuming input.
void rewind_colon(YACC_MARK_TYPE anIndex, const CxxToken *aToken) {
	remark(anIndex);
	unmark();
	primed_tokens[0] = colonToken;
	primed_tokens[1] = PARSE_DOT yylval.token;
}


/*!
 * \fn template_test()
 *
 * Determine whether the just parsed '<' should be interpreted as a template or arithmetic operator.
 * The implementation here intersacts with a binary search to traverse all possibilities in
 * multiple passes. The search proceeds by branch and bound presuming the template interpretation.
 * A true token is forced into the input stream to take the template interpretaion. A false token
 * otherwise.
 * An alternate implementation that keeps track of scopes may interact with semantic knowledge to make
 * the correct decision directly.
 */
void template_test() {
	if (!CxxSearchContext::current() || CxxSearchContext::current()->is_template()) {
		primed_tokens[0] = plusToken;
		primed_tokens[1] = 0;
	}
	else {
		primed_tokens[0] = minusToken;
		primed_tokens[1] = angleToken;
	}
}


/*!
 * \fn token_put(CxxToken *aToken)
 * \brief Inserts token pointer into the tokenBuffer
 *
 * \param aToken token to insert
 */
void token_put(CxxToken *aToken) {
	if (tokenBuffer == NULL || tokenSize == 0) {
		//Creates buffer:
		tokenBuffer = new CxxToken *[tokenSize = 256];
	}
	else if (tokenWriteIndex >= tokenSize) {
		//Creates a new buffer twice size than older:
		CxxToken **oldTokenBuffer = tokenBuffer;
		size_t oldTokenSize = tokenSize;
		tokenBuffer = new CxxToken *[tokenSize *= 2];
		memcpy(tokenBuffer, oldTokenBuffer, oldTokenSize * sizeof(*oldTokenBuffer));
		delete[] oldTokenBuffer;
	}
	//Inserts the token:
	tokenBuffer[tokenWriteIndex++] = aToken;
	tokenReadIndex = tokenWriteIndex;
}

