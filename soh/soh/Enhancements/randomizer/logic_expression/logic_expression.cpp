#include "logic_expression_impl.h"

#include <algorithm>
#include <sstream>
#include <stack>
#include <tuple>
#include <vector>

const std::vector<std::shared_ptr<LogicExpression>>& LogicExpression::GetChildren() const {
    return children;
}

LogicExpression::Type LogicExpression::GetType() const {
    return impl->type;
}

LogicExpression::ValueType LogicExpression::GetValueType() const {
    return impl->valueType;
}

std::string LogicExpression::GetOperation() const {
    return impl->operation;
}

std::string LogicExpression::GetFunctionName() const {
    return impl->functionName;
}

std::string LogicExpression::ToString() const {
    if (impl->expressionString != nullptr) {
        return *impl->expressionString;
    }

    const Impl* root = impl->parent;
    while (root->parent)
        root = root->parent;
    return (*root->expressionString).substr(impl->startIndex, impl->endIndex - impl->startIndex);
}

LogicExpression::ValueVariant LogicExpression::EvaluateVariant(const EvaluationCallback& callback) const {
    return impl->Evaluate("0", 0, callback);
}

ExpressionEvaluation EvaluateExpression(std::string condition) {
    return EvaluateExpression(LogicExpression::Parse(condition));
}

ExpressionEvaluation EvaluateExpression(std::shared_ptr<LogicExpression> expression) {
    std::vector<std::tuple<std::shared_ptr<LogicExpression>, std::string, int, std::string, LogicExpression::ValueVariant>>
        evaluationSequence;

    auto recordCallback = [&evaluationSequence](const std::shared_ptr<LogicExpression>& expr, const std::string& path,
                                                int depth, const std::string& type,
                                                const LogicExpression::ValueVariant& result) {
        evaluationSequence.emplace_back(expr, path, depth, type, result);
    };

    (void)expression->Evaluate<LogicExpression::ValueVariant>(recordCallback);

    auto pathToVector = [](const std::string& path) {
        std::vector<int> result;
        std::stringstream ss(path);
        std::string segment;

        while (std::getline(ss, segment, '.')) {
            try {
                result.push_back(std::stoi(segment));
            } catch (const std::exception&) {
                result.push_back(0);
            }
        }

        return result;
    };

    std::sort(evaluationSequence.begin(), evaluationSequence.end(), [&pathToVector](const auto& a, const auto& b) {
        const auto& pathA = std::get<1>(a);
        const auto& pathB = std::get<1>(b);

        auto vecA = pathToVector(pathA);
        auto vecB = pathToVector(pathB);

        size_t i = 0;
        while (i < vecA.size() && i < vecB.size()) {
            if (vecA[i] != vecB[i]) {
                return vecA[i] < vecB[i];
            }
            i++;
        }

        return vecA.size() < vecB.size();
    });

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
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<int8_t>(value)) {
        return std::to_string(std::get<int8_t>(value));
    } else if (std::holds_alternative<int16_t>(value)) {
        return std::to_string(std::get<int16_t>(value));
    } else if (std::holds_alternative<int32_t>(value)) {
        return std::to_string(std::get<int32_t>(value));
    } else if (std::holds_alternative<uint8_t>(value)) {
        return std::to_string(std::get<uint8_t>(value));
    } else if (std::holds_alternative<uint16_t>(value)) {
        return std::to_string(std::get<uint16_t>(value));
    } else if (std::holds_alternative<uint32_t>(value)) {
        return std::to_string(std::get<uint32_t>(value));
    }

    return "unknown";
}
