#include <gtest/gtest.h>
#include <string_view>
#include "time_index/time_index.hpp"

using Timestamp = uint64_t;
using Key = uint64_t;
const uint64_t ExpireInterval = 10; 

struct TestStruct 
  : public Aux::TimedItem<TestStruct, Key, Timestamp> 
{
  public:
    TestStruct(Timestamp t, const char * val)
      : t_(t),
        value_(val)
    {}

    bool Expired(const Timestamp& t) const override final {
      return t - t_ > ExpireInterval; 
    }

    std::string Value() {
      return value_;
    }
  private:
    const Timestamp t_;
    std::string value_;
};

TEST(TimeIndex, InsertAndFind)
{
  Aux::TimeIndex<TestStruct, Key, Timestamp> storage;
  TestStruct::Ptr t1(new TestStruct(0, "test"));

  storage.Insert(123, t1);
  auto found = storage.Find(123);
  ASSERT_TRUE(t1 == found);
  ASSERT_TRUE(t1->value().Value() == found->value().Value());

  auto notfound = storage.Find(321);
  ASSERT_TRUE(notfound == TestStruct::Ptr());

}

TEST(TimeIndex, Remove)
{
  Aux::TimeIndex<TestStruct, Key, Timestamp> storage;

  storage.Insert(123, TestStruct::Ptr(new TestStruct(0,  "expired")));
  storage.Insert(456, TestStruct::Ptr(new TestStruct(5,  "also expired")));
  storage.Insert(789, TestStruct::Ptr(new TestStruct(15, "not expired")));

  auto found = storage.Find(123);
  storage.Remove(found);
  auto notfound = storage.Find(123);
  ASSERT_TRUE(notfound == TestStruct::Ptr());

  found = storage.Find(456);
  storage.Remove(found);
  notfound = storage.Find(456);
  ASSERT_TRUE(notfound == TestStruct::Ptr());

  found = storage.Find(789);
  storage.Remove(found);
  notfound = storage.Find(789);
  ASSERT_TRUE(notfound == TestStruct::Ptr());

  ASSERT_TRUE(storage.Empty());
}

TEST(TimeIndex, Expire)
{
  const Timestamp current = 20;

  Aux::TimeIndex<TestStruct, Key, Timestamp> storage;

  storage.Insert(123, TestStruct::Ptr(new TestStruct(0,  "expired")));
  storage.Insert(456, TestStruct::Ptr(new TestStruct(5,  "also expired")));
  storage.Insert(789, TestStruct::Ptr(new TestStruct(15, "not expired")));

  storage.CheckExpire(current);

  auto findExpired = storage.Find(123);
  ASSERT_TRUE(findExpired == TestStruct::Ptr());

  findExpired = storage.Find(456);
  ASSERT_TRUE(findExpired == TestStruct::Ptr());

  auto found= storage.Find(789);
  ASSERT_TRUE(found->value().Value() == "not expired");
}


TEST(TimeIndex, Update)
{
  const Timestamp current = 20;

  Aux::TimeIndex<TestStruct, Key, Timestamp> storage;

  storage.Insert(123, TestStruct::Ptr(new TestStruct(0,  "expired")));
  storage.Insert(456, TestStruct::Ptr(new TestStruct(5,  "also expired, but then updated")));
  storage.Insert(789, TestStruct::Ptr(new TestStruct(15, "not expired")));

  auto toUpdate = storage.Find(456);
  storage.Update(toUpdate);

  storage.CheckExpire(current);

  auto findExpired = storage.Find(123);
  ASSERT_TRUE(findExpired == TestStruct::Ptr());

  findExpired = storage.Find(456);
  ASSERT_TRUE(findExpired != TestStruct::Ptr());

  auto found= storage.Find(789);
  ASSERT_TRUE(found->value().Value() == "not expired");
}

