#include <iostream>
#include <limits>

#define MAXTUPLE 21

class triple {
public:
  int x, y, t;
  triple() : triple(1, 1, 1) {};
  triple(int x0, int y0, int t0) {
    x = x0;
    y = y0;
    t = t0;
  }
  bool operator==(const triple &other) const {
    return x == other.x && y == other.y && t == other.t;
  }
  bool operator!=(const triple &other) const {
    return x != other.x || y != other.y || t != other.t;
  }
};

bool compare_triples(const triple &t1, const triple &t2) {
  if (t1.x < t2.x) return true;
  if (t1.x == t2.x && t1.y < t2.y) return true;
  if (t1.x == t2.x && t1.y == t2.y && t1.t < t2.t) return true;
  return false;
}


class solution {
public:
  unsigned n;
  triple s[MAXTUPLE];

  solution() {
    n = 1;
    s[0].x = 0;
    s[0].y = 0;
    s[0].t = 1;
    normalize();
  }

  bool valid_addition(const triple &t) const {
      for (unsigned i = 0; i < n; i++) {
        const triple &s = this->s[i];
        if (s.y == t.y && (s.x-t.x)*(s.t-t.t) <= 0) return false;
        if (s.x == t.x && (s.y-t.y)*(s.t-t.t) <= 0) return false;
        if (s.t == t.t && (s.x-t.x)*(s.y-t.y) <= 0) return false;
      }
      return true;
  }

  void add(const triple &t) {
    s[n++] = t;
    unsigned i = n-1;
    while (i > 0 and compare_triples(s[i], s[i-1])) {
      std::swap(s[i], s[i-1]);
      i--;
    }
    normalize();
  }

  bool valid_removal(unsigned si) {
    int minx = 1000, maxx = -1000, miny = 1000, maxy = -1000;
    triple t = s[si];
    int samex = 0, samey = 0;
    for (unsigned i = 0; i < n; i++) {
      if (s[i].x == t.x) samex++;
      if (s[i].y == t.y) samey++;
      if (s[i].x < minx) minx = s[i].x;
      if (s[i].x > maxx) maxx = s[i].x;
      if (s[i].y < miny) miny = s[i].y;
      if (s[i].y > maxy) maxy = s[i].y;
    }
    if (samex == 1 && t.x != minx && t.x != maxx) return false;
    if (samey == 1 && t.y != miny && t.y != maxy) return false;
    return true;
  }

  void remove(unsigned si) {
    n--;
    for (unsigned i = si; i < n; i++)
      s[i] = s[i+1];
    normalize();
  }

  bool operator==(const solution &other) const {
    if (n != other.n) return false;
    for (unsigned i = 0; i < n; i++) {
      if (s[i] != other.s[i]) return false;
    }
    return true;
  }

  void normalize() {
    int minx = 1000, maxx = -1000, miny = 1000, maxy = -1000;
    for (unsigned i = 0; i < n; i++) {
      if (s[i].x < minx) minx = s[i].x;
      if (s[i].x > maxx) maxx = s[i].x;
      if (s[i].y < miny) miny = s[i].y;
      if (s[i].y > maxy) maxy = s[i].y;
    }
    for (unsigned i = 0; i < n; i++) {
      s[i].x -= minx;
      s[i].y -= miny;
    }
  }

  unsigned size() const {
    int minx = 1000, maxx = -1000, miny = 1000, maxy = -1000;
    for (unsigned i = 0; i < n; i++) {
      if (s[i].x < minx) minx = s[i].x;
      if (s[i].x > maxx) maxx = s[i].x;
      if (s[i].y < miny) miny = s[i].y;
      if (s[i].y > maxy) maxy = s[i].y;
    }
    return maxx - minx + maxy - miny + 2;
  }

  void pretty_print(std::ostream &out) const {
    int minx = 1000, maxx = -1000, miny = 1000, maxy = -1000;
    for (unsigned i = 0; i < n; i++) {
      if (s[i].x < minx) minx = s[i].x;
      if (s[i].x > maxx) maxx = s[i].x;
      if (s[i].y < miny) miny = s[i].y;
      if (s[i].y > maxy) maxy = s[i].y;
    }
    for (int x = minx; x <= maxx; x++) {
      for (int y = maxy; y >= miny; y--) {
        bool occupied = false;
        for (unsigned i = 0; i < n; i++) {
          if (s[i].x == x && s[i].y == y) {
            out << s[i].t;
            occupied = true;
          }
        }
        if (!occupied) {
          out << " ";
        }
      }
      out << std::endl;
    }
  }
};

