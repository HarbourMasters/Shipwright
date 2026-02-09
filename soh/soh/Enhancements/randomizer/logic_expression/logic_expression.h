#pragma once

#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

/**
 * @brief Parsed, typed representation of a logic expression used by the randomizer.
 *
 * A `LogicExpression` is produced by `Parse()` and can be evaluated to a `ValueVariant` via
 * `EvaluateVariant()` (or a concrete type via `Evaluate<T>()`).
 *
 * Expressions form a tree: each node has a `Type` and may have zero or more child expressions
 * accessible through `GetChildren()`.
 */
class LogicExpression {
  public:
    /**
     * @brief The value domain for expression evaluation.
     *
     * Evaluation returns either boolean or an integral type. Callers can convert to a desired type
     * using `GetValue<T>()`.
     */
    using ValueVariant = std::variant<bool, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t>;

    /**
     * @brief Optional callback invoked during evaluation.
     *
     * The evaluator may call this for each visited node, enabling tracing/debugging.
     *
     * Parameters:
     * - `std::shared_ptr<LogicExpression>`: the node being evaluated
     * - `const std::string&`: a textual type/label for the node (implementation-defined)
     * - `int`: nesting depth
     * - `const std::string&`: node-specific details (implementation-defined)
     * - `const ValueVariant&`: the computed result for the node
     */
    using EvaluationCallback = std::function<void(const std::shared_ptr<LogicExpression>&, const std::string&, int,
                                                  const std::string&, const ValueVariant&)>;

    /**
     * @brief The syntactic/semantic kind of an expression node.
     */
    enum class Type { And, Or, Not, Comparison, FunctionCall, Value, Add, Subtract, Multiply, Divide, Ternary };

    /**
     * @brief The kind of literal/value stored in a `Type::Value` node.
     */
    enum class ValueType { Identifier, Boolean, Number, Enum };

    /**
     * @brief Parse a string into an expression tree.
     * @param exprStr Source expression.
     * @return Root node of the expression tree.
     */
    static std::shared_ptr<LogicExpression> Parse(const std::string& exprStr);

    /**
     * @brief Convert the expression tree back into a string representation.
     */
    std::string ToString() const;

    /**
     * @brief Child nodes of this expression.
     */
    const std::vector<std::shared_ptr<LogicExpression>>& GetChildren() const noexcept;

    /**
     * @brief Node type.
     */
    Type GetType() const noexcept;

    /**
     * @brief Value type for `Type::Value` nodes.
     *
     * For non-`Type::Value` nodes, the returned value is implementation-defined.
     */
    ValueType GetValueType() const noexcept;

    /**
     * @brief Operation token for operator/comparison nodes.
     *
     * Examples (implementation-defined): `"=="`, `">"`, `"&&"`, `"+"`.
     */
    const std::string& GetOperation() const noexcept;

    /**
     * @brief Function name for `Type::FunctionCall` nodes.
     */
    const std::string& GetFunctionName() const noexcept;

    /**
     * @brief Evaluate the expression and return the raw `ValueVariant`.
     * @param callback Optional tracing callback invoked during evaluation.
     */
    ValueVariant EvaluateVariant(const EvaluationCallback& callback = nullptr) const;

    /**
     * @brief Evaluate the expression and convert to `T`.
     *
     * Supported `T`:
     * - `bool`: integral values convert with `!= 0`
     * - integral types: `static_cast<T>(value)`
     * - enum types: converted via `int32_t`
     * - `ValueVariant`: returned as-is
     */
    template <typename T> T Evaluate(const EvaluationCallback& callback = nullptr) const {
        return GetValue<T>(EvaluateVariant(callback));
    }

    template <typename> struct always_false : std::false_type {};

    /**
     * @brief Convert an evaluated `ValueVariant` to a requested type.
     *
     * @throws std::bad_variant_access If conversion is not possible.
     */
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
    /** @brief The node that was evaluated. */
    std::shared_ptr<LogicExpression> Expression;
    /** @brief Nesting depth of this node in the evaluation tree. */
    int Depth;
    /** @brief A textual type/label for this evaluation entry (implementation-defined). */
    std::string Type;
    /** @brief Result of evaluating `Expression`. */
    LogicExpression::ValueVariant Result;
    /** @brief Child evaluations (typically matching `Expression->GetChildren()`). */
    std::vector<ExpressionEvaluation> Children;
};

/**
 * @brief Parse and evaluate a condition string, producing a structured trace.
 */
ExpressionEvaluation EvaluateExpression(const std::string& condition);

/**
 * @brief Evaluate an already-parsed expression, producing a structured trace.
 */
ExpressionEvaluation EvaluateExpression(std::shared_ptr<LogicExpression> expression);

/**
 * @brief Convert an evaluated `ValueVariant` to a human-readable string.
 */
std::string ToString(const LogicExpression::ValueVariant& value);
