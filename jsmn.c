#include "jsmn.h"


/**
 * Allocates a fresh unused token from the token pull.
 */
static jsmntok_t* 
jsmn_alloc_token( jsmn_parser* apParser,
                  jsmntok_t* apTokenArr, 
                  size_t auiMaxTokens )
{
   jsmntok_t *tok;
   if( apParser->toknext >= auiMaxTokens )
   {
      return NULL;
   }
   tok = &apTokenArr[apParser->toknext++];
   tok->start = tok->end = -1;
   tok->size = 0;
   tok->parent = -1;
   return tok;
}


/**
 * Fills token type and boundaries.
 */
static void 
jsmn_fill_token( jsmntok_t* apToken,
                 jsmntype_t aTokenType,
                 int aiStart, int aiEnd )
{
   apToken->type = aTokenType;
   apToken->start = aiStart;
   apToken->end = aiEnd;
   apToken->size = 0;
}


/**
 * Fills next available token with JSON primitive.
 */
static int
jsmn_parse_primitive( jsmn_parser* apParser, const char *apJS,
                      size_t auiJSLength, jsmntok_t* apTokenArr, size_t auiMaxTokens )
{
   jsmntok_t *token;
   int start;

   start = apParser->pos;

   for( ; apParser->pos < auiJSLength && apJS[apParser->pos] != '\0'; apParser->pos++ )
   {
      switch( apJS[apParser->pos] )
      {
#ifndef JSMN_STRICT
         /* In strict mode primitive must be followed by "," or "}" or "]" */
         case ':':
#endif
         case '\t': case '\r': case '\n': case ' ':
         case ',': case ']': case '}':
            goto found;
      }
      if( apJS[apParser->pos] < 32 || apJS[apParser->pos] >= 127 )
      {
         apParser->pos = start;
         return JSMN_ERROR_INVAL;
      }
   }
#ifdef JSMN_STRICT
   /* In strict mode primitive must be followed by a comma/object/array */
   parser->pos = start;
   return JSMN_ERROR_PART;
#endif

found:
   if( apTokenArr == NULL )
   {
      apParser->pos--;
      return 0;
   }
   token = jsmn_alloc_token( apParser, apTokenArr, auiMaxTokens );
   if( token == NULL )
   {
      apParser->pos = start;
      return JSMN_ERROR_NOMEM;
   }
   jsmn_fill_token( token, JSMN_PRIMITIVE, start, apParser->pos );
   token->parent = apParser->toksuper;
   apParser->pos--;
   return 0;
}

/**
 * Fills next token with JSON string.
 */
static int 
jsmn_parse_string( jsmn_parser* aParser, const char *aJS,
                   size_t auiJSLength, jsmntok_t* apTokensArr, size_t aiMaxTokens )
{
   jsmntok_t* token;

   int start = aParser->pos;

   aParser->pos++;

   /* Skip starting quote */
   for( ; aParser->pos < auiJSLength && aJS[aParser->pos] != '\0'; aParser->pos++ )
   {
      char c = aJS[aParser->pos];

      /* Quote: end of string */
      if( c == '\"' )
      {
         if( apTokensArr == NULL )
         {
            return 0;
         }
         token = jsmn_alloc_token( aParser, apTokensArr, aiMaxTokens );
         if( token == NULL )
         {
            aParser->pos = start;
            return JSMN_ERROR_NOMEM;
         }
         jsmn_fill_token( token, JSMN_STRING, start + 1, aParser->pos );
         token->parent = aParser->toksuper;
         return 0;
      }

      /* Backslash: Quoted symbol expected */
      if( c == '\\' && aParser->pos + 1 < auiJSLength )
      {
         int i;
         aParser->pos++;
         switch( aJS[aParser->pos] )
         {
            /* Allowed escaped symbols */
         case '\"': case '/': case '\\': case 'b':
         case 'f': case 'r': case 'n': case 't':
            break;
            /* Allows escaped symbol \uXXXX */
         case 'u':
            aParser->pos++;
            for( i = 0; i < 4 && aParser->pos < auiJSLength && aJS[aParser->pos] != '\0'; i++ )
            {
               /* If it isn't a hex character we have an error */
               if( !((aJS[aParser->pos] >= 48 && aJS[aParser->pos] <= 57) || /* 0-9 */
                  (aJS[aParser->pos] >= 65 && aJS[aParser->pos] <= 70) || /* A-F */
                  (aJS[aParser->pos] >= 97 && aJS[aParser->pos] <= 102)) )
               { /* a-f */
                  aParser->pos = start;
                  return JSMN_ERROR_INVAL;
               }
               aParser->pos++;
            }
            aParser->pos--;
            break;
            /* Unexpected symbol */
         default:
            aParser->pos = start;
            return JSMN_ERROR_INVAL;
         }
      }
   }
   aParser->pos = start;
   return JSMN_ERROR_PART;
}


