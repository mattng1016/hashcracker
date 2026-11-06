#include <iostream>
#include <openssl/evp.h>

/*clang++ main.cpp \
  -I"$(brew --prefix openssl@3)/include" \
  -L"$(brew --prefix openssl@3)/lib" \
  -lcrypto -o hashcracker */

#include "hashing.h"
#include "cracker.h"

int main(int argc, char *argv[]) {
  std::string wordlist;
  std::string targetHash;
  std::string word;
  std::string hashType;
  int option;

  for (int i = 1; i < argc; i++) {
    std::string flag = argv[i];
    if (flag != "--wordlist" && flag != "--hashvalue" && flag != "--word" && flag != "--hashtype") {
      std::cerr << "Unknown option: " << flag << '\n';
      return 1;
    }
    if (i + 1 >= argc) {
      std::cerr << "Missing flag value for " << flag << '\n';
      return 1;
    }
    std::string value = argv[++i];
    if (value.rfind("--", 0) == 0) {
      std::cerr << "Missing flag value for " << flag << '\n';
      return 1;
    }
    if (flag == "--wordlist") {
      wordlist = value;
      option = 1;
    } else if (flag == "--hashvalue") {
      targetHash = value;
    } else if (flag == "--word") {
      word = value;
      option = 0;
    } else if (flag == "--hashtype") {
      hashType = value;
    }
  }

  if (option == 0) {
    crackWord(word, targetHash, hashType);
  } else if (option == 1) {
    crackList();
  }

  exit(0);
}
