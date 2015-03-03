#ifndef VIENNA_RNA_PACKAGE_UTILS_H
#define VIENNA_RNA_PACKAGE_UTILS_H

/* make this interface backward compatible with RNAlib < 2.2.0 */
#define VRNA_BACKWARD_COMPAT

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#else
#define DEPRECATED(func) func
#endif

/**
 *  @file utils.h
 *  @brief Various utility- and helper-functions used throughout the Vienna RNA package
 */

#include <stdio.h>

#include <ViennaRNA/data_structures.h>
#include <ViennaRNA/structure_utils.h>

/* two helper macros to indicate whether a function should be exported in
the library or stays hidden */
#define PUBLIC
#define PRIVATE static

/**
 *  @brief Output flag of get_input_line():  @e "An ERROR has occured, maybe EOF"
 */
#define VRNA_INPUT_ERROR                  1U
/**
 *  @brief @brief Output flag of get_input_line():  @e "the user requested quitting the program"
 */
#define VRNA_INPUT_QUIT                   2U
/**
 *  @brief Output flag of get_input_line():  @e "something was read"
 */
#define VRNA_INPUT_MISC                   4U

/**
 *  @brief  Input/Output flag of get_input_line():\n
 *  if used as input option this tells get_input_line() that the data to be read should comply
 *  with the FASTA format
 * 
 *  the function will return this flag if a fasta header was read
 */
#define VRNA_INPUT_FASTA_HEADER           8U

/*
 *  @brief  Input flag for get_input_line():\n
 *  Tell get_input_line() that we assume to read a nucleotide sequence
 * 
 */
#define VRNA_INPUT_SEQUENCE               16U

/** @brief  Input flag for get_input_line():\n
 *  Tell get_input_line() that we assume to read a structure constraint
 * 
 */
#define VRNA_INPUT_CONSTRAINT             32U

/**
 *  @brief  Input switch for get_input_line():
 *  @e "do not trunkate the line by eliminating white spaces at end of line"
 */
#define VRNA_INPUT_NO_TRUNCATION          256U

/**
 *  @brief  Input switch for vrna_read_fasta_record():  @e "do fill rest array"
 */
#define VRNA_INPUT_NO_REST                512U

/**
 *  @brief  Input switch for vrna_read_fasta_record():  @e "never allow data to span more than one line"
 */
#define VRNA_INPUT_NO_SPAN                1024U

/**
 *  @brief  Input switch for vrna_read_fasta_record():  @e "do not skip empty lines"
 */
#define VRNA_INPUT_NOSKIP_BLANK_LINES     2048U

/**
 *  @brief  Output flag for vrna_read_fasta_record():  @e "read an empty line"
 */
#define VRNA_INPUT_BLANK_LINE             4096U

/**
 *  @briefInput switch for get_input_line():  @e "do not skip comment lines"
 */
#define VRNA_INPUT_NOSKIP_COMMENTS        128U

/**
 *  @brief  Output flag for vrna_read_fasta_record():  @e "read a comment"
 */
#define VRNA_INPUT_COMMENT                8192U

/**
 *  @brief  Tell a function that an input is assumed to span several lines
 *
 *  If used as input-option a function might also be returning this state telling
 *  that it has read data from multiple lines.
 *
 *  @see vrna_extract_record_rest_structure(), vrna_read_fasta_record(), vrna_extract_record_rest_constraint()
 *
 */
#define VRNA_OPTION_MULTILINE             32U


/**
 *  @brief Get the minimum of two comparable values
 */
#define MIN2(A, B)      ((A) < (B) ? (A) : (B))

/**
 *  @brief Get the maximum of two comparable values
 */
#define MAX2(A, B)      ((A) > (B) ? (A) : (B))

/**
 *  @brief Get the minimum of three comparable values
 */
#define MIN3(A, B, C)   (MIN2(  (MIN2((A),(B))) ,(C)))

/**
 *  @brief Get the maximum of three comparable values
 */
#define MAX3(A, B, C)   (MAX2(  (MAX2((A),(B))) ,(C)))


/**
 * @brief Stringify a macro after expansion
 */
#define XSTR(s) STR(s)

/**
 * @brief Stringify a macro argument
 */
#define STR(s) #s

#ifndef FILENAME_MAX_LENGTH

/**
 *  @brief Maximum length of filenames that are generated by our programs
 *
 *  This definition should be used throughout the complete ViennaRNA package
 *  wherever a static array holding filenames of output files is declared.
 */
#define FILENAME_MAX_LENGTH   80

/**
 *  @brief Maximum length of id taken from fasta header for filename generation
 *
 *  this has to be smaller than FILENAME_MAX_LENGTH since in most cases,
 *  some suffix will be appended to the ID
 */
#define FILENAME_ID_LENGTH    42

#endif


#ifdef HAVE_CONFIG_H
#include <config.h>
#ifndef HAVE_STRDUP
char *strdup(const char *s);
#endif
#endif
#ifdef WITH_DMALLOC
/* use dmalloc library to check for memory management bugs */
#include "dmalloc.h"
#define space(S) calloc(1,(S))
#else

