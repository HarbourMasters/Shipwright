#pragma once

#include "logic_expression.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "../SeedContext.h"

namespace Rando {
class Logic;
}

struct LogicExpression::Impl {
    Type type;
    ValueType valueType;
    std::string value;
    std::string operation;
    std::string functionName;
    std::vector<std::shared_ptr<Impl>> children;
    std::unique_ptr<std::string> expressionString;
    Impl* parent = nullptr;
    size_t startIndex = 0;
    size_t endIndex = 0;
    std::weak_ptr<LogicExpression> expression;

    ValueVariant Evaluate(const std::string& path = "0", int depth = 0,
                          const EvaluationCallback& callback = nullptr) const;
    std::string GetTypeString() const;

  private:
    std::string GetExprErrorContext() const;
    ValueVariant EvaluateFunction(const std::string& path = "0", int depth = 0,
                                  const EvaluationCallback& callback = nullptr) const;
    ValueVariant EvaluateEnum() const;
    ValueVariant EvaluateVariable() const;
    ValueVariant EvaluateArithmetic(char op, const std::string& path = "0", int depth = 0,
                                    const EvaluationCallback& callback = nullptr) const;

    using FunctionAdapter = std::function<ValueVariant(const std::vector<std::shared_ptr<Impl>>&, const std::string&,
                                                       int, const EvaluationCallback&)>;

    static std::unordered_map<std::string, FunctionAdapter> functionAdapters;
    static void PopulateFunctionAdapters();
    static std::unordered_map<std::string, int> enumMap;
    static void PopulateEnumMap();
    static std::unordered_map<std::string, FunctionAdapter> variableAdapters;
    static void PopulateVariableAdapters();

    friend bool IsEnumConstant(const std::string& s);

    template <typename T> struct function_traits;

    template <typename R, typename... Args> struct function_traits<R (*)(Args...)> {
        using result_type = R;
        using args_tuple = std::tuple<Args...>;
    };

    template <typename Function> struct function_traits : function_traits<decltype(&Function::operator())> {};

    template <typename C, typename R, typename... Args> struct function_traits<R (C::*)(Args...) const> {
        using result_type = R;
        using args_tuple = std::tuple<Args...>;
    };

    template <typename C, typename R, typename... Args> struct function_traits<R (C::*)(Args...)> {
        using result_type = R;
        using args_tuple = std::tuple<Args...>;
    };

    using ConditionFn = std::function<bool()>;

    template <typename T>
    static T EvaluateArg(const std::shared_ptr<LogicExpression::Impl>& expr, const std::string& path, int depth,
                         const EvaluationCallback& callback) {
        if constexpr (std::is_same_v<T, ConditionFn>) {
            return [&expr = *expr, path, depth, callback]() -> bool {
                return std::get<bool>(expr.Evaluate(path, depth + 1, callback));
            };
        } else {
            try {
                auto value = expr->Evaluate(path, depth + 1, callback);
                return LogicExpression::GetValue<T>(value);
            } catch (const std::bad_variant_access&) {
                throw std::runtime_error("Invalid argument type: " + expr->GetExprErrorContext());
            }
        }
    }

    template <typename Function, typename... Args, size_t... Is>
    static ValueVariant CallFunctionImpl(const std::string& functionName, Function function,
                                         const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                         const std::string& path, int depth, const EvaluationCallback& callback,
                                         std::index_sequence<Is...>) {
        (void)functionName;
        return function(EvaluateArg<Args>(args[Is], path + "." + std::to_string(Is), depth, callback)...);
    }

    template <typename Function, typename... Args>
    static ValueVariant MakeFunctionAdapter(const std::string& functionName, Function function,
                                            const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                            const std::string& path, int depth, const EvaluationCallback& callback) {
        constexpr size_t expectedArgCount = sizeof...(Args);
        if (args.size() != expectedArgCount) {
            throw std::runtime_error("Function " + functionName + " expects " + std::to_string(expectedArgCount) +
                                     " arguments, but got " + std::to_string(args.size()));
        }
        return CallFunctionImpl<Function, Args...>(functionName, function, args, path, depth, callback,
                                                   std::index_sequence_for<Args...>{});
    }

    template <typename Function>
    static FunctionAdapter RegisterFunction(const std::string& functionName, Function function) {
        using traits = function_traits<Function>;
        using ArgsTuple = typename traits::args_tuple;
        return [functionName, function](const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                        const std::string& path, int depth,
                                        const EvaluationCallback& callback) -> ValueVariant {
            return std::apply(
                [&](auto... dummy) {
                    return MakeFunctionAdapter<Function, decltype(dummy)...>(functionName, function, args, path, depth,
                                                                             callback);
                },
                ArgsTuple{});
        };
    }

