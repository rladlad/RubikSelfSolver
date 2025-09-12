#include "CommandParser.h"

CommandParser::CommandParser() : command(nullptr), paramCount(0) {}

bool CommandParser::parse(char* input) {
  paramCount = 0;

  if (strlen(input) >= MAX_INPUT_LENGTH) {
    Serial.println("Error: Input too long");
    return false;
  }

  command = strtok(input, " ");
  if (command == nullptr || strlen(command) == 0) {
    Serial.println("Error: Missing command");
    return false;
  }

  char* token = strtok(nullptr, " ");
  while (token != nullptr && paramCount < MAX_PARAMS) {
    if (!isValidParam(token)) {
      Serial.print("Error: Invalid param: ");
      Serial.println(token);
      return false;
    }
    params[paramCount++] = token;
    token = strtok(nullptr, " ");
  }

  return true;
}

const char* CommandParser::getCommand() const {
  return command;
}

int CommandParser::getParamCount() const {
  return paramCount;
}

const char* CommandParser::getParam(int index) const {
  if (index < 0 || index >= paramCount) return nullptr;
  return params[index];
}

bool CommandParser::isValidParam(const char* param) {
  for (int i = 0; param[i] != '\0'; i++) {
    if (!isdigit(param[i]) && param[i] != '-' && param[i] != '.') {
      return false;
    }
  }
  return true;
}
