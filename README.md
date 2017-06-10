Shamir Secret Sharing library
---

## Test

```
g++ test_sss.cpp -std=c++11 -g
```

## How to use

```C++
#include "sss.cpp"

int main() {
    // Required for initialization
    gf256_init();

    // split into shares
    std::vector<unsigned char> secret = {100, 100, 100};
  
    unsigned int num = 5;
    unsigned int th = 3;
  
    std::vector<std::vector<share>> shares(
        num, std::vector<share>(secret.size(), share(1)));

    shares = split(secret, num, th);

    // recover from shares
    std::vector<std::vector<share>> ss(
        th, std::vector<share>(secret.size(), share(1)));

    for (auto i = 0; i < th; i++) {
        ss[i] = shares[i];
    }

    std::vector<unsigned char> recoverd_secret(ss.size());

    recoverd_secret = recover(ss);

    return 0;
}
```

Dependency

https://github.com/catid/gf256