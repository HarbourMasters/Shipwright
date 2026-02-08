#include "logic_expression_impl.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <variant>

std::string LogicExpression::Impl::GetTypeString() const {
    switch (type) {
        case Type::Value:
            switch (valueType) {
                case ValueType::Boolean:
                    return "Boolean";
                case ValueType::Number:
                    return "Number";
                case ValueType::Enum:
                    return "Enum";
                case ValueType::Identifier:
                    return "Variable";
                default:
                    return "Unknown Value";
            }
        case Type::FunctionCall:
            return "Function: " + functionName;
        case Type::Not:
            return "Not";
        case Type::And:
            return "And";
        case Type::Or:
            return "Or";
        case Type::Comparison:
            return "Comparison: " + operation;
        case Type::Add:
            return "Add";
        case Type::Subtract:
            return "Subtract";
        case Type::Multiply:
            return "Multiply";
        case Type::Divide:
            return "Divide";
        case Type::Ternary:
            return "Ternary";
        default:
            return "Unknown";
    }
}

std::string LogicExpression::Impl::GetExprErrorContext() const {
    size_t contextLen = 10;
    const Impl* root = this;
    while (root->parent)
        root = root->parent;
    const std::string& input = *root->expressionString;
    size_t exprStart = startIndex;
    size_t exprEnd = endIndex;
    size_t globalStart = (exprStart >= contextLen) ? exprStart - contextLen : 0;
    size_t globalEnd = std::min(input.size(), exprEnd + contextLen);
    std::string contextLine = input.substr(globalStart, globalEnd - globalStart);
    size_t pointerOffset = exprStart - globalStart;
    size_t pointerLength = (exprEnd > exprStart) ? exprEnd - exprStart : 1;
    std::string pointerLine(pointerOffset, ' ');
    pointerLine.append(std::string(pointerLength, '^'));
    return "\n" + contextLine + "\n" + pointerLine;
}

LogicExpression::ValueVariant LogicExpression::Impl::EvaluateArithmetic(char op, const std::string& path, int depth,
                                                                        const EvaluationCallback& callback) const {
    if (children.empty()) {
        throw std::runtime_error("Arithmetic node has no children" + GetExprErrorContext());
    }

    auto applyOp = [&](auto a, auto b) -> ValueVariant {
        using A = std::decay_t<decltype(a)>;
        using B = std::decay_t<decltype(b)>;

        if constexpr (std::is_integral_v<A> && std::is_integral_v<B>) {
            if (op == '+' || op == '*') {
                if constexpr (std::is_unsigned_v<A> && std::is_unsigned_v<B>) {
                    uint64_t l = static_cast<uint64_t>(a);
                    uint64_t r = static_cast<uint64_t>(b);
                    uint64_t out = (op == '+') ? (l + r) : (l * r);
                    return static_cast<uint32_t>(out);
                } else {
                    int64_t l = static_cast<int64_t>(a);
                    int64_t r = static_cast<int64_t>(b);
                    int64_t out = (op == '+') ? (l + r) : (l * r);
                    return static_cast<int32_t>(out);
                }
            }

            if (op == '-' || op == '/') {
                int64_t l = static_cast<int64_t>(a);
                int64_t r = static_cast<int64_t>(b);
                int64_t out = 0;
                if (op == '-') {
                    out = l - r;
                } else {
                    if (r == 0) {
                        throw std::runtime_error("Division by zero" + GetExprErrorContext());
                    }
                    out = l / r;
                }
                return static_cast<int32_t>(out);
            }

            throw std::runtime_error("Unknown arithmetic op" + GetExprErrorContext());
        }

        throw std::runtime_error("Invalid types for arithmetic (must be integral, not bool)" + GetExprErrorContext());
    };

    ValueVariant accum = children[0]->Evaluate(path + ".0", depth + 1, callback);

    for (size_t i = 1; i < children.size(); ++i) {
        auto nextVal = children[i]->Evaluate(path + "." + std::to_string(i), depth + 1, callback);
        try {
            accum = std::visit(applyOp, accum, nextVal);
        } catch (const std::bad_variant_access&) {
            throw std::runtime_error("Invalid variant access in arithmetic" + GetExprErrorContext());
        }
    }

    if (callback) {
        std::string opStr;
        switch (op) {
            case '+':
                opStr = "Add";
                break;
            case '-':
                opStr = "Subtract";
                break;
            case '*':
                opStr = "Multiply";
                break;
            case '/':
                opStr = "Divide";
                break;
            default:
                opStr = "Unknown";
                break;
        }
        callback(expression, path, depth, opStr, accum);
    }

    return accum;
}

