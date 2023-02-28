#include "../../util/fd_util.h"
extern "C" {
#include "../fd_funk.h"
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string_view>
#include <unordered_map>

// A simple, fast, but not super great random number generator
class randgen {
  private:
    ulong _s1 = 0;
    ulong _s2 = 0;
    ulong _s3 = 0;

    void mix() {
      _s1 = (_s1+135075924757140979UL)*625099173779010167UL;
      _s2 = (_s2+578819778140026727UL)*115476946196358727UL;
      _s3 = (_s3+237010196533530919UL)*274199408033833319UL;
      _s1 ^= _s2>>31U;
      _s2 ^= _s3>>31U;
      _s3 ^= _s1>>31U;
    }
    
  public:
    randgen() { }
    ~randgen() { }

    void genbytes(char* data, unsigned len) {
      mix();
      while (len >= sizeof(ulong)) {
        memcpy(data, &_s1, sizeof(ulong));
        data += sizeof(ulong);
        len -= (unsigned)sizeof(ulong);
        mix();
      }
      memcpy(data, &_s1, len);
    }
};

struct recordkey {
    fd_funk_recordid _id;
    recordkey() { }
    recordkey(const recordkey& x) { memcpy(&_id, &x._id, sizeof(_id)); }
    recordkey(recordkey&& x) { memcpy(&_id, &x._id, sizeof(_id)); }
    bool operator== (const recordkey& x) const {
      return memcmp(&_id, &x._id, sizeof(_id)) == 0;
    }
    operator const fd_funk_recordid* () const { return &_id; }
};

struct recordkeyhash {
    size_t operator() (const recordkey& key) const {
      std::string_view v((const char*)&key._id, sizeof(key._id));
      return std::hash<std::string_view>{}(v);
    }
};

struct xactionkey {
    fd_funk_xactionid _id;
    xactionkey() { }
    xactionkey(const xactionkey& x) { memcpy(&_id, &x._id, sizeof(_id)); }
    xactionkey(xactionkey&& x) { memcpy(&_id, &x._id, sizeof(_id)); }
    xactionkey(const fd_funk_xactionid* id) { memcpy(&_id, id, sizeof(_id)); }
    bool operator== (const xactionkey& x) const {
      return memcmp(&_id, &x._id, sizeof(_id)) == 0;
    }
    operator const fd_funk_xactionid* () const { return &_id; }
};

struct xactionkeyhash {
    size_t operator() (const xactionkey& key) const {
      std::string_view v((const char*)&key._id, sizeof(key._id));
      return std::hash<std::string_view>{}(v);
    }
};

class databuf {
  private:
    char* _buf;
    ulong _buflen;

  public:
    databuf() {
      _buf = nullptr;
      _buflen = 0;
    }
    databuf(const databuf& x) {
      if (x._buf == nullptr) {
        _buf = nullptr;
        _buflen = 0;
      } else {
        _buf = (char*)malloc(_buflen = x._buflen);
        memcpy(_buf, x._buf, _buflen);
      }
    }
    databuf(databuf&& x) {
      _buf = x._buf;
      x._buf = nullptr;
      _buflen = x._buflen;
      x._buflen = 0;
    }
    ~databuf() {
      if (_buf != nullptr)
        free(_buf);
    }
    databuf& operator= (const databuf& x) = delete;
    databuf& operator= (databuf&& x) = delete;

    void write(const void* data, ulong offset, ulong datalen) {
      if (datalen == 0)
        return;
      if (_buf == nullptr) {
        _buf = (char*)malloc(offset + datalen);
        if (offset > 0)
          memset(_buf, 0, offset);
        _buflen = offset + datalen;
      } else if (offset + datalen > _buflen) {
        _buf = (char*)realloc(_buf, offset + datalen);
        if (offset > _buflen)
          memset(_buf + _buflen, 0, offset - _buflen);
        _buflen = offset + datalen;
      }
      memcpy(_buf + offset, data, datalen);
    }

    bool equals(const void* data, ulong datalen) const {
      return datalen == _buflen && memcmp(_buf, data, datalen) == 0;
    }
};

static const unsigned MAXRECORDSIZE = 100000;
uint random_size(randgen& rg) {
  // Simulate a crude exponential distribution of sizes
  uint s;
  rg.genbytes((char*)&s, sizeof(s));
  s = s%MAXRECORDSIZE;
  if (s < 95*MAXRECORDSIZE/100)
    // 95% are less than 1000
    s /= 100;
  else if (s < 99*MAXRECORDSIZE/100)
    // 4% are less than 10000
    s /= 10;
  return s;
}

int main(int argc, char **argv) {
  fd_boot( &argc, &argv );

  unlink("testback");

  fd_wksp_t* wksp = fd_wksp_new_anonymous( FD_SHMEM_GIGANTIC_PAGE_SZ, 1UL, fd_log_cpu_id(), "wksp", 0UL );

  ulong index_max = 1000000;    // Maximum size (count) of master index
  ulong xactions_max = 100;     // Maximum size (count) of transaction index
  ulong cache_max = 10000;      // Maximum number of cache entries
  auto* funk = fd_funk_new("testback", wksp, 1, index_max, xactions_max, cache_max);

  fd_funk_validate(funk);

  char* scratch = (char*)malloc(MAXRECORDSIZE);

  typedef std::unordered_map<recordkey,databuf,recordkeyhash> xactionstate_t;
  std::unordered_map<xactionkey,xactionstate_t,xactionkeyhash> golden;

  xactionkey rootxid(fd_funk_root(funk));

  randgen rg;
  xactionkey xid;
  rg.genbytes((char*)&xid, sizeof(xid));
  fd_funk_fork(funk, rootxid, xid);
  golden[xid] = golden[rootxid];
  for (unsigned i = 0; i < 100; ++i) {
    recordkey key;
    rg.genbytes((char*)&key, sizeof(key));
    auto len = random_size(rg);
    rg.genbytes(scratch, len);
    if (fd_funk_write(funk, xid, key, scratch, 0, len) != (long)len)
      FD_LOG_ERR(("write failed"));
    databuf& db = golden[xid][key];
    db.write(scratch, 0, len);
  }

  auto validateall = [&](){
    fd_funk_validate(funk);
    if (fd_funk_num_records(funk) != golden[rootxid].size())
      FD_LOG_ERR(("wrong record count"));
    for (auto& [xid,xstate] : golden) {
      for (auto& [key,db] : xstate) {
        const void* res;
        auto reslen = fd_funk_read(funk, xid, key, &res, 0, MAXRECORDSIZE);
        if (!db.equals(res, reslen))
          FD_LOG_ERR(("read returned wrong result"));
      }
    }
  };
  validateall();

  fd_funk_commit(funk, xid);
  golden[rootxid] = golden[xid];
  golden.erase(xid);

  validateall();

  auto reload = [&](){
    fd_funk_delete(funk);
    fd_wksp_tag_free(wksp, 1);
    for (auto it = golden.begin(); it != golden.end(); ) {
      if (it->first == rootxid)
        ++it;
      else
        it = golden.erase(it);
    }
    funk = fd_funk_new("testback", wksp, 1, 100000, 100, 10000);
  };
  reload();

  validateall();

  rg.genbytes((char*)&xid, sizeof(xid));
  fd_funk_fork(funk, rootxid, xid);
  golden[xid] = golden[rootxid];
  for (unsigned i = 0; i < 100; ++i) {
    recordkey key;
    rg.genbytes((char*)&key, sizeof(key));
    auto len = random_size(rg);
    rg.genbytes(scratch, len);
    if (fd_funk_write(funk, xid, key, scratch, 0, len) != (long)len)
      FD_LOG_ERR(("write failed"));
    databuf& db = golden[xid][key];
    db.write(scratch, 0, len);
  }

  validateall();
  // Implicitly cancels all uncommitted transaction
  reload();
  validateall();

  xactionkey* prevxid = &rootxid;
  xactionkey xidchain[10];
  for (unsigned j = 0; j < 10; ++j) {
    rg.genbytes((char*)&xidchain[j], sizeof(xidchain[j]));
    fd_funk_fork(funk, *prevxid, xidchain[j]);
    golden[xidchain[j]] = golden[*prevxid];
    for (unsigned i = 0; i < 100; ++i) {
      recordkey key;
      rg.genbytes((char*)&key, sizeof(key));
      auto len = random_size(rg);
      rg.genbytes(scratch, len);
      if (fd_funk_write(funk, xidchain[j], key, scratch, 0, len) != (long)len)
        FD_LOG_ERR(("write failed"));
      databuf& db = golden[xidchain[j]][key];
      db.write(scratch, 0, len);
    }
    prevxid = &xidchain[j];
  }

  validateall();

  fd_funk_commit(funk, xidchain[9]);
  golden[rootxid] = golden[xidchain[9]];
  for (unsigned j = 0; j < 10; ++j)
    golden.erase(xidchain[j]);
  
  validateall();

  prevxid = &rootxid;
  for (unsigned j = 0; j < 10; ++j) {
    rg.genbytes((char*)&xidchain[j], sizeof(xidchain[j]));
    fd_funk_fork(funk, *prevxid, xidchain[j]);
    golden[xidchain[j]] = golden[*prevxid];
    for (auto& [key,_] : golden[rootxid]) {
      auto len = random_size(rg);
      rg.genbytes(scratch, len);
      if (fd_funk_write(funk, xidchain[j], key, scratch, 0, len) != (long)len)
        FD_LOG_ERR(("write failed"));
      databuf& db = golden[xidchain[j]][key];
      db.write(scratch, 0, len);
    }
    prevxid = &xidchain[j];
  }

  validateall();

  fd_funk_commit(funk, xidchain[9]);
  golden[rootxid] = golden[xidchain[9]];
  for (unsigned j = 0; j < 10; ++j)
    golden.erase(xidchain[j]);
  
  validateall();
  reload();
  validateall();

  for (auto& [key,_] : golden[rootxid])
    fd_funk_delete_record(funk, rootxid, key);
  golden.clear();

  validateall();

  for (unsigned i = 0; i < 100; ++i) {
    recordkey key;
    rg.genbytes((char*)&key, sizeof(key));
    auto len = 0;
    if (fd_funk_write(funk, rootxid, key, scratch, 0, len) != (long)len)
      FD_LOG_ERR(("write failed"));
    databuf& db = golden[rootxid][key];
    db.write(scratch, 0, len);
  }

  validateall();

  prevxid = &rootxid;
  for (unsigned j = 0; j < 10; ++j) {
    rg.genbytes((char*)&xidchain[j], sizeof(xidchain[j]));
    fd_funk_fork(funk, *prevxid, xidchain[j]);
    golden[xidchain[j]] = golden[*prevxid];
    for (auto& [key,_] : golden[rootxid]) {
      auto len = 50;
      auto offset = 100*(j+1);
      rg.genbytes(scratch, len);
      if (fd_funk_write(funk, xidchain[j], key, scratch, offset, len) != (long)len)
        FD_LOG_ERR(("write failed"));
      databuf& db = golden[xidchain[j]][key];
      db.write(scratch, offset, len);
    }
    prevxid = &xidchain[j];
  }

  validateall();

  fd_funk_commit(funk, xidchain[9]);
  golden[rootxid] = golden[xidchain[9]];
  for (unsigned j = 0; j < 10; ++j)
    golden.erase(xidchain[j]);
  
  validateall();
  reload();
  validateall();

  prevxid = &rootxid;
  for (unsigned j = 0; j < 10; ++j) {
    rg.genbytes((char*)&xidchain[j], sizeof(xidchain[j]));
    fd_funk_fork(funk, *prevxid, xidchain[j]);
    golden[xidchain[j]] = golden[*prevxid];
    for (auto& [key,_] : golden[rootxid]) {
      if ((j&1) == 1) {
        fd_funk_delete_record(funk, xidchain[j], key);
        golden[xidchain[j]].erase(key);
        continue;
      }
      auto len = 50;
      auto offset = 100*(j+1);
      rg.genbytes(scratch, len);
      if (fd_funk_write(funk, xidchain[j], key, scratch, offset, len) != (long)len)
        FD_LOG_ERR(("write failed"));
      databuf& db = golden[xidchain[j]][key];
      db.write(scratch, offset, len);
    }
    prevxid = &xidchain[j];
  }

  validateall();

  fd_funk_commit(funk, xidchain[9]);
  golden[rootxid] = golden[xidchain[9]];
  for (unsigned j = 0; j < 10; ++j)
    golden.erase(xidchain[j]);
  
  validateall();

  free(scratch);
  
  fd_funk_delete(funk);
  fd_wksp_detach(wksp);
  unlink("testback");

  FD_LOG_WARNING(("test passed!"));
  fd_log_flush();
  return 0;
}
