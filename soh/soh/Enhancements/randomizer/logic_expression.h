#pragma once
#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <stdexcept>
#include <unordered_map>

#include "logic.h"
extern std::shared_ptr<Rando::Logic> logic;

class LogicExpression {
  public:
    using ValueVariant = std::variant<bool, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t>;
    // Define callback type for evaluation tracing
    using EvaluationCallback = std::function<void(const std::shared_ptr<LogicExpression>&, const std::string&, int,
                                                  const std::string&, const ValueVariant&)>;

    enum class Type { And, Or, Not, Comparison, FunctionCall, Value, Add, Subtract, Multiply, Divide, Ternary };
    enum class ValueType { Identifier, Boolean, Number, Enum };

    static std::shared_ptr<LogicExpression> Parse(const std::string& exprStr);
    std::string ToString() const;
    const std::vector<std::shared_ptr<LogicExpression>>& GetChildren() const;
    Type GetType() const;
    ValueType GetValueType() const;
    std::string GetOperation() const;
    std::string GetFunctionName() const;

    // Add optional callback parameter to Evaluate
    template <typename T> T Evaluate(const EvaluationCallback& callback = nullptr) const {
        return GetValue<T>(impl->Evaluate("0", 0, callback));
    }

    // Helper for producing dependent false in static_assert to show T in error
    template <typename> struct always_false : std::false_type {};

