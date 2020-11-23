#include "fuzzy_native.h"
#include "MatcherBase.h"

using namespace Napi;

#define CHECK(env, cond, msg)                                    \
    if (!(cond))                                                 \
    {                                                            \
        Napi::Error::New(env, msg).ThrowAsJavaScriptException(); \
        return;                                                  \
    }

Matcher::Matcher(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
}

void Matcher::Match(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    CHECK(env, info.Length() > 1, "Wrong Number of arguments");
    CHECK(env, info[0].IsString(), "First argument should be a query string");
    CHECK(env, info[1].IsObject(), "Second argument should be an options object");

    MatcherOptions options;
    Napi::Object options_obj = info[1].As<Napi::Object>();

    options.case_sensitive = options_obj.Get("caseSensitive");
    options.smart_case = options_obj.Get("smartCase");
    options.num_threads = options_obj.Get("numThreads").ToNumber().Uint32Value();
    options.max_results = options_obj.Get("maxResults").ToNumber().Uint32Value();
    options.max_gap = options_obj.Get("maxGap").ToNumber().Uint32Value();
    options.record_match_indexes = options_obj.Get("recordMatchIndexes");
    options.root_path = options_obj.Get("rootPath").ToString();
}

void Matcher::AddCandidates(const Napi::CallbackInfo &info)
{
}

void Matcher::RemoveCandidates(const Napi::CallbackInfo &info)
{
}

void Matcher::SetCandidates(const Napi::CallbackInfo &info)
{
}

Napi::FunctionReference Matcher::constructor;

Napi::Function Matcher::GetClass(Napi::Env env)
{
    Napi::Function func = DefineClass(env, "Matcher", {InstanceMethod("match", &Matcher::Match), InstanceMethod("addCandidates", &Matcher::AddCandidates), InstanceMethod("removeCandidates", &Matcher::RemoveCandidates), InstanceMethod("setCandidates", &Matcher::SetCandidates)});
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
