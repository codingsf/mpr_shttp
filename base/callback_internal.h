#ifndef BASE_CALLBACK_INTERNAL_H_
#define BASE_CALLBACK_INTERNAL_H_
#include "base/atomic_ref_count.h"
#include "base/callback_forward.h"
#include "base/macros.h"
#include "base/ref_counted.h"

namespace base {

namespace internal {

template<CopyMode copy_mode>
class CallbackBase;

class BindStateBase {
 protected:
  explicit BindStateBase(void (*destructor)(BindStateBase*))
    : ref_count_(0),
      destructor_(destructor) {}
  ~BindStateBase() = default;

 private:
  friend class scoped_ref_ptr<BindStateBase>;
  template <CopyMode copy_mode>
  friend class CallbackBase;

  void AddRef();
  void Release();

  AtomicRefCount ref_count_;

  void (*destructor_)(BindStateBase *);

  DISALLOW_COPY_AND_ASSIGN(BindStateBase);
};

template<>
class CallbackBase<CopyMode::MoveOnly> {
 public:
  CallbackBase(CallbackBase&& c);
  CallbackBase& operator=(CallbackBase&& c);

  bool is_null() const { return bind_state_.get() == nullptr; }
  explicit operator bool() const { return !is_null(); }

  void Reset();

 protected:
  using InvokeFuncStorage = void(*)();
  bool EqualsInternal(const CallbackBase& other) const;
  explicit CallbackBase(BindStateBase* bind_state);

  ~CallbackBase();
  
  scoped_ref_ptr<BindStateBase> bind_state_;
  InvokeFuncStorage polymorphic_invoke_ = nullptr;
};

template<>
class CallbackBase<CopyMode::Copyable>
    : public CallbackBase<CopyMode::MoveOnly> {
 public:
  CallbackBase(const CallbackBase& c);
  CallbackBase(CallbackBase&& c);
  CallbackBase& operator=(const CallbackBase& c);
  CallbackBase& operator=(CallbackBase&& c);

 protected:
  explicit CallbackBase(BindStateBase* bind_state)
    : CallbackBase<CopyMode::MoveOnly>(bind_state) {}
  ~CallbackBase() {}
};

extern template class CallbackBase<CopyMode::MoveOnly>;
extern template class CallbackBase<CopyMode::Copyable>;

} // namespace internal
} // namespace base
#endif // BASE_CALLBACK_INTERNAL_H_
