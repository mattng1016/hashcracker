#include <cctype>
#include <iostream>
#include <openssl/evp.h>

/*clang++ main.cpp \
  -I"$(brew --prefix openssl@3)/include" \
  -L"$(brew --prefix openssl@3)/lib" \
  -lcrypto -o hashcracker */

#include "hashing.h"
#include "cracker.h"

enum mode {word, wordlist, salt};

int main(int argc, char *argv[]) {
  std::string wordlist, targetHash, word, saltValue, hashType;
  enum mode option; 
  Salt salt;

  if (argc < 2) {
    std::cout << "Usage: ./hashcracker [--wordlist] [--hashvalue] [--word] [--hashtype] [--salt] [--saltpos]\n";
    exit(1);
  }

  for (int i = 1; i < argc; i++) {
    std::string flag = argv[i];
    if (flag != "--wordlist" && flag != "--hashvalue" && flag != "--word" && flag != "--hashtype" && flag != "--salt" && flag != "--saltpos") {
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
      option = mode::wordlist;
    } else if (flag == "--hashvalue") {
      targetHash = value;
    } else if (flag == "--word") {
      word = value;
      option = mode::word;
    } else if (flag == "--hashtype") {
      hashType = value;
    } else if (flag == "--salt") {
      salt.saltValue = value;
      option = mode::salt;
    } else if (flag == "--saltpos") {
      for (auto& c : value) {
        c = tolower(c);
      }
      if (value == "append") {
        salt.pos = SaltPosition::Append;
      } else if (value == "prepend") {
        salt.pos = SaltPosition::Prepend;
      } else {
        std::cerr << "Invalid value for --saltpos: " << value << "\n";
        exit(1);
      }
    }
  }

  if (option == mode::word) {
    crackWord(word, targetHash, hashType);
  } else if (option == mode::wordlist) {
    Summary s = crackList(wordlist, targetHash, hashType, salt);
    displaySummary(s);
  } else if (option == mode::salt) {
    Summary s = crackList(wordlist, targetHash, hashType, salt);
    displaySummary(s);
  }
  exit(0);
}
