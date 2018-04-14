#include "jsmn_ext.h"

int 
jsoneq( const char* apJS, jsmntok_t* apToken, const char *apEqualCheck )
{
   if( apToken->type == JSMN_STRING && (int)strlen( apEqualCheck ) == apToken->end - apToken->start &&
      strncmp( apJS + apToken->start, apEqualCheck, apToken->end - apToken->start ) == 0 )
   {
      return 0;
   }
   else
   {
      return -1;
   }
}


int
jsmn_ext_init( const char* apJS, jsmntok_t* apTokens, unsigned short auiNumTokens,
               unsigned short auiRoot, jsmntok_ext_t* rpExtjsmntok_t )
{
   rpExtjsmntok_t->js = apJS;
   rpExtjsmntok_t->num_root = auiRoot;
   rpExtjsmntok_t->num_tokens = auiNumTokens;
   rpExtjsmntok_t->tokens = apTokens;
   rpExtjsmntok_t->root_token = &apTokens[auiRoot];
   return 0;
}

int 
jsmn_ext_read( jsmntok_ext_t* apRoot, const char* apKey, jsmntok_ext_t* rpNode )
{
   // We expect the input node to be a JSON object
   if( apRoot->root_token->type != JSMN_OBJECT )
   {
      return -1;
   }

   int jsmn_child_index = 0;
   for( int i = 1; jsmn_child_index < apRoot->root_token->size && i < apRoot->num_tokens; i++ )
   {
      int iCurrentToken = i + apRoot->num_root;
      if( apRoot->tokens[iCurrentToken].parent == apRoot->num_root )
      {
         if( jsoneq( apRoot->js, &apRoot->tokens[iCurrentToken], apKey ) == 0 )
         {
            // The next token in the token list will be the entry at apKey in the JSON obj.
            int iKeyValue = iCurrentToken+1;
            rpNode->js = apRoot->js;
            rpNode->num_root = iKeyValue;
            rpNode->root_token = &apRoot->tokens[iKeyValue];
            rpNode->num_tokens = apRoot->num_tokens;
            rpNode->tokens = apRoot->tokens;
            return 0;
         }
         else
         {
            jsmn_child_index++;
         }
      }
   }

   return -1;
}

int 
jsmn_ext_array_get( jsmntok_ext_t* apRoot, unsigned short auiIndex, jsmntok_ext_t* rpNode )
{
   // We expect the input node to be a JSON object
   if( apRoot->root_token->type != JSMN_ARRAY )
   {
      return -1;
   }

   int jsmn_arr_index = 0;
   for( int i = 1; jsmn_arr_index < apRoot->root_token->size && i < apRoot->num_tokens; i++ )
   {
      int iCurrentToken = i + apRoot->num_root;
      if( apRoot->tokens[iCurrentToken].parent == apRoot->num_root )
      {
         if( jsmn_arr_index == auiIndex )
         {
            rpNode->js = apRoot->js;
            rpNode->num_root = iCurrentToken;
            rpNode->root_token = &apRoot->tokens[iCurrentToken];
            rpNode->num_tokens = apRoot->num_tokens;
            rpNode->tokens = apRoot->tokens;
            return 0;
         }
         else
         {
            jsmn_arr_index++;
         }
      }
   }

   return -1;
}