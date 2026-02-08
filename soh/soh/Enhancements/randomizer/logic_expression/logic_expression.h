#pragma once

#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

class LogicExpression {
  public:
    using ValueVariant = std::variant<bool, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t>;
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

    ValueVariant EvaluateVariant(const EvaluationCallback& callback = nullptr) const;

    template <typename T> T Evaluate(const EvaluationCallback& callback = nullptr) const {
        return GetValue<T>(EvaluateVariant(callback));
    }

    template <typename> struct always_false : std::false_type {};

    template <typename T> static T GetValue(const ValueVariant& value) {
        if constexpr (std::is_same_v<T, bool>) {
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
            return static_cast<T>(GetValue<int32_t>(value));
        } else if constexpr (std::is_same_v<T, ValueVariant>) {
            return value;
        } else {
            static_assert(always_false<T>::value, "Unsupported type for LogicExpression::GetValue");
        }
    }

  private:
    struct Impl;

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
