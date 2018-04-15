 JSMN Ext
 ===========
 JSMN Ext offers 3 higher level functions on top of zserge's [JSMN library](https://github.com/zserge/jsmn).  
 
 See jsmn_ext.h for function details.
 
 Overview
 -------- 
 There are a few pieces of overhead in the jsmn library required to understand the tokens produced by the jsmn parser.
 
 That information is the JSON string, the tokens produced by the jmsn parser, and the number of tokens.  The jsmntok_ext_t struct
 wraps that information with a 'Root' node. A root node is either a JSON structure (JSON object or JSON Array) or a JSON value token. 
 
 When the root node is a JSON structure, the jsmn_ext functions may be used to extract nested values/structures from that node.
 
 As with many high level JSON libraries, reading the value of a JSON Object's key returns the value at the key in the JSON object. Likewise reading a JSON Array's
 value at a specific index returns the value at that index.  
 
 The extension functions added in jsmn_ext return the jsmntok_t corresponding to the value at a JSMN_OBJECT's key. That token is wrapped in the jsmntok_ext_t struct. If the token at the JSON Object's key or JSON Array's index is an JSMN_OBJECT or JSMN_ARRAY, the returned struct can further be queried by the jsmn_ext's functions. 