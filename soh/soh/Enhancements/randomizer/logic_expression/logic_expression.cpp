#include "logic_expression_impl.h"

#include <algorithm>
#include <charconv>
#include <stdexcept>
#include <stack>
#include <tuple>
#include <vector>

const std::vector<std::shared_ptr<LogicExpression>>& LogicExpression::GetChildren() const noexcept {
    return children;
}

LogicExpression::Type LogicExpression::GetType() const noexcept {
    return impl->type;
}

LogicExpression::ValueType LogicExpression::GetValueType() const noexcept {
    return impl->valueType;
}

const std::string& LogicExpression::GetOperation() const noexcept {
    return impl->operation;
}

const std::string& LogicExpression::GetFunctionName() const noexcept {
    return impl->functionName;
}

std::string LogicExpression::ToString() const {
    if (!impl) {
        return "";
    }

    if (impl->expressionString) {
        return *impl->expressionString;
    }

    const Impl* root = impl.get();
    while (root->parent)
        root = root->parent;

    if (!root->expressionString) {
        return "";
    }

    const auto& str = *root->expressionString;
    if (impl->startIndex > str.size() || impl->endIndex > str.size() || impl->endIndex < impl->startIndex) {
        return str;
    }

    return str.substr(impl->startIndex, impl->endIndex - impl->startIndex);
}

LogicExpression::ValueVariant LogicExpression::EvaluateVariant(const EvaluationCallback& callback) const {
    if (!impl) {
        throw std::runtime_error("LogicExpression::EvaluateVariant called with null impl");
    }
    return impl->Evaluate("0", 0, callback);
}

ExpressionEvaluation EvaluateExpression(const std::string& condition) {
    return EvaluateExpression(LogicExpression::Parse(condition));
}

ExpressionEvaluation EvaluateExpression(std::shared_ptr<LogicExpression> expression) {
    if (!expression) {
        throw std::runtime_error("EvaluateExpression called with null expression");
    }

    std::vector<std::tuple<std::shared_ptr<LogicExpression>, std::string, int, std::string, LogicExpression::ValueVariant>>
        evaluationSequence;

    auto recordCallback = [&evaluationSequence](const std::shared_ptr<LogicExpression>& expr, const std::string& path,
                                                int depth, const std::string& type,
                                                const LogicExpression::ValueVariant& result) {
        evaluationSequence.emplace_back(expr, path, depth, type, result);
    };

    (void)expression->Evaluate<LogicExpression::ValueVariant>(recordCallback);

    auto parseNextSegment = [](const std::string& path, size_t& pos) -> int {
        if (pos > path.size()) {
            return 0;
        }

        size_t end = path.find('.', pos);
        if (end == std::string::npos) {
            end = path.size();
        }

        int v = 0;
        if (end > pos) {
            const char* first = path.data() + pos;
            const char* last = path.data() + end;
            auto [ptr, ec] = std::from_chars(first, last, v);
            if (ec != std::errc{} || ptr != last) {
                v = 0;
            }
        }

        pos = (end < path.size()) ? (end + 1) : (path.size() + 1);
        return v;
    };

    std::sort(evaluationSequence.begin(), evaluationSequence.end(), [&parseNextSegment](const auto& a, const auto& b) {
        const auto& pathA = std::get<1>(a);
        const auto& pathB = std::get<1>(b);

        size_t posA = 0;
        size_t posB = 0;
        while (posA <= pathA.size() && posB <= pathB.size()) {
            int segA = parseNextSegment(pathA, posA);
            int segB = parseNextSegment(pathB, posB);
            if (segA != segB) {
                return segA < segB;
            }
        }

        // If one path has fewer segments and all previous segments matched, shorter path comes first.
        return posA > pathA.size() && posB <= pathB.size();
    });

    if (evaluationSequence.empty()) {
        throw std::runtime_error("Expression evaluation produced no results");
    }

    ExpressionEvaluation evaluation;
    evaluation.Expression = std::get<0>(evaluationSequence[0]);
    evaluation.Depth = std::get<2>(evaluationSequence[0]);
    evaluation.Type = std::get<3>(evaluationSequence[0]);
    evaluation.Result = std::get<4>(evaluationSequence[0]);

    std::stack<ExpressionEvaluation*> parentStack;
    parentStack.push(&evaluation);

    for (size_t i = 1; i < evaluationSequence.size(); ++i) {
        ExpressionEvaluation child;
        child.Expression = std::get<0>(evaluationSequence[i]);
        child.Depth = std::get<2>(evaluationSequence[i]);
        child.Type = std::get<3>(evaluationSequence[i]);
        child.Result = std::get<4>(evaluationSequence[i]);

        while (!parentStack.empty() && parentStack.top()->Depth >= child.Depth) {
            parentStack.pop();
        }

        if (!parentStack.empty()) {
            parentStack.top()->Children.push_back(std::move(child));
            parentStack.push(&(parentStack.top()->Children.back()));
        }
    }

    return evaluation;
}

std::string ToString(const LogicExpression::ValueVariant& value) {
    return std::visit(
        [](auto v) -> std::string {
            using V = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<V, bool>) {
                return v ? "true" : "false";
            } else {
                return std::to_string(v);
            }
        },
        value);
}
