#include "fuzzy-native.h"

using namespace Napi;

#define CHECK(env, cond, msg)                                    \
    if (!(cond))                                                 \
    {                                                            \
        Napi::Error::New(env, msg).ThrowAsJavaScriptException(); \
        return;                                                  \
    }

#define CHECK_OR_RETURN(env, cond, msg, retval)                  \
    if (!(cond))                                                 \
    {                                                            \
        Napi::Error::New(env, msg).ThrowAsJavaScriptException(); \
        return retval;                                           \
    }

Matcher::Matcher(const Napi::CallbackInfo &info) : ObjectWrap<Matcher>(info)
{
  if (info.Length() > 0) {
    AddCandidates(info);
  }
}

Napi::Value Matcher::Match(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    CHECK_OR_RETURN(env, info.Length() >= 1, "Wrong Number of arguments", env.Null());
    CHECK_OR_RETURN(env, info[0].IsString(), "First argument should be a query string", env.Null());
    auto query = info[0].As<Napi::String>().Utf8Value();
    MatcherOptions options;
    if (info.Length() > 1) {
      CHECK_OR_RETURN(env, info[1].IsObject(), "Second argument should be an options object", env.Null());
      Napi::Object options_obj = info[1].As<Napi::Object>();

      options.case_sensitive = options_obj.Get("caseSensitive").ToBoolean();
      options.smart_case = options_obj.Get("smartCase").ToBoolean();
      options.num_threads = options_obj.Get("numThreads").ToNumber().Uint32Value();
      options.max_results = options_obj.Get("maxResults").ToNumber().Uint32Value();
      options.max_gap = options_obj.Get("maxGap").ToNumber().Uint32Value();
      options.record_match_indexes = options_obj.Get("recordMatchIndexes");
      options.fuzzaldrin = options_obj.Get("algorithm").ToString().Utf8Value() == "fuzzaldrin";
      options.root_path = options_obj.Get("rootPath").ToString();
    }
  auto idKey = Napi::String::New(env, "id");
  auto valueKey = Napi::String::New(env, "value");
  auto scoreKey = Napi::String::New(env, "score");
  auto matchIndexesKey = Napi::String::New(env, "matchIndexes");

  std::vector<MatchResult> matches = this->_impl.findMatches(query, options);

  auto result = Napi::Array::New(env);
  size_t result_count = 0;

  for (const auto &match : matches) {
    auto obj = Napi::Object::New(env);
    obj.Set(idKey, Napi::Number::New(env, match.id));
    obj.Set(scoreKey, Napi::Number::New(env, match.score));
    obj.Set(valueKey, Napi::String::New(env, *match.value));

    if (match.matchIndexes != nullptr) {
      Napi::Array array = Napi::Array::New(env, match.matchIndexes->size());
      for (size_t i = 0; i < array.Length(); i++) {
        array.Set(i, Napi::Number::New(env, match.matchIndexes->at(i)));
      }
      obj.Set(matchIndexesKey, array);
    }
    result.Set(result_count++, obj);
  }
  return result;
}

void Matcher::AddCandidates(const Napi::CallbackInfo &info)
{
  auto env = info.Env();
  if (info.Length() > 0) {
    CHECK(env, info[0].IsArray(), "Expected an array of unsigned 32-bit integer ids as the first argument");
    CHECK(env, info[1].IsArray(), "Expected an array of strings as the second argument");

    auto ids = info[0].As<Napi::Array>();
    auto values = info[1].As<Napi::Array>();

    CHECK(env, ids.Length() == values.Length(), "Expected ids array and values array to have the same length");

    // Create a random permutation so that candidates are shuffled.
    std::vector<size_t> indexes(ids.Length());
    for (size_t i = 0; i < indexes.size(); i++)
    {
      indexes[i] = i;
      if (i > 0)
      {
        std::swap(indexes[rand() % i], indexes[i]);
      }
    }
    this->_impl.reserve(this->_impl.size() + ids.Length());
    for (auto i : indexes) {
      auto id_value = ids.Get(i);
      double raw = id_value.As<Napi::Number>().DoubleValue();
      CHECK(env, raw >= 0 && raw == std::floor(raw), "Expected first array to only contain unsigned 32-bit integer ids");
      uint32_t id = id_value.As<Napi::Number>().Uint32Value();
      auto value = values.Get(i).As<Napi::String>();
      this->_impl.addCandidate(id, value.Utf8Value());
    }
  }
}

void Matcher::RemoveCandidates(const Napi::CallbackInfo &info)
{
  auto env = info.Env();
  if (info.Length() > 0) {
    CHECK(env, info[0].IsArray(), "Expected an array of unsigned 32-bit integer ids");
    auto ids = info[0].As<Napi::Array>();
    for (size_t i = 0; i < ids.Length(); i++) {
      auto id_value = ids.Get(i);
      double raw = id_value.As<Napi::Number>().DoubleValue();
      CHECK(env, raw >= 0 && raw == std::floor(raw), "Expected array to only contain unsigned 32-bit integer ids");
      uint32_t id = id_value.As<Napi::Number>().Uint32Value();
      this->_impl.removeCandidate(id);
    }
  }
}

void Matcher::SetCandidates(const Napi::CallbackInfo &info)
{
  this->_impl.clear();
  AddCandidates(info);
}

Napi::FunctionReference Matcher::constructor;

Napi::Function Matcher::GetClass(Napi::Env env) {
  Napi::Function func = DefineClass(
      env, "Matcher",
      {InstanceMethod("match", &Matcher::Match),
       InstanceMethod("addCandidates", &Matcher::AddCandidates),
       InstanceMethod("removeCandidates", &Matcher::RemoveCandidates),
       InstanceMethod("setCandidates", &Matcher::SetCandidates)});
  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  return func;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String matcher = Napi::String::New(env, "Matcher");
    exports.Set(matcher, Matcher::GetClass(env));

    return exports;
}

NODE_API_MODULE(addon, Init)