bool compare_solutions(const solution &s1, const solution &s2) {
  if (s1.size() < s2.size()) return true;
  if (s1.n < s2.n) return true;
  for (unsigned i = 0; i < s1.n; i++) {
    if (compare_triples(s1.s[i], s2.s[i])) return true;
    if (compare_triples(s2.s[i], s1.s[i])) return false;
  }
  return false;
}

std::ostream & operator << (std::ostream &out, const triple& t) {
  out << "(" << t.x << "," << t.y << "," << t.t << ")";
  return out;
}

std::ostream & operator << (std::ostream &out, const solution& s) {
  out << "[";
  for (unsigned i = 0; i < s.n; i++) {
    out << s.s[i];
  }
  out << "]";
  return out;
}


std::uint64_t iteration = 0;
double bestyet = 0;

void explore(const solution &s, int r, unsigned maxsize) {
  if (s.size() > maxsize) return;

  // std::cout << "=====================================" << std::endl;
  // s.pretty_print(std::cout);

  double score = 2.0*s.n/s.size();
  if (score >= bestyet) {
    std::cout << "--------------------------" << std::endl;
    std::cout << s.size() << " " << s.n << " " << score << std::endl;
    s.pretty_print(std::cout);
    std::cout << "--------------------------" << std::endl;
    bestyet=score;
  }
  if (iteration % 10000 == 0) {
      std::cout << "r=" << r << ", maxsize=" << maxsize
                << ", iteration=" << iteration << "            \r";
      std::cout.flush();
  }
  iteration++;
  int minx = 1000, maxx = -1000, miny = 1000, maxy = -1000;
  for (unsigned i = 0; i < s.n; i++) {
    if (s.s[i].x < minx) minx = s.s[i].x;
    if (s.s[i].x > maxx) maxx = s.s[i].x;
    if (s.s[i].y < miny) miny = s.s[i].y;
    if (s.s[i].y > maxy) maxy = s.s[i].y;
  }

  // randomize the search space
  triple totry[1000];
  unsigned k = 0;
  for (int x = minx-1; x <= maxx+1; x++) {
    for (int y = miny-1; y <= maxy+1; y++) {
      for (int t = 1; t <= r; t++) {
        totry[k++] = triple(x, y, t);
      }
    }
  }

  for (unsigned ti = 0; ti < k; ti++) {
    unsigned ri = ti+rand()%(k-ti);
    triple xyt = totry[ri];
    totry[ri] = totry[ti];
    if (s.valid_addition(xyt)) {
      // std::cout << "Adding " << triple(x,y,t) << std::endl;
      solution s1 = s;
      s1.add(xyt);
      // std::cout << "=====================================" << std::endl;
      // s1.pretty_print(std::cout);
      solution s1p;
      for (unsigned i = 0; i < s1.n; i++) {
        if (s1.valid_removal(i)) {
          // std::cout << "--" << std::endl;
          solution stmp = s1;
          // std::cout << stmp << std::endl;
          // std::cout << "removing " << i << " - " << s1.s[i] << std::endl;
          stmp.remove(i);
          // std::cout << stmp << std::endl;
          // stmp.pretty_print(std::cout);
          if (i == 0 || compare_solutions(stmp, s1p)) {
            s1p = stmp;
          }
        }
      }
      // std::cout << "-------" << std::endl;
      // s1p.pretty_print(std::cout);
      // std::cout << "vs" << std::endl;
      // s.pretty_print(std::cout);
      // std::cout << "-------" << std::endl;
      // std::cout << (s1p == s) << std::endl;
      if (s1p == s) {
        explore(s1, r, maxsize);
      }
    }
  }
}

int main(int argc, char *argv[])
{
  // command line arguments
  int r = 5;
  unsigned maxsize = 12;
  if (argc >= 2) {
    r = std::stoi(argv[1]);
  }
  if (argc >= 3) {
    maxsize = std::stoi(argv[2]);
  }
  // seed the queue with a trivial solution
  solution s0;
  explore(s0, r, maxsize);
}