/**
 *  @brief Allocate space safely
 *
 *  @param size The size of the memory to be allocated in bytes
 *  @return     A pointer to the allocated memory
 */
/*@only@*/ /*@notnull@*/
void  *space(unsigned size) /*@ensures MaxSet(result) == (size-1);@*/;

/**
 *  @brief Reallocate space safely
 *
 *  @param p    A pointer to the memory region to be reallocated
 *  @param size The size of the memory to be allocated in bytes
 *  @return     A pointer to the newly allocated memory
 */
/*@only@*/ /*@notnull@*/
void  *xrealloc(/*@null@*/ /*@only@*/ /*@out@*/ /*@returned@*/ void *p,
                unsigned size) /*@modifies *p @*/ /*@ensures MaxSet(result) == (size-1) @*/;
#endif

/**
 *  @brief Die with an error message
 *
 *  @see warn_user()
 *  @param message The error message to be printed before exiting with 'FAILURE'
 */
/*@exits@*/
void nrerror(const char message[]);

/**
 *  @brief Print a warning message
 *
 *  Print a warning message to @e stderr
 *
 *  @param  message   The warning message
 */
void warn_user(const char message[]);

/**
 *  @brief  Make random number seeds
 */
void   init_rand(void);

/**
 * @brief Current 48 bit random number
 *
 *  This variable is used by urn(). These should be set to some
 *  random number seeds before the first call to urn().
 *
 *  @see urn()
 */
extern unsigned short xsubi[3];

/**
 *  @brief get a random number from [0..1]
 *
 *  @note Usually implemented by calling @e erand48().
 *  @return   A random number in range [0..1]
 */
double urn(void);

/**
 *  @brief Generates a pseudo random integer in a specified range
 *
 *  @param from   The first number in range
 *  @param to     The last number in range
 *  @return       A pseudo random number in range [from, to]
 */
int    int_urn(int from, int to);

void   filecopy(FILE *from, FILE *to); /* inefficient `cp' */

/**
 *  @brief Get a timestamp
 *
 *  Returns a string containing the current date in the format
 *  @verbatim Fri Mar 19 21:10:57 1993 @endverbatim
 *
 *  @return A string containing the timestamp
 */
char  *time_stamp(void);

/**
 *  @brief Create a random string using characters from a specified symbol set
 *
 *  @param l        The length of the sequence
 *  @param symbols  The symbol set
 *  @return         A random string of length 'l' containing characters from the symbolset
 */
/*@only@*/ /*@notnull@*/
char  *random_string(int l, const char symbols[]);

/**
 *  @brief Calculate hamming distance between two sequences
 *
 *  Calculate the number of positions in which 
 *  @param s1   The first sequence
 *  @param s2   The second sequence
 *  @return     The hamming distance between s1 and s2
 */
int   hamming(const char *s1, const char *s2);

/**
 *  @brief Calculate hamming distance between two sequences up to a specified length
 *
 *  This function is similar to hamming() but instead of comparing both sequences
 *  up to their actual length only the first 'n' characters are taken into account
 *  @param s1   The first sequence
 *  @param s2   The second sequence
 *  @return     The hamming distance between s1 and s2
 */
int   hamming_bound(const char *s1, const char *s2, int n);

/**
 *  @brief Read a line of arbitrary length from a stream
 *
 *  Returns a pointer to the resulting string. The necessary memory is
 *  allocated and should be released using @e free() when the string is
 *  no longer needed.
 *
 *  @param  fp  A file pointer to the stream where the function should read from
 *  @return     A pointer to the resulting string
 */
/*@only@*/ /*@null@*/
char  *get_line(FILE *fp);

/**
 *  Retrieve a line from 'stdin' savely while skipping comment characters and
 *  other features
 *  This function returns the type of input it has read if recognized.
 *  An option argument allows to switch between different reading modes.\n
 *  Currently available options are:\n
 *  #VRNA_INPUT_NOPRINT_COMMENTS, #VRNA_INPUT_NOSKIP_COMMENTS, #VRNA_INPUT_NOELIM_WS_SUFFIX
 * 
 *  pass a collection of options as one value like this:
 *  @verbatim get_input_line(string, option_1 | option_2 | option_n) @endverbatim
 * 
 *  If the function recognizes the type of input, it will report it in the return
 *  value. It also reports if a user defined 'quit' command (@-sign on 'stdin')
 *  was given. Possible return values are:\n
 *  #VRNA_INPUT_FASTA_HEADER, #VRNA_INPUT_ERROR, #VRNA_INPUT_MISC, #VRNA_INPUT_QUIT
 * 
 *  @param string   A pointer to the character array that contains the line read
 *  @param options  A collection of options for switching the functions behavior
 *  @return         A flag with information about what has been read
 */
unsigned int get_input_line(char **string,
                            unsigned int options);



