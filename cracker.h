#include <iostream>
#include <chrono>

struct Summary {  
  std::chrono::duration<double> duration;
  u_int32_t attempts = 0;
};

void displaySummary(const Summary& summary);
bool crackWord(std::string word, std::string hashValue, std::string hashType);
Summary crackList(std::string fileName, std::string hashValue, std::string hashType);
