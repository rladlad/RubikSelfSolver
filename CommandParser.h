#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <Arduino.h>

#define MAX_PARAMS 10
#define MAX_INPUT_LENGTH 128

class CommandParser {
public:
  CommandParser();

  bool parse(char* input);
  const char* getCommand() const;
  int getParamCount() const;
  const char* getParam(int index) const;

private:
  char* command;
  char* params[MAX_PARAMS];
  int paramCount;

  bool isValidParam(const char* param);
};

#endif // COMMAND_PARSER_H
