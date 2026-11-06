#include <vector>
#include <iostream>
#include <openssl/evp.h>

std::vector<unsigned char> hashWord (std::string word, std::string hashType) {
  EVP_MD_CTX* mdctx;
  const EVP_MD *md;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len, i;
  std::vector<unsigned char> result;

  md = EVP_get_digestbyname(hashType.c_str());
  if (md == NULL) {
    std::cout << "Unknown message digest " << hashType << "\n"; 
    exit(1);
  }

  mdctx = EVP_MD_CTX_new();
  if (!EVP_DigestInit_ex2(mdctx, md, NULL)) {
    std::cout << "Message digest initialization failed. \n";
    EVP_MD_CTX_free(mdctx);
    exit(1);
  }

  if (!EVP_DigestUpdate(mdctx, word.c_str(), word.size())) {
    std::cout << "Message digest update failed. \n";
    EVP_MD_CTX_free(mdctx);
    exit(1);
  }

  if (!EVP_DigestFinal_ex(mdctx, md_value, &md_len)) {
    std::cout << "Message digest finalization failed. \n";
    EVP_MD_CTX_free(mdctx);
    exit(1);
  }

  EVP_MD_CTX_free(mdctx);
  for (int i = 0; i < md_len; i++) {
    result.push_back(md_value[i]);
  }
  return result;
}
