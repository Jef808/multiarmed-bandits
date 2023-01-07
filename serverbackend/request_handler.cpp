#include "serverbackend/request_handler.h"
//#include "serverbackend/boost_serializer_helper.h"
#include "environments/multiarmed_bandits/multiarmed_bandits.h"
#include "policies/policies.h"
#include "policies/agent.h"

#include <boost/json.hpp>
#include <boost/json/src.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <variant>

namespace json = boost::json;

namespace policy {

void tag_invoke( json::value_from_tag, json::value& jv, Sample const& s) {
  jv = {
    { "action" , s.action.id },
    { "step" , s.step },
    { "value" , s.value }
  };
}

}  // namespace policy

namespace {

inline std::string json_serialize(const std::vector<::policy::Sample>& series) {
  return json::serialize(json::value_from( series ));
}

}

namespace Query {

json::value
parse_string( std::string_view sv )
{
    json::stream_parser p;
    json::error_code ec;
    do
    {
        char buf[4096];
        const auto nread = std::max( sizeof(buf) / sizeof(char), sv.size() );
        std::copy_n(sv.substr(0, nread).data(), nread, buf);
        sv.remove_prefix(nread);
        p.write( buf, nread, ec );
    }
    while( ! sv.empty() );
    if( ec )
        return nullptr;
    p.finish( ec );
    if( ec )
        return nullptr;
    return p.release();
}

void
pretty( std::ostream& out, json::value const& jv, std::string indent = "")
{
    switch(jv.kind())
    {
    case json::kind::object:
    {
        out << "{\n";
        indent.append(4, ' ');
        auto const& obj = jv.get_object();
        if(! obj.empty())
        {
            auto it = obj.begin();
            for(;;)
            {
                out << indent << json::serialize(it->key()) << " : ";
                pretty(out, it->value(), indent);
                if(++it == obj.end())
                    break;
                out << ",\n";
            }
        }
        out << "\n";
        indent.resize(indent.size() - 4);
        out << indent << "}";
        break;
    }

    case json::kind::array:
    {
        out << "[\n";
        indent.append(4, ' ');
        auto const& arr = jv.get_array();
        if(! arr.empty())
        {
            auto it = arr.begin();
            for(;;)
            {
                out << indent;
                pretty( out, *it, indent);
                if(++it == arr.end())
                    break;
                out << ",\n";
            }
        }
        out << "\n";
        indent.resize(indent.size() - 4);
        out << indent << "]";
        break;
    }

    case json::kind::string:
    {
        out << json::serialize(jv.get_string());
        break;
    }

    case json::kind::uint64:
        out << jv.get_uint64();
        break;

    case json::kind::int64:
        out << jv.get_int64();
        break;

    case json::kind::double_:
        out << jv.get_double();
        break;

    case json::kind::bool_:
        if(jv.get_bool())
            out << "true";
        else
            out << "false";
        break;

    case json::kind::null:
        out << "null";
        break;
    }

    if(indent.empty())
        out << "\n";
}

auto GetModelVariant(json::value modelName, json::value modelParameters) {
    using Variant = std::variant<env::MultiArmedBandit>;
    if (modelName.get_string() == "mab") {
        auto nb_of_arms = static_cast<size_t>(modelParameters.get_object()["nbOfArms"].get_int64());
        return Variant{env::MultiArmedBandit{nb_of_arms}};
    }

    throw std::runtime_error("Unknown model name");
}

auto GetPolicyVariant(json::value policyName, json::value policyParameters) {
    using Variant = std::variant<policy::EpsilonGreedy, policy::Ucb>;
    if (policyName.get_string() == "epsilonGreedy") {
        auto epsilon = policyParameters.get_object()["epsilon"].get_double();
        return Variant{policy::EpsilonGreedy{epsilon}};
    }
    else if (policyName.get_string() == "ucb") {
        auto exploration = policyParameters.get_object()["exploration"].get_double();
        return Variant{policy::Ucb{exploration}};
    }

    throw std::runtime_error("Unknown policy name");
}

auto GetNbSteps(json::value options) {
    return static_cast<size_t>(options.get_object()["nbOfSteps"].get_int64());
}

template<typename OutputIter>
struct Visitor {

    Visitor(size_t nb_samples, OutputIter out)
    : nb_samples{nb_samples}, out{out} {}

    void operator()(auto& m, auto& p) {
        policy::Agent{m, p}.run(nb_samples, out);
    }

    size_t nb_samples;
    OutputIter out;
};

std::pair<bool, std::string> RequestHandler::operator()(const std::string& request) {
  static constexpr size_t indent_size = 2;

    if (request == "ping") {
      return { true, "pong" };
    }

    try
    {
        auto req = json::parse(request).as_object();
        std::ostringstream oss;

        std::cout << "Received request ";

        pretty(std::cout, req, std::string(indent_size, ' '));
        std::cout << std::endl;

        const auto model = GetModelVariant(req["modelName"], req["modelParameters"]);
        const auto policy = GetPolicyVariant(req["policyName"], req["policyParameters"]);
        const auto nb_steps = GetNbSteps(req["options"]);
        std::vector<::policy::Sample> buffer;

        auto out = std::back_inserter(buffer);

        try
        {
          std::visit(Visitor{nb_steps, out}, model, policy);
          for (const auto& sample : buffer) {
            std::cout << "action: " << sample.action.id << ",\nstep: " << sample.step << ",\nvalue: " << sample.value << std::endl;
          }
          return { true, json_serialize(buffer) };
          //return { true, "" };
        }
        catch (std::exception const& e)
        {
          std::cerr << "Error while running policy: "
                    << e.what() << std::endl;
          return { false, "" };
        }
        try
        {
          //std::string result = json::serialize(json::value_from(buffer));
          //return std::make_pair( true, std::move(result) );
        }
        catch (std::exception const& e)
        {
          std::cerr << "Error while serializing results: "
              << e.what() << std::endl;
          return { false, "" };
        }
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error while parsing request: "
                  << e.what() << std::endl;
        return { false, "" };
    }
}

} // namespace Query
