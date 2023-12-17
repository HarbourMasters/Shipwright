#pragma once

#include <string>
#include <vector>
#include <variant>
#include <any>
#include "soh/Enhancements/scripting-layer/hosts/hostapi.h"
#include "soh/Enhancements/scripting-layer/exceptions/hostapiexception.h"

class MethodCall;

typedef void (*FunctionPtr)(uintptr_t context, MethodCall*);

enum class BindingType {
    KFunction,
    KField
};

struct GameBinding {
    BindingType type;
    std::variant<FunctionPtr, std::any> binding;
    std::string module = ROOT_MODULE;
};

class MethodCall {
    HostAPI* mHost;
    uintptr_t mContext;
    std::vector<std::any> mResult;
    bool mSuccess = false;
public:
    explicit MethodCall(HostAPI* host, uintptr_t context) : mHost(host), mContext(context) {}

    HostAPI* GetHost(){
        return this->mHost;
    }

    template<typename T>
    T GetArgument(int index){
        try {
            return std::any_cast<T>(this->mHost->GetArgument(index, this->mContext));
        } catch (const std::bad_any_cast& e) {
            throw HostAPIException("Argument " + std::to_string(index) + " is not of type " + typeid(T).name());
        }
    }

    std::any RawArgument(int index){
        return this->mHost->GetArgument(index, this->mContext);
    }

    size_t ArgumentCount(){
        return this->mHost->GetArgumentCount(this->mContext);
    }

    // Response methods
    void success();
    void success(std::any result...);
    void error(const std::string& error);

    [[nodiscard]] bool succeed() const {
        return this->mSuccess;
    }

    [[nodiscard]] std::vector<std::any> result() const {
        return this->mResult;
    }
};