LogicExpression::ValueVariant LogicExpression::Impl::Evaluate(const std::string& path, int depth,
                                                              const EvaluationCallback& callback) const {
    ValueVariant result;

    std::string exprText;
    if (expressionString) {
        exprText = *expressionString;
    } else if (parent) {
        const Impl* root = this;
        while (root->parent)
            root = root->parent;

        if (root->expressionString) {
            exprText = root->expressionString->substr(startIndex, endIndex - startIndex);
        }
    }

    if (exprText.empty()) {
        exprText = "Unknown expression";
    }

    switch (type) {
        case Type::Value:
            if (valueType == ValueType::Boolean) {
                result = value == "true";
            } else if (valueType == ValueType::Number) {
                result = std::stoi(value);
            } else if (valueType == ValueType::Enum) {
                result = EvaluateEnum();
            } else if (valueType == ValueType::Identifier) {
                result = EvaluateVariable();
            } else {
                throw std::runtime_error("Unknown value type: " + value + GetExprErrorContext());
            }

            if (callback) {
                callback(expression, path, depth, GetTypeString(), result);
            }
            return result;

        case Type::FunctionCall:
            return EvaluateFunction(path, depth, callback);

        case Type::Not: {
            auto childResult = children[0]->Evaluate(path + ".0", depth + 1, callback);
            result = !LogicExpression::GetValue<bool>(childResult);
            if (callback) {
                callback(expression, path, depth, GetTypeString(), result);
            }
            return result;
        }

        case Type::And: {
            bool accum = true;
            for (size_t i = 0; i < children.size(); ++i) {
                auto childResult = children[i]->Evaluate(path + "." + std::to_string(i), depth + 1, callback);
                if (!LogicExpression::GetValue<bool>(childResult)) {
                    accum = false;
                    break;
                }
            }
            result = accum;
            if (callback) {
                callback(expression, path, depth,
                         GetTypeString() + (LogicExpression::GetValue<bool>(result) ? "" : " (short-circuit)"), result);
            }
            return result;
        }

        case Type::Or: {
            bool accum = false;
            for (size_t i = 0; i < children.size(); ++i) {
                auto childResult = children[i]->Evaluate(path + "." + std::to_string(i), depth + 1, callback);
                if (LogicExpression::GetValue<bool>(childResult)) {
                    accum = true;
                    break;
                }
            }
            result = accum;
            if (callback) {
                callback(expression, path, depth,
                         GetTypeString() + (LogicExpression::GetValue<bool>(result) ? " (short-circuit)" : ""), result);
            }
            return result;
        }

        case Type::Comparison: {
            auto leftResult = children[0]->Evaluate(path + ".0", depth + 1, callback);
            auto rightResult = children[1]->Evaluate(path + ".1", depth + 1, callback);

            auto compare = [&](auto a, auto b) -> bool {
                using A = decltype(a);
                using B = decltype(b);

                if constexpr (std::is_same_v<A, bool> && std::is_same_v<B, bool>) {
                    if (operation == "==")
                        return a == b;
                    if (operation == "!=")
                        return a != b;
                    throw std::runtime_error("Unsupported operator for booleans: " + operation + GetExprErrorContext());
                } else if constexpr (std::is_integral_v<A> && std::is_integral_v<B>) {
                    int l = static_cast<int>(a);
                    int r = static_cast<int>(b);
                    if (operation == "==")
                        return l == r;
                    if (operation == "!=")
                        return l != r;
                    if (operation == ">=")
                        return l >= r;
                    if (operation == "<=")
                        return l <= r;
                    if (operation == ">")
                        return l > r;
                    if (operation == "<")
                        return l < r;
                    throw std::runtime_error("Unknown comparison operator: " + operation + GetExprErrorContext());
                } else {
                    throw std::runtime_error("Invalid comparison between types" + GetExprErrorContext());
                }
            };

            try {
                result = std::visit(compare, leftResult, rightResult);
                if (callback) {
                    callback(expression, path, depth, GetTypeString(), result);
                }
                return result;
            } catch (const std::bad_variant_access&) {
                throw std::runtime_error("Invalid variant access in comparison" + GetExprErrorContext());
            }
        }

        case Type::Add:
            return EvaluateArithmetic('+', path, depth, callback);
        case Type::Subtract:
            return EvaluateArithmetic('-', path, depth, callback);
        case Type::Multiply:
            return EvaluateArithmetic('*', path, depth, callback);
        case Type::Divide:
            return EvaluateArithmetic('/', path, depth, callback);

        case Type::Ternary: {
            auto condResult = children[0]->Evaluate(path + ".0", depth + 1, callback);
            bool cond = LogicExpression::GetValue<bool>(condResult);

            if (cond) {
                result = children[1]->Evaluate(path + ".1", depth + 1, callback);
            } else {
                result = children[2]->Evaluate(path + ".2", depth + 1, callback);
            }

            if (callback) {
                callback(expression, path, depth, GetTypeString() + (cond ? " (true branch)" : " (false branch)"),
                         result);
            }
            return result;
        }

        default:
            throw std::runtime_error("Unknown expression type" + GetExprErrorContext());
    }
}
