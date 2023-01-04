/**
 * https://www.boost.org/doc/libs/master/libs/describe/doc/html/describe.html#example_from_json
 */
#ifndef BOOST_FROM_JSON_H_
#define BOOST_FROM_JSON_H_

#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <boost/json.hpp>
#include <string>
#include <vector>


namespace policy {
struct Action {
    size_t id{0};
    explicit Action(size_t id)
        : id{ id } { }

    operator std::string() const { return std::to_string(id); }

    bool operator==(const Action& action) const { return id == action.id;}
    bool operator<(const Action& action) const { return id < action.id; }
};
   BOOST_DESCRIBE_STRUCT(Action, (), (id));
}

namespace QuerySerialize {

template <class T, class D1 = boost::describe::describe_members<T, boost::describe::mod_public | boost::describe::mod_protected>,
    class D2 = boost::describe::describe_members<T, boost::describe::mod_private>,
    class En = std::enable_if_t<boost::mp11::mp_empty<D2>::value && !std::is_union<T>::value>>

void tag_invoke(boost::json::value_from_tag const&, boost::json::value& v, T const& t)
{
    auto& obj = v.emplace_object();

    boost::mp11::mp_for_each<D1>(
        [&](auto D) { obj[D.name] = boost::json::value_from(t.*D.pointer); });
}

template <class T>
void extract(boost::json::object const &obj, char const *name, T &value) {
  value = boost::json::value_to<T>(obj.at(name));
}

template <class T,
          class D1 = boost::describe::describe_members<
              T, boost::describe::mod_public | boost::describe::mod_protected>,
          class D2 = boost::describe::describe_members<
              T, boost::describe::mod_private>,
          class En = std::enable_if_t<boost::mp11::mp_empty<D2>::value &&
                                      !std::is_union<T>::value>>

T tag_invoke(boost::json::value_to_tag<T> const &,
             boost::json::value const &v) {
  auto const &obj = v.as_object();

  T t{};

  boost::mp11::mp_for_each<D1>(
      [&](auto D) { extract(obj, D.name, t.*D.pointer); });

  return t;
}

struct Sample {
  size_t action_idx{0};
  double value{0};

  Sample() = default;

  Sample(std::pair<::policy::Action, double> av)
      : action_idx{ av.first.id }, value{ av.second } {}
};

using Series = std::vector<Sample>;

BOOST_DESCRIBE_STRUCT(Sample, (), (action_idx, value));
//BOOST_DESCRIBE_STRUCT(Series, ());


} // QuerySerialize


#endif // BOOST_FROM_JSON_H_
