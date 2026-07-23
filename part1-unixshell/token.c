/** 
* File: token.c 
* Description: Soruce code for tokenizing commands from user 
* Date: 22/6/2026
*/ 

#include <stdio.h>
#include <string.h> 
#include "token.h"

// Tokenize outputs the same as the previous tokenizer using strtok() but properly handles "", '', and \
// tk = current character in string
// *tk = Values of the current character in string
// cleanToken = Final tokenized string stored in token array.
int tokenize(char* inputLine, char *token[]){
  char *tk = inputLine; // Current character initialized as the first character of the input line
  int n = 0; // Number of tokens

  // Loops through the fuill input line
  // Each iteration finds one complete token
  // Iterates through each word in the inputLine
  while(*tk != '\0'){
    while(*tk == ' ' || *tk == '\t' || *tk == '\n') tk++; // Ignore delimiters

    // Break out of tokenization loop if end of string if found
    if(*tk == '\0') break;

    token[n] = tk; // Initialize first token index as position of the first character of token
    int isDoubleQuote = 0; // Checks if string is inside double quote
    int isSingleQuote = 0; // Checks if string is inside single quotes
    char *cleanToken = tk; // Full tokenized string

    // Loops through one token/word in the inputLine
    // Finds the start and end of each token and accounts for "", '', and \
    // Iterates through each character in the inputLine, Stops when delimiter is found
    while(*tk != '\0'){

      // Handles \. Deletes it and finalizes token
      if(*tk == '\\'){
        tk++; // Skips '\'

        // Checks next char value.
        if(*tk != '\0'){
          *cleanToken = *tk;
          cleanToken++;
          tk++;
        }
        continue;
      }

      // Handles double quotes.
      if(*tk == '"' && !isSingleQuote){
        isDoubleQuote = !isDoubleQuote;
        tk++;
        continue;
      }

      // Handles Singles quotes, ignores delminiter if found in quote
      if(*tk == '\'' && !isDoubleQuote){
        isSingleQuote = !isSingleQuote;
        tk++;
        continue;
      }

      // Completes token when delmiter is found and the token is not inside of quotes
      if((*tk == ' ' || *tk == '\t' || *tk == '\n') && !isDoubleQuote && !isSingleQuote){
        *tk = '\0';
        tk++;
        break;
      }

      // Copies value of current character to the clean token and moves onto the next
      *cleanToken = *tk;
      cleanToken++;
      tk++;
    }

    *cleanToken = '\0'; // End of string

    // Checks for mismatching quotes
    if(isDoubleQuote || isSingleQuote) return -1;

    n++; // Move on to the next character in string

    // If current number of tokens exceed max value, error is returned;
    if(n >= MAX_NUM_TOKENS) return -1;
  }
  // Set the end of the token array to null. End of the array
  token[n] = NULL;
  return n;
}

// BUG: potential overflow with over 500 files if found. add edge case check 
int expandWildCard(char *token[], char *expandedTokens[], char expandedStorage[][COMMAND_LINE_SIZE], int tokenSize){
  int count = 0; 

  // Exit if empty token 
  if(tokenSize == 0) return 0;

  for(int n = 0; n < tokenSize; n++){
    
    // If wild card is found 
    if((strchr(token[n], '*')) != NULL || strchr(token[n], '?') != NULL){
      
      // results store Count of matched paths, List of matched pathnames, and Slots reserve in gl_pathv
      glob_t results; 
      int status = glob(token[n], 0, NULL, &results); // Glob searches matching patterns 

      // If glob returns successful/matches found, start token expansion 
      if(status == 0){
        for(int i = 0; i < results.gl_pathc; i++){
            
          // Handle overflow 
          if(count >= MAX_NUM_TOKENS -1){
            globfree(&results);
            return -1;
          }

          // Copy into expanded tokens array 
          strcpy(expandedStorage[count], results.gl_pathv[i]);
          expandedTokens[count] = expandedStorage[count]; // gl_pathv stores file names 
          count++; 
        }
      } 
       
      // If no matches found, keep original token
      else {
        strcpy(expandedStorage[count], token[n]);
        expandedTokens[count] = expandedStorage[count];
        count++; 
      }
      
      globfree(&results); // Frees dynamically allocated memory 
    } 

    // Normal Tokens 
    else{
      if(count >= MAX_NUM_TOKENS -1){
        return -1;
      }
      strcpy(expandedStorage[count], token[n]);
      expandedTokens[count] = expandedStorage[count];
      count++; 
    }
  }

  expandedTokens[count] = NULL; // null terminator 
  return count; 
}
