#include "methodcall.h"

void MethodCall::success(){
    this->mResult.emplace_back(std::monostate());
    this->mSuccess = true;
}

void MethodCall::success(std::any result...){
    this->mSuccess = true;
    this->mResult = { std::move(result) };
}

void MethodCall::error(const std::string& error){
    this->mResult.emplace_back(error);
    this->mSuccess = false;
}