/**
 * @file    instr_error.h
 * @brief   error and status defines, structs, constants and macros
 *
 * @{
 */

#ifndef _INSTR_ERROR_H_
#define _INSTR_ERROR_H_


// Principles of error and status return values and codes for embedded and system code:
//    If the only possible errors are programmer errors, use asserts inside the function.
//       - These should be assertions that validate the inputs, and clearly communicates what 
//         the function expects.
//       - Too much error checking can obscure the program logic. Deciding what to do for all 
//         the various error cases can really complicate the design. 
//         Example:  NULL pointers:  
//             Instead of logic to allow a functionX to handle a null pointer
//             just assert to insist that the programmer never pass one
//
//   Design driver or low-level routines to return simple language-natural types, such as
//       - bool (true for no error, false for errors)
//       - int:  0  for  OK, negative for error, positive for informative status
//       - if using bools, name the function so the code documents the intent:
//         Example:  name a driver to initialize the USB so its return value makes
//                   sense, and is self-documenting in an IF-statement:
//                    if( USB_is_initialized() )  then ...
//
//   Design higher-level routings, i.e., methods that _can_ give context to 
//   an error, a return type that is a standardized system-wide type.
// 
//   Here are some characteristcs of a well-defined system-defined data type:
//       - store all possible error-states in one typedef enumeration
//       - agree that all system-level routines will return the type
//       - be clear about what constitutes a driver-return, and which methods
//         must use the system-level type.  Do NOT combine both in a library.
//       - once you have an error-state enumeration, provide a function that 
//               - converts errors into human readable text
//               - can be simple: error-enum in, const char* out
//       - for multi-threaded cases, set a global error-callback to allow 
//         putting a breakpoint into the callback during bug-hunt sessions
//
// Driver-level defines
#define NOERR                   0   // same as SUCCESS  in OSandPlatform.h
// #define FAILURE             -1   // conflicts with definition in OSandPlatform.h
#define ERR_OOR                -2   // argument or computation out-of-range
#define ERR_CKSUM              -3
#define ERR_UNKNOWN            -1000
#define STATUS_IDLE             1
#define STATUS_CKSUM_OK         2

// system-level error enumeration
typedef enum {
  HOK = 0,
  CHKSUM_ERR,
  LAST_ERR
} error_t;
  

#endif // _INSTR_ERROR_H_
