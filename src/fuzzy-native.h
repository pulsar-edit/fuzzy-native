#pragma once

#include <napi.h>

class Matcher : public Napi::ObjectWrap<Matcher>
{
private:
    static Napi::FunctionReference constructor;

public:
    Matcher(const Napi::CallbackInfo &info);
    void Create(const Napi::CallbackInfo &info);
    void Match(const Napi::CallbackInfo &info);
    void AddCandidates(const Napi::CallbackInfo &info);
    void RemoveCandidates(const Napi::CallbackInfo &info);
    void SetCandidates(const Napi::CallbackInfo &info);

    static Napi::Function GetClass(Napi::Env);
};
