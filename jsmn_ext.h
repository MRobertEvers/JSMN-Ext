#pragma once

// Requires "parent links" within jsmn.
#include "jsmn.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* JSON ext token description.
* root_token   jsmntok_t That this token wraps
* num_root     The index of 'root_token' in the tokens array.
* tokens       Array of tokens that root_token belongs to and that was parsed from jsmn_parse =
* num_tokens   Size of the array of tokens
* js           Const JSON string that the array of tokens was parsed from.
*/
typedef struct
{
   jsmntok_t* root_token;
   unsigned short num_root;
   jsmntok_t* tokens;
   unsigned short num_tokens;
   char* js;
} jsmntok_ext_t;

/**
* @brief Compares the string value of the input token to the input string value.
*
* @param apJS      JSON string
* @param apToken   jsmn Token - Represents a span in the json string.
* @param apCompare String to compare the token's span to.
* @return 0 if matching string; -1 otherwise.
*/
int jsoneq( const char* apJS, jsmntok_t* apToken, const char *apCompare );

/**
* @brief Creates an 'extended' jsmn token.
*
* Wraps a jsmn token with the extra information required to read/parse a JSON token.
* See extjsmntok_t for structure.
*
* @param apJS         Source JSON string. 
* @param apTokens     Array containing all the tokens parsed from the apJS string.
* @param auiNumTokens Total number of tokens in tokens array.
* @param auiRoot      Index of the root token in the token array. Ideally JSMN_OBJECT or _ARRAY
* @param rpNode [out] Node to return
* @return 0
*/
int jsmn_ext_init( const char* apJS, jsmntok_t* apTokens, unsigned short auiNumTokens, 
                   unsigned short auiRoot, jsmntok_ext_t* rpNode );

/**
* @brief Return the ext jsmn token at the specified key if it exists in a JSON object.
* 
* @param apRoot ext jsmn token representing a JSMN_OBJECT. (Must be JSMN_OBJECT)
* @param apKey  Key string to search for in the JSON object
* @param rpNode [out] Node at key if it exists
* @return 0 if key found; -1 otherwise.
*/
int jsmn_ext_read( jsmntok_ext_t* apRoot, const char* apKey, jsmntok_ext_t* rpNode);

/**
* @brief Return the ext jsmn token at the specified index in a JSON array.
* 
* @param apRoot    ext jsmn token representing a JSMN_OBJECT. (Must be JSMN_OBJECT)
* @param auiIndex  Index to find in JSON array   
* @param rpNode [out] Node at key if it exists
* @return 0 if index found; -1 otherwise.
*/
int jsmn_ext_array_get( jsmntok_ext_t* apRoot, unsigned short auiIndex , jsmntok_ext_t* rpNode );

#ifdef __cplusplus
}
#endif