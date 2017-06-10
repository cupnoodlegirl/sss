#include "sss.cpp"

void test() {
  std::cout << "executing test function" << std::endl;
  std::cout << "secret is ...(unsigned int)" << std::endl;
  std::vector<unsigned char> secret = {100, 100, 100};
  for (auto i = 0; i < secret.size(); i++) {
    std::cout << (unsigned int)secret[i] << std::endl;
  }
  unsigned int num = 5;
  unsigned int th = 3;
  std::cout << "number of shares" << num << std::endl;
  std::cout << "threshold is" << th << std::endl;
  std::vector<std::vector<share>> shares(
      num, std::vector<share>(secret.size(), share(1)));

  shares = split(secret, num, th);

  std::vector<std::vector<share>> ss(
      th, std::vector<share>(secret.size(), share(1)));

  for (auto i = 0; i < th; i++) {
    ss[i] = shares[i];
  }

  std::vector<unsigned char> recoverd_secret(ss.size());

  recoverd_secret = recover(ss);
  std::cout << "recovered secret is ...(unsigned int)" << std::endl;
  for (unsigned int i = 0; i < recoverd_secret.size(); i++) {
    std::cout << static_cast<unsigned int>(recoverd_secret[i]) << std::endl;
  }
}

int main() {
  std::cout << "gf256 library initialization" << std::endl;
  gf256_init();
  test();

  return 0;
}