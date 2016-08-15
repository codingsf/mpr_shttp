#ifndef BASE_CALLBACK_FORWARD_H_
#define BASE_CALLBACK_FORWARD_H_
namespace base {

namespace internal {

enum class CopyMode {
  MoveOnly,
  Copyable,
};

} // namespace internal

template<typename Signature,
         internal::CopyMode copy_mode = internal::CopyMode::Copyable>
class Callback;

using Closure = Callback<void()>;

} // namespace base
#endif // BASE_CALLBACK_FORWARD_H_
