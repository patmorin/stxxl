#include <iostream>
#include <limits>

#include <stxxl/queue>
#include <stxxl/bits/common/binary_buffer.h>


#define MAXTUPLE 21

typedef std::int8_t blob[1+MAXTUPLE*3];

struct blobby {
  blob b;
};

class triple {
public:
  int x, y, t;
  triple() : triple(1, 1, 1) {};
  triple(int x0, int y0, int t0) {
    x = x0;
    y = y0;
    t = t0;
  }
};

bool compare_triples(const triple &t1, const triple &t2) {
  return t1.x < t2.x || t1.y < t2.y || t1.t < t2.t;
}

// TODO: normalize at lower-left corner
class solution {
public:
  unsigned n;
  triple s[MAXTUPLE];

  solution() {
    n = 1;
    s[0].x = 0;
    s[0].y = 0;
    s[0].t = 1;
  }

  solution(unsigned init_n) {
    n = init_n;
    for (unsigned i = 0; i < n; i++) {
      s[i].x = i;
      s[i].y = i;
      s[i].t = 1;
    }
  }

  solution(solution &other) {
    n = other.n;
    std::copy_n(other.s, n, s);
  }

  solution(blob a) {
    n = (unsigned)a[0];
    for (unsigned i = 0; i < n; i++) {
      s[i].x = a[3*i+1];
      s[i].y = a[3*i+2];
      s[i].t = a[3*i+3];
    }
  }

  bool valid(const triple &t) const {
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
    // std::sort(s, s+n, compare_triples);
  }

  void pack(blob &a) const {
    a[0] = (std::int8_t)n;
    for (unsigned i = 0; i < n; i++) {
      a[3*i+1] = (std::int8_t)s[i].x;
      a[3*i+2] = (std::int8_t)s[i].y;
      a[3*i+3] = (std::int8_t)s[i].t;
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


void enqueue(const solution &s, stxxl::queue<blobby> &q) {
  blobby b;
  s.pack(b.b);
  q.push(b);
}

void seed5(stxxl::queue<blobby> &q) {
  int r = 5;
  for (int i = 1; i < r; i++) {
    for (int j = i+1; j <= r; j++) {
      solution(s);
      s.n = 0;
      int m = 0;
      for (int p = 1; p <= r; p++) {
        if (p != i && p != j) {
          s.add(triple(++m, 0, p));
        }
      }
      std::cout << s << std::endl;
      enqueue(s, q);
    }
  }
}

int main(int argc, char *argv[])
{
  // command line arguments
  int r = 5;
  if (argc == 2) {
    r = std::stoi(argv[1]);
  }

  // seed the queue with a trivial solution
  solution s0;
  int size = s0.size();
  stxxl::queue<blobby> q[50];
  enqueue(s0, q[size]);

  // start exploring
  double best = 0;
  std::uint64_t iteration = 1;
  while (1 < 2) {
    while (q[size].empty()) {
      size++;
    }
    solution s(q[size].front().b);
    q[size].pop();
    if (iteration % 1000 == 0) {
      std::cout << iteration << " " << size << " " << best << " " << "    \r";
    }
    iteration++;
    int minx = 1000, maxx = -1000, miny = 1000, maxy = -1000;
    for (unsigned i = 0; i < s.n; i++) {
      if (s.s[i].x < minx) minx = s.s[i].x;
      if (s.s[i].x > maxx) maxx = s.s[i].x;
      if (s.s[i].y < miny) miny = s.s[i].y;
      if (s.s[i].y > maxy) maxy = s.s[i].y;
    }
    int size = maxx-minx + maxy-miny + 2;
    double ratio = 2.0*s.n/size;
    if (ratio >= best) {
      best = ratio;
      std::cout << "size = " << size << ", score = " << s.n
                << ", ratio = " << ratio << std::endl;
      std::cout << s << std::endl;
      s.pretty_print(std::cout);
    }

    for (int x = minx-1; x <= maxx+1; x++) {
      for (int y = miny-1; y <= maxy+1; y++) {
        for (int t = 1; t <= r; t++) {
          // std::cout << triple(x, y, t);
          if (s.valid(triple(x, y, t))) {
            solution s1(s);
            s1.add(triple(x,y,t));
            // std::cout << "  Enqueuing " << s1 << std::endl;
            enqueue(s1, q[s1.size()]);
          }
        }
      }
    }
  }
  return 0;

}
