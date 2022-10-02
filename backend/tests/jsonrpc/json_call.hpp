#ifndef RPC_CALL_H_
#define RPC_CALL_H_

#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <boost/mp11.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/string_view.hpp>
#include <stdexcept>
#include <string>

namespace rpc {

template <class C1, class C2, class R, class... A, std::size_t... I>
boost::json::value call_impl_(C1 &c1, R (C2::*pmf)(A...),
                              boost::json::array const &args,
                              std::index_sequence<I...>) {
  return boost::json::value_from(
      (c1.*pmf)(boost::json::value_to<boost::remove_cv_ref_t<A>>(args[I])...));
}

template <class C1, class C2, class R, class... A>
boost::json::value call_impl(C1 &c1, R (C2::*pmf)(A...),
                             boost::json::array const &args) {
  if (args.size() != sizeof...(A)) {
    throw std::invalid_argument("Invalid number of arguments");
  }

  return call_impl_(c1, pmf, args, std::index_sequence_for<A...>());
}

template <class C>
boost::json::value call(C &c, boost::string_view method,
                        boost::json::value const &args) {
  using Fd =
      boost::describe::describe_members<C, boost::describe::mod_public |
                                               boost::describe::mod_function>;

  bool found = false;
  boost::json::value result;

  boost::mp11::mp_for_each<Fd>([&](auto D) {
    if (!found && method == D.name) {
      result = call_impl(c, D.pointer, args.as_array());
      found = true;
    }
  });

  if (!found) {
    throw std::invalid_argument("Invalid method name");
  }

  return result;
}

} // namespace rpc

#endif // RPC_CALL_H_