    template <typename T> static T GetValue(const ValueVariant& value) {
        if constexpr (std::is_same_v<T, bool>) {
            // Accept any integral variant alternative as boolean
            return std::visit(
                [](auto v) -> bool {
                    using V = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<V, bool>) {
                        return v;
                    } else if constexpr (std::is_integral_v<V>) {
                        return v != 0;
                    } else {
                        throw std::bad_variant_access();
                    }
                },
                value);
        } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
            // Accept any integral alternative and cast to the requested integral type
            return std::visit(
                [](auto v) -> T {
                    using V = std::decay_t<decltype(v)>;
                    if constexpr (std::is_integral_v<V>) {
                        return static_cast<T>(v);
                    } else {
                        throw std::bad_variant_access();
                    }
                },
                value);
        } else if constexpr (std::is_enum_v<T>) {
            // Enums are represented as integers in the variant; convert via int32_t
            return static_cast<T>(GetValue<int32_t>(value));
        } else if constexpr (std::is_same_v<T, ValueVariant>) {
            return value;
        } else {
            static_assert(always_false<T>::value, "Unsupported type for LogicExpression::GetValue");
        }
    }

  private:
    struct Impl {
        Type type;
        ValueType valueType;
        std::string value;
        std::string operation;
        std::string functionName;
        std::vector<std::shared_ptr<Impl>> children;
        std::unique_ptr<std::string> expressionString;
        Impl* parent;
        size_t startIndex;
        size_t endIndex;
        std::shared_ptr<LogicExpression> expression;

        // Modified to accept path and depth
        ValueVariant Evaluate(const std::string& path = "0", int depth = 0,
                              const EvaluationCallback& callback = nullptr) const;

        // Helper to get a string representation of the type
        std::string GetTypeString() const;

      private:
        std::string GetExprErrorContext() const;
        // Updated to pass callback to children
        ValueVariant EvaluateFunction(const std::string& path = "0", int depth = 0,
                                      const EvaluationCallback& callback = nullptr) const;
        ValueVariant EvaluateEnum() const;
        ValueVariant EvaluateVariable() const;
        // Updated to pass callback to children
        ValueVariant EvaluateArithmetic(char op, const std::string& path = "0", int depth = 0,
                                        const EvaluationCallback& callback = nullptr) const;

        using FunctionAdapter = std::function<ValueVariant(const std::vector<std::shared_ptr<Impl>>&,
                                                           const std::string&, int, const EvaluationCallback&)>;

        static std::unordered_map<std::string, FunctionAdapter> functionAdapters;
        static void PopulateFunctionAdapters();
        static std::unordered_map<std::string, int> enumMap;
        static void PopulateEnumMap();
        static std::unordered_map<std::string, FunctionAdapter> variableAdapters;
        static void PopulateVariableAdapters();

        friend bool IsEnumConstant(const std::string& s);

        // Define FunctionAdapter as a unified interface for wrapping functions.
        // It represents a function that takes a vector of LogicExpression pointers
        // and returns a ValueVariant.
        // using FunctionAdapter = std::function<ValueVariant(const
        // std::vector<std::unique_ptr<LogicExpression::Impl>>&)>;

        // Below are helper templates and functions that enable dynamic registration and invocation
        // of functions using the "Template Magic" method. These utilities deduce function parameter types,
        // convert LogicExpression arguments to the expected types, check argument counts, and finally call the
        // functions. The idea is to expose a unified FunctionAdapter interface that wraps any function with varying
        // argument types.

        // Helper: function_traits for lambdas and function pointers.
        // This trait extracts the return type and parameter types for a function pointer or lambda.
        // It supports both function pointers (first specialization) and lambda objects (via the deduced operator()).
        template <typename T> struct function_traits;

        template <typename R, typename... Args> struct function_traits<R (*)(Args...)> {
            using result_type = R;                  // The return type of the function.
            using args_tuple = std::tuple<Args...>; // A tuple of the function's parameter types.
        };

        // For lambdas and other callable objects, we deduce the type from the operator() member.
        template <typename Function> struct function_traits : function_traits<decltype(&Function::operator())> {};

        // Specialization for const member function pointers (for lambdas).
        template <typename C, typename R, typename... Args> struct function_traits<R (C::*)(Args...) const> {
            using result_type = R;                  // The return type.
            using args_tuple = std::tuple<Args...>; // Tuple of argument types.
        };

        // --- New specialization for non-const member function pointers ---
        template <typename C, typename R, typename... Args> struct function_traits<R (C::*)(Args...)> {
            using result_type = R;                  // The return type.
            using args_tuple = std::tuple<Args...>; // Tuple of argument types.
        };

        using ConditionFn = std::function<bool()>;

        // Helper to conditionally convert an LogicExpression argument.
        // If T is the expected function type (e.g. ConditionFn), this helper wraps the expression into a
        // callable lambda. Otherwise, it evaluates the expression immediately to convert it into a bool, int, or enum
        // value. Template parameter T represents the expected type of the argument for the function
        // being registered.
        template <typename T>
        static T EvaluateArg(const std::shared_ptr<LogicExpression::Impl>& expr, const std::string& path, int depth,
                             const EvaluationCallback& callback) {
            if constexpr (std::is_same_v<T, ConditionFn>) {
                // Wrap the expression in a lambda to delay evaluation until the function is called.
                return [&expr = *expr, path, depth, callback]() -> bool {
                    return std::get<bool>(expr.Evaluate(path, depth + 1, callback));
                };
            } else {
                // For value types, evaluate the expression immediately.
                try {
                    auto value = expr->Evaluate(path, depth + 1, callback);
                    return GetValue<T>(value);
                } catch (const std::bad_variant_access&) {
                    throw std::runtime_error("Invalid argument type: " + expr->GetExprErrorContext());
                }
            }
        }

        // Helper to call the function with extracted (converted) arguments.
        // This function unpacks the tuple of expected argument types using an index sequence,
        // converts each provided LogicExpression (using EvaluateArg) to the required type, and then
        // calls 'function'. functionName is used for error reporting.
        template <typename Function, typename... Args, size_t... Is>
        static ValueVariant CallFunctionImpl(const std::string& functionName, Function function,
                                             const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                             const std::string& path, int depth, const EvaluationCallback& callback,
                                             std::index_sequence<Is...>) {
            // Each args[Is] is converted to its expected type using EvaluateArg<Args> and passed to
            // 'function'.
            return function(EvaluateArg<Args>(args[Is], path + "." + std::to_string(Is), depth, callback)...);
        }

        // Wraps a function into a FunctionAdapter by ensuring the correct number of arguments are provided
        // and by utilizing CallFunctionImpl to perform the actual function call with type-checked
        // parameters. It throws an error if the number of provided arguments does not match the expected count.
        // Template parameter Function is the original function type, and Args are its parameter types.
        template <typename Function, typename... Args>
        static ValueVariant MakeFunctionAdapter(const std::string& functionName, Function function,
                                                const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                                const std::string& path, int depth,
                                                const EvaluationCallback& callback) {
            constexpr size_t expectedArgCount = sizeof...(Args);
            if (args.size() != expectedArgCount) {
                throw std::runtime_error("Function " + functionName + " expects " + std::to_string(expectedArgCount) +
                                         " arguments, but got " + std::to_string(args.size()));
            }
            return CallFunctionImpl<Function, Args...>(functionName, function, args, path, depth, callback,
                                                       std::index_sequence_for<Args...>{});
        }

        // Wrapper that deduces parameter types from the function and returns a FunctionAdapter.
        // This adapter wraps the original function so that it can be called with a vector of LogicExpression pointers.
        // It performs deducing of the function's expected parameter types via function_traits,
        // and then uses MakeFunctionAdapter to perform argument validation and conversion before invocation.
        template <typename Function>
        static FunctionAdapter RegisterFunction(const std::string& functionName, Function function) {
            using traits = function_traits<Function>;
            using ArgsTuple = typename traits::args_tuple;
            return [functionName, function](const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                            const std::string& path, int depth,
                                            const EvaluationCallback& callback) -> ValueVariant {
                return std::apply(
                    [&](auto... dummy) {
                        return MakeFunctionAdapter<Function, decltype(dummy)...>(functionName, function, args, path,
                                                                                 depth, callback);
                    },
                    ArgsTuple{}); // Unpacks the expected parameter types.
            };
        }

        // Helper to call member function with default parameters
        // This function evaluates provided arguments and fills missing ones with defaults.
        template <typename Function, typename Tuple, size_t... Is>
        static ValueVariant
        CallFunctionWithDefaultsImpl(const std::string& functionName, Function function,
                                     const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                     const std::string& path, int depth, const EvaluationCallback& callback,
                                     Tuple&& defaults, std::index_sequence<Is...>) {
            constexpr size_t expectedArgCount = sizeof...(Is);

            // Ensure the number of provided arguments does not exceed the expected count
            if (args.size() > expectedArgCount) {
                throw std::runtime_error("Function " + functionName + " expects up to " +
                                         std::to_string(expectedArgCount) + " arguments, but got " +
                                         std::to_string(args.size()));
            }

            // Evaluate provided arguments and fill missing ones with defaults
            return function((Is < args.size() ? EvaluateArg<std::tuple_element_t<Is, std::decay_t<Tuple>>>(
                                                    args[Is], path + "." + std::to_string(Is), depth, callback)
                                              : std::get<Is>(defaults))...);
        }

        // Wrapper for member functions with default parameters
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

        // Helper to call pointer-to-member function on "logic" with extracted arguments.
        template <typename MemberFunction, typename... Args, size_t... Is>
        static ValueVariant CallMemberFunctionImpl(const std::string& functionName, MemberFunction function,
                                                   const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                                   const std::string& path, int depth,
                                                   const EvaluationCallback& callback, std::index_sequence<Is...>) {
            return ((*logic).*
                    function)(EvaluateArg<Args>(args[Is], path + "." + std::to_string(Is), depth, callback)...);
        }

        // Implementation of RegisterLogicFunction that wraps a pointer-to-member function from Logic.
        template <typename Function>
        static FunctionAdapter RegisterLogicFunction(const std::string& functionName, Function function) {
            using traits = function_traits<Function>;
            using ArgsTuple = typename traits::args_tuple;
            return [functionName, function](const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                            const std::string& path, int depth,
                                            const EvaluationCallback& callback) -> ValueVariant {
                return std::apply(
                    [&](auto... dummy) {
                        constexpr size_t expectedArgCount = sizeof...(dummy);
                        if (args.size() != expectedArgCount) {
                            throw std::runtime_error("Function " + functionName + " expects " +
                                                     std::to_string(expectedArgCount) + " arguments, but got " +
                                                     std::to_string(args.size()));
                        }
                        return CallMemberFunctionImpl<Function, decltype(dummy)...>(
                            functionName, function, args, path, depth, callback,
                            std::index_sequence_for<decltype(dummy)...>{});
                    },
                    ArgsTuple{});
            };
        }

        // Helper to call member function with default parameters
        // This function evaluates provided arguments and fills missing ones with defaults.
        template <typename Function, typename Tuple, size_t... Is>
        static ValueVariant
        CallMemberFunctionWithDefaultsImpl(const std::string& functionName, Function function,
                                           const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                           const std::string& path, int depth, const EvaluationCallback& callback,
                                           Tuple&& defaults, std::index_sequence<Is...>) {
            constexpr size_t expectedArgCount = sizeof...(Is);

            // Ensure the number of provided arguments does not exceed the expected count
            if (args.size() > expectedArgCount) {
                throw std::runtime_error("Function " + functionName + " expects up to " +
                                         std::to_string(expectedArgCount) + " arguments, but got " +
                                         std::to_string(args.size()));
            }

            // Evaluate provided arguments and fill missing ones with defaults
            return ((*logic).*function)((Is < args.size()
                                             ? EvaluateArg<std::tuple_element_t<Is, std::decay_t<Tuple>>>(
                                                   args[Is], path + "." + std::to_string(Is), depth, callback)
                                             : std::get<Is>(defaults))...);
        }

        // Wrapper for member functions with default parameters
        template <typename Function, typename... Args>
        static FunctionAdapter RegisterLogicFunctionWithDefaults(const std::string& functionName, Function function,
                                                                 std::tuple<Args...> defaults) {
            return [functionName, function, defaults](const std::vector<std::shared_ptr<LogicExpression::Impl>>& args,
                                                      const std::string& path, int depth,
                                                      const EvaluationCallback& callback) -> ValueVariant {
                constexpr size_t expectedArgCount = sizeof...(Args);
                return CallMemberFunctionWithDefaultsImpl(functionName, function, args, path, depth, callback, defaults,
                                                          std::make_index_sequence<expectedArgCount>{});
            };
        }

        // Add this function in the Template Magic section to implement variable registration.
        template <typename T>
        static FunctionAdapter RegisterLogicVariable(const std::string& varName, T Rando::Logic::*var) {
            return
                [varName, var](const std::vector<std::shared_ptr<LogicExpression::Impl>>& args, const std::string& path,
                               int depth, const EvaluationCallback& callback) -> ValueVariant {
                    if (!args.empty()) {
                        throw std::runtime_error("Variable " + varName + " expects 0 arguments, but got " +
                                                 std::to_string(args.size()));
                    }
                    auto value = (*logic).*var;
                    return GetValue<T>(value);
                };
        }
    };

    std::shared_ptr<Impl> impl;
    std::vector<std::shared_ptr<LogicExpression>> children;

    friend class Parser;
    friend bool IsEnumConstant(const std::string& s);
};

struct ExpressionEvaluation {
    std::shared_ptr<LogicExpression> Expression;
    int Depth;
    std::string Type;
    LogicExpression::ValueVariant Result;
    std::vector<ExpressionEvaluation> Children;
};

ExpressionEvaluation EvaluateExpression(std::string condition);

ExpressionEvaluation EvaluateExpression(std::shared_ptr<LogicExpression> expression);

std::string ToString(const LogicExpression::ValueVariant& value);
