#include "base/callback_internal.h"

#include <glog/logging.h>


namespace base {
namespace internal {

void BindStateBase::AddRef() {
  AtomicRefCountInc(&ref_count_);
}

void BindStateBase::Release() {
  if (!AtomicRefCountDec(&ref_count_))
    destructor_(this);
}

CallbackBase<CopyMode::MoveOnly>::CallbackBase(CallbackBase&& ) = default;

CallbackBase<CopyMode::MoveOnly>&
CallbackBase<CopyMode::MoveOnly>::operator=(CallbackBase&& ) = default;

void CallbackBase<CopyMode::MoveOnly>::Reset() {
  polymorphic_invoke_ = nullptr;
  bind_state_ = nullptr;
}

bool CallbackBase<CopyMode::MoveOnly>::EqualsInternal(
    const CallbackBase& other) const {
  // Ignore |polymorphic_invoke_| value in null case.
  if (!bind_state_ || !other.bind_state_)
    return bind_state_ == other.bind_state_;
  return bind_state_ == other.bind_state_ &&
         polymorphic_invoke_ == other.polymorphic_invoke_;
}

CallbackBase<CopyMode::MoveOnly>::CallbackBase(
    BindStateBase* bind_state)
    : bind_state_(bind_state) {
  DCHECK(!bind_state_.get() || bind_state_->ref_count_ == 1);
}

CallbackBase<CopyMode::MoveOnly>::~CallbackBase() {}

CallbackBase<CopyMode::Copyable>::CallbackBase(
    const CallbackBase& c)
    : CallbackBase<CopyMode::MoveOnly>(nullptr) {
  bind_state_ = c.bind_state_;
  polymorphic_invoke_ = c.polymorphic_invoke_;
}

CallbackBase<CopyMode::Copyable>::CallbackBase(CallbackBase&& ) = default;

CallbackBase<CopyMode::Copyable>&
CallbackBase<CopyMode::Copyable>::operator=(const CallbackBase& c) {
  bind_state_ = c.bind_state_;
  polymorphic_invoke_ = c.polymorphic_invoke_;
  return *this;
}

CallbackBase<CopyMode::Copyable>&
CallbackBase<CopyMode::Copyable>::operator=(CallbackBase&& ) = default;

template class CallbackBase<CopyMode::MoveOnly>;
template class CallbackBase<CopyMode::Copyable>;

}  // namespace internal
}  // namespace base