/**
 *  @brief Print a line to @e stdout that asks for an input sequence
 *
 *  There will also be a ruler (scale line) printed that helps orientation of the sequence positions
 */
void print_tty_input_seq(void);

/**
 *  @brief Print a line with a user defined string and a ruler to stdout.
 *
 *  (usually this is used to ask for user input)
 *  There will also be a ruler (scale line) printed that helps orientation of the sequence positions
 * 
 *  @param s A user defined string that will be printed to stdout
 */
void print_tty_input_seq_str(const char *s);

/**
 *  @brief Convert an input sequence (possibly containing DNA alphabet characters) to RNA alphabet
 *
 *  This function substitudes <i>T</i> and <i>t</i> with <i>U</i> and <i>u</i>, respectively
 * 
 *  @param sequence The sequence to be converted
 */
void vrna_seq_toRNA(char *sequence);

/**
 *  @brief Convert an input sequence to uppercase
 * 
 *  @param sequence The sequence to be converted
 */
void vrna_seq_toupper(char *sequence);


/**
 *  @brief Get an index mapper array (iindx) for accessing the energy matrices, e.g. in partition function related functions.
 *
 *  Access of a position "(i,j)" is then accomplished by using @verbatim (i,j) ~ iindx[i]-j @endverbatim
 *  This function is necessary as most of the two-dimensional energy matrices are actually one-dimensional arrays throughout
 *  the ViennaRNA Package
 * 
 *  Consult the implemented code to find out about the mapping formula ;)
 * 
 *  @see vrna_get_indx()
 *  @param length The length of the RNA sequence
 *  @return       The mapper array
 */
int   *vrna_get_iindx(unsigned int length);

/**
 *  @brief Get an index mapper array (indx) for accessing the energy matrices, e.g. in MFE related functions.
 *
 *  Access of a position "(i,j)" is then accomplished by using @verbatim (i,j) ~ indx[j]+i @endverbatim
 *  This function is necessary as most of the two-dimensional energy matrices are actually one-dimensional arrays throughout
 *  the ViennaRNAPackage
 * 
 *  Consult the implemented code to find out about the mapping formula ;)
 * 
 *  @see vrna_get_iindx()
 *  @param length The length of the RNA sequence
 *  @return       The mapper array
 * 
 */
int   *vrna_get_indx(unsigned int length);

/**
 *  @brief Get a numerical representation of the nucleotide sequence
 *
 */
short *vrna_seq_encode(const char *sequence, vrna_md_t *md);

/**
 *  @brief Get a numerical representation of the nucleotide sequence (simple version)
 *
 */
short *vrna_seq_encode_simple(const char *sequence, vrna_md_t *md);


/**
 *  @brief  Encode a nucleotide character to numerical value
 *
 *  This function encodes a nucleotide character to its numerical representation as required by many functions in RNAlib.
 *
 *  @see  vrna_nucleotide_decode(), vrna_seq_encode()
 *
 *  @param  c   The nucleotide character to encode
 *  @param  md  The model details that determine the kind of encoding
 *  @return     The encoded nucleotide
 */
int   vrna_nucleotide_encode(char c, vrna_md_t *md);

/**
 *  @brief  Decode a numerical representation of a nucleotide back into nucleotide alphabet
 *
 *  This function decodes a numerical representation of a nucleotide character back into nucleotide alphabet
 *
 *  @see  vrna_nucleotide_encode(), vrna_seq_encode()
 *
 *  @param  enc The encoded nucleotide
 *  @param  md  The model details that determine the kind of decoding
 *  @return     The decoded nucleotide character
 */
char  vrna_nucleotide_decode(int enc, vrna_md_t *md);

void  vrna_ali_encode(const char *sequence,
                      short **S_p,
                      short **s5_p,
                      short **s3_p,
                      char **ss_p,
                      unsigned short **as_p,
                      vrna_md_t *md);

/**
 *  @brief Get an array of the numerical encoding for each possible base pair (i,j)
 *
 *  @note This array is always indexed via jindx, in contrast to previously
 *  different indexing between mfe and pf variants!
 *  @see  vrna_get_indx(), #vrna_fold_compound
 *
 */
char  *vrna_get_ptypes( const short *S,
                        vrna_md_t *md);

#ifdef  VRNA_BACKWARD_COMPAT

DEPRECATED(char  *get_ptypes(const short *S,
                  vrna_md_t *md,
                  unsigned int idx_type));

DEPRECATED(int   *get_indx(unsigned int length));

DEPRECATED(int   *get_iindx(unsigned int length));

/**
 *  @brief Convert an input sequence to uppercase
 *  @deprecated   Use vrna_seq_toupper() instead!
 *  @param sequence The sequence to be converted
 */
DEPRECATED(void  str_uppercase(char *sequence));

/**
 *  @brief Convert a DNA input sequence to RNA alphabet
 *
 *  @deprecated Use vrna_seq_toRNA() instead!
 * 
 *  @param sequence The sequence to be converted
 */
DEPRECATED(void str_DNA2RNA(char *sequence));

#endif

#endif
