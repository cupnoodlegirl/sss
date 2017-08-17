#include "gf256.cpp"
#include <iostream>
#include <random>
#include <vector>

////////////////////////////////////////////////////////////
/**
* point represents pair of x coordinate and y coordinate
* Be careful that range of coordinate is [0,inf)
*/
struct point {
  unsigned int xcoord;
  unsigned int ycoord;
};

/**
* poly_ycoord returns y coordination of polynomial using x coordination
*/
unsigned char poly_ycoord(std::vector<unsigned char> polycoeffs,
                          unsigned int xcoord) {
  unsigned char acc = 0;
  unsigned char temp = 1;

  for (unsigned int i = 0; i < polycoeffs.size(); i++) {
    acc = gf256_add(acc, gf256_mul(polycoeffs[i], temp));
    temp = gf256_mul(temp, (unsigned char)xcoord);
  }

  return acc;
}

/**
* poly_interpolate returns lagrange interpolation from k(threshold) points
*/
unsigned char poly_interpolate(std::vector<point> points) {
  const unsigned char ord = points.size();
  unsigned char acc = 0;
  for (unsigned int i = 0; i < ord; i++) {
    unsigned char temp = 1;
    for (unsigned int j = 0; j < ord; j++) {
      if (i != j) {
        temp = gf256_mul(temp,
                         gf256_div((unsigned char)points[j].xcoord,
                                   gf256_add((unsigned char)points[i].xcoord,
                                             (unsigned char)points[j].xcoord)));
      }
    }
    acc = gf256_add(acc, gf256_mul(temp, points[i].ycoord));
  }
  return acc;
}

/**
* gen_poly returns randomly generated polynomial
*/
std::vector<unsigned char> gen_poly(unsigned int th, unsigned char secret) {
  std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> dis(0, 255);

  // initialize polynomial
  std::vector<unsigned char> poly(th);
  poly[0] = secret;
  for (unsigned int i = 1; i < th; i++) {
    poly[i] = dis(gen);
  }

  return poly;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/**
* share represents pair of x coordinate and y coordinate
* Be careful that range of coordinate is [0,inf)
*/
struct share {
  unsigned int xcoord;
  unsigned int th;
  std::vector<unsigned char> data;
  explicit share(size_t bs) : xcoord(0), data(bs) {}
  share(unsigned int x, std::vector<unsigned char> d)
      : xcoord(x), data(move(d)) {}
};

/**
* byte_split returns shares from 1 byte secret
*/
std::vector<share> byte_split(unsigned int num, unsigned int th,
                              std::vector<unsigned char> poly) {
  std::vector<share> shares(num, share(1));
  for (unsigned int i = 0; i < num; i++) {
    shares[i].xcoord = i + 1;
    shares[i].th = th;
    shares[i].data[0] = poly_ycoord(poly, i + 1);
  }

  return shares;
}

/**
* byte_recover returns 1 byte secret from k(threshold) shares
*/
unsigned char byte_recover(std::vector<share> shares) {

  std::vector<point> points(shares.size());

  for (unsigned int i = 0; i < shares.size(); i++) {
    points[i].xcoord = shares[i].xcoord;
    points[i].ycoord = shares[i].data[0];
  }
  unsigned char secret;
  secret = poly_interpolate(points);

  return secret;
}

/**
* split returns shares from multi bytes secret
* ith row represents ith share, jth column represents j byte index at secret
* each element is a share from 1 byte secret
*/
std::vector<std::vector<share> > split(std::vector<unsigned char> secret,
                                      unsigned int num, unsigned int th) {
  std::vector<std::vector<share> > shares(
      num, std::vector<share>(secret.size(), share(1)));

  for (unsigned int j = 0; j < secret.size(); j++) { // j byte index
    std::vector<unsigned char> poly(th);
    poly = gen_poly(th, secret[j]); // same polynomial is used at j byte index
    for (unsigned int i = 0; i < num; i++) { // ith share
      std::vector<share> byte_shares(num, share(1));
      byte_shares = byte_split(num, th, poly);
      shares[i][j] = byte_shares[i];
    }
  }

  return shares;
}

/**
* recover returns multi byte secret from k(threshold) shares
*/
std::vector<unsigned char> recover(std::vector<std::vector<share> > shares) {

  std::vector<unsigned char> secret(shares[0].size());
  std::vector<share> byte_shares(shares.size(),
                                 share(1)); // shares against secret byteindex i
  for (unsigned int i = 0; i < shares[0].size(); i++) { // secret size
    for (unsigned int j = 0; j < shares.size(); j++) {  // number of share
      byte_shares[j] = shares[j][i];
    }
    secret[i] = byte_recover(byte_shares);
  }

  return secret;
}
////////////////////////////////////////////////////////////