    template <typename Function, typename Tuple, size_t... Is>
    static ValueVariant CallFunctionWithDefaultsImpl(const std::string& functionName, Function function,
                                                     const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                                     const std::string& path, int depth,
                                                     const EvaluationCallback& callback, Tuple&& defaults,
                                                     std::index_sequence<Is...>) {
        constexpr size_t expectedArgCount = sizeof...(Is);

        if (args.size() > expectedArgCount) {
            throw std::runtime_error("Function " + functionName + " expects up to " + std::to_string(expectedArgCount) +
                                     " arguments, but got " + std::to_string(args.size()));
        }

        return function((Is < args.size() ? EvaluateArg<std::tuple_element_t<Is, std::decay_t<Tuple>>>(
                                                args[Is], path + "." + std::to_string(Is), depth, callback)
                                          : std::get<Is>(defaults))...);
    }

    template <typename Function, typename... Args>
    static FunctionAdapter RegisterFunctionWithDefaults(const std::string& functionName, Function function,
                                                        std::tuple<Args...> defaults) {
        return [functionName, function, defaults](const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                                  const std::string& path, int depth,
                                                  const EvaluationCallback& callback) -> ValueVariant {
            constexpr size_t expectedArgCount = sizeof...(Args);
            return CallFunctionWithDefaultsImpl(functionName, function, args, path, depth, callback, defaults,
                                                std::make_index_sequence<expectedArgCount>{});
        };
    }

    template <typename MemberFunction, typename... Args, size_t... Is>
    static ValueVariant
    CallMemberFunctionImpl(Rando::Logic* logic, const std::string& functionName, MemberFunction function,
                           const std::vector<std::shared_ptr<LogicExpression::Impl>>& args, const std::string& path,
                           int depth, const EvaluationCallback& callback, std::index_sequence<Is...>) {
        (void)functionName;
        return ((*logic).*function)(EvaluateArg<Args>(args[Is], path + "." + std::to_string(Is), depth, callback)...);
    }

    template <typename Function>
    static FunctionAdapter RegisterLogicFunction(const std::string& functionName, Function function) {
        using traits = function_traits<Function>;
        using ArgsTuple = typename traits::args_tuple;
        return [functionName, function](const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                        const std::string& path, int depth,
                                        const EvaluationCallback& callback) -> ValueVariant {
            auto* logic = Rando::Context::GetInstance()->GetLogic().get();
            return std::apply(
                [&](auto... dummy) {
                    constexpr size_t expectedArgCount = sizeof...(dummy);
                    if (args.size() != expectedArgCount) {
                        throw std::runtime_error("Function " + functionName + " expects " +
                                                 std::to_string(expectedArgCount) + " arguments, but got " +
                                                 std::to_string(args.size()));
                    }
                    return CallMemberFunctionImpl<Function, decltype(dummy)...>(
                        logic, functionName, function, args, path, depth, callback,
                        std::index_sequence_for<decltype(dummy)...>{});
                },
                ArgsTuple{});
        };
    }

    template <typename Function, typename Tuple, size_t... Is>
    static ValueVariant
    CallMemberFunctionWithDefaultsImpl(Rando::Logic* logic, const std::string& functionName, Function function,
                                       const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                       const std::string& path, int depth, const EvaluationCallback& callback,
                                       Tuple&& defaults, std::index_sequence<Is...>) {
        constexpr size_t expectedArgCount = sizeof...(Is);

        if (args.size() > expectedArgCount) {
            throw std::runtime_error("Function " + functionName + " expects up to " + std::to_string(expectedArgCount) +
                                     " arguments, but got " + std::to_string(args.size()));
        }

        return ((*logic).*function)((Is < args.size() ? EvaluateArg<std::tuple_element_t<Is, std::decay_t<Tuple>>>(
                                                            args[Is], path + "." + std::to_string(Is), depth, callback)
                                                      : std::get<Is>(defaults))...);
    }

    template <typename Function, typename... Args>
    static FunctionAdapter RegisterLogicFunctionWithDefaults(const std::string& functionName, Function function,
                                                             std::tuple<Args...> defaults) {
        return [functionName, function, defaults](const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                                  const std::string& path, int depth,
                                                  const EvaluationCallback& callback) -> ValueVariant {
            auto* logic = Rando::Context::GetInstance()->GetLogic().get();
            constexpr size_t expectedArgCount = sizeof...(Args);
            return CallMemberFunctionWithDefaultsImpl(logic, functionName, function, args, path, depth, callback,
                                                      defaults, std::make_index_sequence<expectedArgCount>{});
        };
    }

    template <typename T>
    static FunctionAdapter RegisterLogicVariable(const std::string& varName, T Rando::Logic::*var) {
        return [varName, var](const std::vector<std::shared_ptr<LogicExpression::Impl>>& args, const std::string& path,
                              int depth, const EvaluationCallback& callback) -> ValueVariant {
            (void)path;
            (void)depth;
            (void)callback;
            if (!args.empty()) {
                throw std::runtime_error("Variable " + varName + " expects 0 arguments, but got " +
                                         std::to_string(args.size()));
            }
            auto* logic = Rando::Context::GetInstance()->GetLogic().get();
            auto value = (*logic).*var;
            return LogicExpression::GetValue<T>(value);
        };
    }
};
