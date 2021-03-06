// JSMNDemo.cpp : Defines the entry point for the console application.
//

#include "../../../jsmn_ext.h"
#include <string>
#include <iostream>
int main()
{
   const char* html = "{ \"url\": \"https://test.com/item_bin.bin\", \"home\": [ 1, \"Test\", { \"arrKey1\": \"arrVal1\" }] }";
   jsmn_parser parser;
   jsmntok_t tokens[50];
   jsmn_init( &parser );
   auto num_toks = jsmn_parse( &parser, html, strlen( html ), tokens, 50 );
   if( num_toks > 0 )
   {
      jsmntok_ext_t payload;
      jsmn_ext_init( html, tokens, num_toks, 0, &payload );

      if( jsmn_ext_read( &payload, "home", &payload ) == 0 )
      {
         if( jsmn_ext_array_get( &payload, 2, &payload ) == 0 )
         {
            if( jsmn_ext_read( &payload, "arrKey1", &payload ) == 0 )
            {
               std::cout << std::string( payload.js, payload.root_token->start,
                                         payload.root_token->end-payload.root_token->start );
            }
         }
      }
   }

   std::cin.get();
   return 0;
}