/**
* Creates a new parser based over a given  buffer with an array of tokens
* available.
*/
void
jsmn_init( jsmn_parser *parser )
{
   parser->pos = 0;
   parser->toknext = 0;
   parser->toksuper = -1;
}


/**
 * Parse JSON string and fill tokens.
 */
int 
jsmn_parse( jsmn_parser* apParser, const char *apJS, size_t auiJSLength,
            jsmntok_t* apTokenArr, unsigned int auiMaxTokens )
{
   int r;
   int i;
   jsmntok_t* token;
   int count = apParser->toknext;

   for( ; apParser->pos < auiJSLength && apJS[apParser->pos] != '\0'; apParser->pos++ )
   {
      char c;
      jsmntype_t type;

      c = apJS[apParser->pos];
      switch( c )
      {
      case '{': case '[':
         count++;
         if( apTokenArr == NULL )
         {
            break;
         }
         token = jsmn_alloc_token( apParser, apTokenArr, auiMaxTokens );
         if( token == NULL )
            return JSMN_ERROR_NOMEM;
         if( apParser->toksuper != -1 )
         {
            apTokenArr[apParser->toksuper].size++;
            token->parent = apParser->toksuper;
         }
         token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
         token->start = apParser->pos;
         apParser->toksuper = apParser->toknext - 1;
         break;
      case '}': case ']':
         if( apTokenArr == NULL )
            break;
         type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);

         if( apParser->toknext < 1 )
         {
            return JSMN_ERROR_INVAL;
         }
         token = &apTokenArr[apParser->toknext - 1];
         for( ;;)
         {
            if( token->start != -1 && token->end == -1 )
            {
               if( token->type != type )
               {
                  return JSMN_ERROR_INVAL;
               }
               token->end = apParser->pos + 1;
               apParser->toksuper = token->parent;
               break;
            }
            if( token->parent == -1 )
            {
               if( token->type != type || apParser->toksuper == -1 )
               {
                  return JSMN_ERROR_INVAL;
               }
               break;
            }
            token = &apTokenArr[token->parent];
         }
         break;
      case '\"':
         r = jsmn_parse_string( apParser, apJS, auiJSLength, apTokenArr, auiMaxTokens );
         if( r < 0 ) return r;
         count++;
         if( apParser->toksuper != -1 && apTokenArr != NULL )
            apTokenArr[apParser->toksuper].size++;
         break;
      case '\t': case '\r': case '\n': case ' ':
         break;
      case ':':
         apParser->toksuper = apParser->toknext - 1;
         break;
      case ',':
         if( apTokenArr != NULL && apParser->toksuper != -1 &&
            apTokenArr[apParser->toksuper].type != JSMN_ARRAY &&
            apTokenArr[apParser->toksuper].type != JSMN_OBJECT )
         {
            apParser->toksuper = apTokenArr[apParser->toksuper].parent;
            for( i = apParser->toknext - 1; i >= 0; i-- )
            {
               if( apTokenArr[i].type == JSMN_ARRAY || apTokenArr[i].type == JSMN_OBJECT )
               {
                  if( apTokenArr[i].start != -1 && apTokenArr[i].end == -1 )
                  {
                     apParser->toksuper = i;
                     break;
                  }
               }
            }
         }
         break;
#ifdef JSMN_STRICT
         /* In strict mode primitives are: numbers and booleans */
      case '-': case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case 't': case 'f': case 'n':
         /* And they must not be keys of the object */
         if( tokens != NULL && parser->toksuper != -1 )
         {
            jsmntok_t *t = &tokens[parser->toksuper];
            if( t->type == JSMN_OBJECT ||
               (t->type == JSMN_STRING && t->size != 0) )
            {
               return JSMN_ERROR_INVAL;
            }
         }
#else
         /* In non-strict mode every unquoted value is a primitive */
      default:
#endif
         r = jsmn_parse_primitive( apParser, apJS, auiJSLength, apTokenArr, auiMaxTokens );
         if( r < 0 ) return r;
         count++;
         if( apParser->toksuper != -1 && apTokenArr != NULL )
            apTokenArr[apParser->toksuper].size++;
         break;

#ifdef JSMN_STRICT
         /* Unexpected char in strict mode */
      default:
         return JSMN_ERROR_INVAL;
#endif
      }
   }

   if( apTokenArr != NULL )
   {
      for( i = apParser->toknext - 1; i >= 0; i-- )
      {
         /* Unmatched opened object or array */
         if( apTokenArr[i].start != -1 && apTokenArr[i].end == -1 )
         {
            return JSMN_ERROR_PART;
         }
      }
   }

   return count;
}


