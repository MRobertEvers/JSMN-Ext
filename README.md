 JSMN Ext
 ===========
 JSMN Ext offers 3 higher level functions on top of zserge's [JSMN library](https://github.com/zserge/jsmn).  
 
 See jsmn_ext.h for function details.
 
 Overview
 -------- 
 There are a few pieces of overhead in the jsmn library required to understand the tokens produced by the jsmn parser.
 
 That information is the JSON string, the tokens produced by the jmsn parser, and the number of tokens.  The jsmntok_ext_t struct
 wraps that information with a 'Root' node.  

    typedef struct
    {
       jsmntok_t* root_token;     // JSMN token wrapped with the data necessary to understand it
       unsigned short num_root;   // Location of the root_token in the 'tokens' array
       jsmntok_t* tokens;         // Array of tokens that was parsed from js
       unsigned short num_tokens; // Number of tokens in the array
       char* js;                  // JSON String that produced the tokens array
    } jsmntok_ext_t;  

 A root node is either a JSON structure (JSON object or JSON Array) or a JSON value token. 
 
 When the root node is a JSON structure, the jsmn_ext functions may be used to extract nested values/structures from that node. i.e. if the token is a JSON object...  
 
    { // <--Root token starts here. (including '{')
        "key": "value",
        "key2": "{
                     "subkey": ["val1", "val2", ... ],
                     "anotherkey": "val"
                 }"
        "key3": [ 1 ]
    } // <--Root token ends here.  (including '}')
    
 Or a JSON array 
 
     [ // <--Root token starts here.(including '[')
         "ind0",
         "ind1",
         "ind2"
     ] // <--Root token ends here. (including ']')
 
 When the root token is a JSON Object, use `jsmn_ext_read` to return the token representing a value at a key. e.g. If we want the token represent the value of key "key3" in the JSON object above, use
 
	if( jsmn_ext_read( &input_token, "key3", &output_token ) == 0 )
	{
	   // Do something with the output_token.
	   // output_token represents "[ 1 ]"
	}
 
 Note that the input token and the output token can be the same. Also, notice the output token is an array! That means you can read the array using `jsmn_ext_array_get`
 
 When the root token is a JSON Object, use `jsmn_ext_array_get` to return the token representing a value at an index in the array. e.g. If we want the token represent the value of index 0 in the JSON object above, use
 
	if( jsmn_ext_array_get( &input_token, 0, &output_token ) == 0 )
	{
	   // Do something with the output_token.
	   // output_token represents "ind0"
	}
 
 
 As with many high level JSON libraries, reading the value of a JSON Object's key returns the value at the key in the JSON object. Likewise reading a JSON Array's
 value at a specific index returns the value at that index.
 
 When the root token of a jsmntok_ext token is neither a JSMN_ARRAY nor JSMN_OBJECT, then neither of the above functions will do anything... they will return < 0.

 JSMN Notes
 -------------
 JSMN tokens represent a span of the parsed input string. i.e. in the JSON Object `{ "Key1": "val1" }`, a token represents all 19 characters of the JSON object. That token will be a JSMN_Object type and its tok->start=0 and tok->end=19. i.e. The token just indicates the starting character of the token,
 and the ending character of the token in the input JSON Character array(string). Furthermore, another token would represent `"Key1"` and it would be a JSMN_String, start=3, end=7. (Note that on JSMN_Strings, the span does not include the quotes)