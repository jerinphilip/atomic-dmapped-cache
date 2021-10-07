
#include <iostream>
#include <memory>
#include <random>
#include <vector>

template <class Key, class Value, class Hash = std::hash<Key>,
          class Equals = std::equal_to<Key>>
class SimpleCache {
public:
  struct Record {
    Key key;
    Value value;
  };

  // Floating, because ownership is floating.
  using FloatingRecord = std::shared_ptr<Record>;

  explicit SimpleCache(std::size_t size) : records_(size, nullptr) {}

  FloatingRecord Find(const Key &key) const {
    const FloatingRecord &record = records_[hash_(key) % records_.size()];
    FloatingRecord ret = std::atomic_load(&record);
    if (ret && equals_(key, ret->key)) {
      return ret;
    } else {
      return FloatingRecord{nullptr};
    }
  }

  void Store(FloatingRecord &recordIn) {
    FloatingRecord &record = records_[hash_(recordIn->key) % records_.size()];
    atomic_store(&record, recordIn);
  }

private:
  std::vector<FloatingRecord> records_;

  Hash hash_;
  Equals equals_;
};
