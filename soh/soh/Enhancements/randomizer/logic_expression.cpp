#include "logic_expression.h"
#include "randomizerTypes.h"
#include "location_access.h"
#include "dungeon.h"
#include "variables.h"

#include <sstream>
#include <stack>
#include <unordered_map>
#include <variant>

extern SaveContext gSaveContext;

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

#pragma region Tokenizer

enum class LETokenType { Identifier, Number, Boolean, EnumConstant, ParenOpen, ParenClose, Comma, Operator, End };

struct Token {
    LETokenType Type;
    std::string Text;
    size_t StartIndex;
    size_t EndIndex;
};

static std::string GetCharacterErrorContext(const std::string& input, size_t pos, size_t contextLen = 10) {
    size_t start = (pos >= contextLen) ? pos - contextLen : 0;
    size_t end = std::min(input.size(), pos + contextLen + 1);
    std::string contextLine = input.substr(start, end - start);
    std::string pointerLine;
    pointerLine.append(pos - start, ' ');
    pointerLine.push_back('^');
    return "\n" + contextLine + "\n" + pointerLine;
}

bool IsEnumConstant(const std::string& s) {
    if (LogicExpression::Impl::enumMap.empty()) {
        LogicExpression::Impl::PopulateEnumMap();
    }
    return LogicExpression::Impl::enumMap.find(s) != LogicExpression::Impl::enumMap.end();
}

static bool IsIdentifierChar(char c) {
    return std::isalnum(c) || c == '_' || c == '.' || c == ':';
}

static std::vector<Token> Tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;
    const size_t len = input.length();

    while (i < len) {
        if (std::isspace(input[i])) {
            ++i;
            continue;
        }

        size_t start = i;
        if (std::isalpha(input[i])) {
            while (i < len && IsIdentifierChar(input[i]))
                ++i;
            std::string id = input.substr(start, i - start);
            if (id == "true" || id == "false")
                tokens.emplace_back(Token{ LETokenType::Boolean, id, start, i });
            else if (IsEnumConstant(id))
                tokens.emplace_back(Token{ LETokenType::EnumConstant, id, start, i });
            else
                tokens.emplace_back(Token{ LETokenType::Identifier, id, start, i });
        } else if (std::isdigit(input[i])) {
            while (i < len && std::isdigit(input[i]))
                ++i;
            tokens.emplace_back(Token{ LETokenType::Number, input.substr(start, i - start), start, i });
        } else if (input[i] == '(') {
            tokens.emplace_back(Token{ LETokenType::ParenOpen, "(", start, start + 1 });
            ++i;
        } else if (input[i] == ')') {
            tokens.emplace_back(Token{ LETokenType::ParenClose, ")", start, start + 1 });
            ++i;
        } else if (input[i] == ',') {
            tokens.emplace_back(Token{ LETokenType::Comma, ",", start, start + 1 });
            ++i;
        } else if (input[i] == '?') {
            tokens.emplace_back(Token{ LETokenType::Operator, "?", start, start + 1 });
            ++i;
        } else if (input[i] == ':') {
            tokens.emplace_back(Token{ LETokenType::Operator, ":", start, start + 1 });
            ++i;
        } else if (i + 1 < len &&
                   ((input[i] == '!' && input[i + 1] == '=') || (input[i] == '&' && input[i + 1] == '&') ||
                    (input[i] == '|' && input[i + 1] == '|') || (input[i] == '=' && input[i + 1] == '=') ||
                    (input[i] == '>' && input[i + 1] == '=') || (input[i] == '<' && input[i + 1] == '='))) {
            tokens.emplace_back(Token{ LETokenType::Operator, input.substr(i, 2), start, start + 2 });
            i += 2;
        } else if (strchr("!<>=+-*/", input[i])) {
            tokens.emplace_back(Token{ LETokenType::Operator, std::string(1, input[i]), start, start + 1 });
            ++i;
        } else {
            throw std::runtime_error("Unknown character: '" + std::string(1, input[i]) + "' at position " +
                                     std::to_string(i) + GetCharacterErrorContext(input, i));
        }
    }
    tokens.emplace_back(Token{ LETokenType::End, "", i, i });
    return tokens;
}

#pragma endregion

#pragma region Parser

static std::string GetTokenErrorContext(const std::string& input, const Token& token, size_t contextLen = 10) {
    size_t start = (token.StartIndex >= contextLen) ? token.StartIndex - contextLen : 0;
    size_t end = std::min(input.size(), token.EndIndex + contextLen);
    std::string contextLine = input.substr(start, end - start);
    size_t pointerOffset = token.StartIndex - start;
    size_t pointerLength = (token.EndIndex > token.StartIndex) ? token.EndIndex - token.StartIndex : 1;
    std::string pointerLine(pointerOffset, ' ');
    pointerLine.append(std::string(pointerLength, '^'));
    return "\n" + contextLine + "\n" + pointerLine;
}

class Parser {
    const std::vector<Token> tokens;
    const std::string& input;
    size_t pos = 0;

    const Token& Peek() const {
        return tokens[pos];
    }

    const Token& Next() {
        return tokens[pos++];
    }

    bool Match(const std::string& op) {
        if (Peek().Type == LETokenType::Operator && Peek().Text == op) {
            ++pos;
            return true;
        }
        return false;
    }

    // Helper to build an n-ary node with common book-keeping
    std::shared_ptr<LogicExpression::Impl>
    BuildNAryNode(LogicExpression::Type type, const std::string& op, size_t initial_pos,
                  const std::vector<std::shared_ptr<LogicExpression::Impl>>& terms) {
        auto expr = std::make_shared<LogicExpression::Impl>();
        expr->type = type;
        expr->operation = op;
        for (auto& t : terms) {
            expr->children.emplace_back(t);
            expr->children.back()->parent = expr.get();
        }
        expr->startIndex = tokens[initial_pos].StartIndex;
        expr->endIndex = tokens[pos - 1].EndIndex;
        return expr;
    }

    std::shared_ptr<LogicExpression::Impl> ParsePrimary() {
        size_t initial_pos = pos;
        std::shared_ptr<LogicExpression::Impl> expr;

        if (Match("!")) {
            expr = std::make_shared<LogicExpression::Impl>();
            expr->type = LogicExpression::Type::Not;
            expr->children.emplace_back(ParsePrimary());
            expr->children.back()->parent = expr.get();
        } else if (Peek().Type == LETokenType::ParenOpen) {
            Next();
            expr = ParseExpression();
            if (Peek().Type != LETokenType::ParenClose) {
                throw std::runtime_error("Expected ')' at position " + std::to_string(Peek().StartIndex) +
                                         GetTokenErrorContext(input, Peek()));
            }
            Next();
        } else if (Peek().Type == LETokenType::Identifier || Peek().Type == LETokenType::Boolean ||
                   Peek().Type == LETokenType::Number || Peek().Type == LETokenType::EnumConstant) {
            Token token = Next();
            expr = std::make_shared<LogicExpression::Impl>();

            if (Peek().Type == LETokenType::ParenOpen && token.Type == LETokenType::Identifier) {
                --pos;
                std::string id = Next().Text;
                Next(); // consume '('
                expr->type = LogicExpression::Type::FunctionCall;
                expr->functionName = id;
                while (Peek().Type != LETokenType::ParenClose) {
                    if (!expr->children.empty()) {
                        if (Peek().Type != LETokenType::Comma) {
                            throw std::runtime_error("Expected ',' at position " + std::to_string(Peek().StartIndex) +
                                                     GetTokenErrorContext(input, Peek()));
                        }
                        Next(); // consume ','
                    }
                    expr->children.emplace_back(ParseExpression());
                    expr->children.back()->parent = expr.get();
                }
                Next(); // consume ')'
            } else {
                expr->type = LogicExpression::Type::Value;
                expr->value = token.Text;
                if (token.Type == LETokenType::Boolean)
                    expr->valueType = LogicExpression::ValueType::Boolean;
                else if (token.Type == LETokenType::Number)
                    expr->valueType = LogicExpression::ValueType::Number;
                else if (token.Type == LETokenType::EnumConstant)
                    expr->valueType = LogicExpression::ValueType::Enum;
                else
                    expr->valueType = LogicExpression::ValueType::Identifier;
            }
        }

        if (expr == nullptr) {
            throw std::runtime_error("Unexpected token: " + Peek().Text + GetTokenErrorContext(input, Peek()));
        }

        // Set startIndex/endIndex for error context
        expr->startIndex = tokens[initial_pos].StartIndex;
        expr->endIndex = tokens[pos - 1].EndIndex;
        return expr;
    }

    template <typename LowerFunc>
    std::shared_ptr<LogicExpression::Impl>
    ParseBinaryOp(size_t& pos, const std::vector<Token>& tokens, LowerFunc lowerFunc,
                  const std::vector<std::pair<std::string, LogicExpression::Type>>& operators) {
        size_t initial_pos = pos;
        auto left = (this->*lowerFunc)();

        if (pos < tokens.size() && tokens[pos].Type == LETokenType::Operator) {
            for (const auto& [op, exprType] : operators) {
                if (tokens[pos].Text == op) {
                    ++pos; // consume operator
                    auto right = ParseBinaryOp(pos, tokens, lowerFunc, operators);
                    auto expr = std::make_shared<LogicExpression::Impl>();
                    expr->type = exprType;
                    expr->operation = op;
                    expr->children.emplace_back(left);
                    expr->children.back()->parent = expr.get();
                    expr->children.emplace_back(right);
                    expr->children.back()->parent = expr.get();
                    expr->startIndex = tokens[initial_pos].StartIndex;
                    expr->endIndex = tokens[pos - 1].EndIndex;
                    return expr;
                }
            }
        }
        return left;
    }

    std::shared_ptr<LogicExpression::Impl> ParseMulDiv() {
        // Group consecutive '*' into n-ary Multiply and '/' into n-ary Divide (left associative for '/')
        size_t initial_pos = pos;
        auto first = ParsePrimary();

        if (Peek().Type == LETokenType::Operator && (Peek().Text == "*" || Peek().Text == "/")) {
            if (Peek().Text == "*") {
                std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
                terms.emplace_back(first);
                while (Peek().Type == LETokenType::Operator && Peek().Text == "*") {
                    Next();
                    auto nextTerm = ParsePrimary();
                    terms.emplace_back(nextTerm);
                }
                if (terms.size() == 1) {
                    return first;
                }
                return BuildNAryNode(LogicExpression::Type::Multiply, "*", initial_pos, terms);
            } else { // "/"
                std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
                terms.emplace_back(first);
                while (Peek().Type == LETokenType::Operator && Peek().Text == "/") {
                    Next();
                    auto nextTerm = ParsePrimary();
                    terms.emplace_back(nextTerm);
                }
                if (terms.size() == 1) {
                    return first;
                }
                return BuildNAryNode(LogicExpression::Type::Divide, "/", initial_pos, terms);
            }
        }

        return first;
    }

    std::shared_ptr<LogicExpression::Impl> ParseAddSub() {
        // Group consecutive '+' as n-ary Add and '-' as n-ary Subtract (left associative for '-')
        size_t initial_pos = pos;
        auto first = ParseMulDiv();

        if (Peek().Type == LETokenType::Operator && (Peek().Text == "+" || Peek().Text == "-")) {
            if (Peek().Text == "+") {
                std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
                terms.emplace_back(first);
                while (Peek().Type == LETokenType::Operator && Peek().Text == "+") {
                    Next();
                    auto nextTerm = ParseMulDiv();
                    terms.emplace_back(nextTerm);
                }
                if (terms.size() == 1) {
                    return first;
                }
                return BuildNAryNode(LogicExpression::Type::Add, "+", initial_pos, terms);
            } else {
                std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
                terms.emplace_back(first);
                while (Peek().Type == LETokenType::Operator && Peek().Text == "-") {
                    Next();
                    auto nextTerm = ParseMulDiv();
                    terms.emplace_back(nextTerm);
                }
                if (terms.size() == 1) {
                    return first;
                }
                return BuildNAryNode(LogicExpression::Type::Subtract, "-", initial_pos, terms);
            }
        }

        return first;
    }

    std::shared_ptr<LogicExpression::Impl> ParseComparison() {
        return ParseBinaryOp(pos, tokens, &Parser::ParseAddSub,
                             { { "==", LogicExpression::Type::Comparison },
                               { "!=", LogicExpression::Type::Comparison },
                               { ">=", LogicExpression::Type::Comparison },
                               { "<=", LogicExpression::Type::Comparison },
                               { ">", LogicExpression::Type::Comparison },
                               { "<", LogicExpression::Type::Comparison } });
    }

    std::shared_ptr<LogicExpression::Impl> ParseAnd() {
        // Parse as n-ary AND collecting chains of '&&'
        size_t initial_pos = pos;
        auto first = ParseComparison();
        std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
        terms.emplace_back(first);

        while (Peek().Type == LETokenType::Operator && Peek().Text == "&&") {
            Next(); // consume '&&'
            auto nextTerm = ParseComparison();
            terms.emplace_back(nextTerm);
        }

        if (terms.size() == 1) {
            return first;
        }

        return BuildNAryNode(LogicExpression::Type::And, "&&", initial_pos, terms);
    }

    std::shared_ptr<LogicExpression::Impl> ParseOr() {
        // Parse as n-ary OR collecting chains of '||'
        size_t initial_pos = pos;
        auto first = ParseAnd();
        std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
        terms.emplace_back(first);

        while (Peek().Type == LETokenType::Operator && Peek().Text == "||") {
            Next(); // consume '||'
            auto nextTerm = ParseAnd();
            terms.emplace_back(nextTerm);
        }

        if (terms.size() == 1) {
            return first;
        }

        return BuildNAryNode(LogicExpression::Type::Or, "||", initial_pos, terms);
    }

    std::shared_ptr<LogicExpression::Impl> ParseTernary() {
        size_t initial_pos = pos;
        auto cond = ParseOr();

        if (Peek().Type == LETokenType::Operator && Peek().Text == "?") {
            Next(); // consume '?'
            auto trueExpr = ParseTernary();

            if (!(Peek().Type == LETokenType::Operator && Peek().Text == ":")) {
                throw std::runtime_error("Expected ':' in ternary expression at position " +
                                         std::to_string(Peek().StartIndex) + GetTokenErrorContext(input, Peek()));
            }

            Next(); // consume ':'
            auto falseExpr = ParseTernary();

            auto expr = std::make_shared<LogicExpression::Impl>();
            expr->type = LogicExpression::Type::Ternary;

            expr->children.emplace_back(cond);
            expr->children.back()->parent = expr.get();
            expr->children.emplace_back(trueExpr);
            expr->children.back()->parent = expr.get();
            expr->children.emplace_back(falseExpr);
            expr->children.back()->parent = expr.get();

            expr->startIndex = tokens[initial_pos].StartIndex;
            expr->endIndex = tokens[pos - 1].EndIndex;
            return expr;
        }

        return cond;
    }

    std::shared_ptr<LogicExpression::Impl> ParseExpression() {
        return ParseTernary();
    }

  public:
    Parser(const std::string& input) : tokens(Tokenize(input)), input(input) {
    }

    std::shared_ptr<LogicExpression::Impl> Parse() {
        if (tokens.empty() || tokens[0].Type == LETokenType::End) {
            throw std::runtime_error("Empty input");
        }
        auto expr = ParseExpression();
        if (Peek().Type != LETokenType::End) {
            throw std::runtime_error("Unexpected token: " + Peek().Text + " at position " +
                                     std::to_string(Peek().StartIndex) + GetTokenErrorContext(input, Peek()));
        }
        expr->expressionString = std::make_unique<std::string>(input);
        return expr;
    };
};

#pragma endregion

std::shared_ptr<LogicExpression> LogicExpression::Parse(const std::string& exprStr) {
    Parser parser(exprStr);
    std::shared_ptr<LogicExpression::Impl> impl = parser.Parse();

    std::function<std::shared_ptr<LogicExpression>(const std::shared_ptr<LogicExpression::Impl>&)> populateChildren;
    populateChildren = [&](const std::shared_ptr<LogicExpression::Impl>& impl) {
        auto expr = std::make_shared<LogicExpression>();
        expr->impl = impl;
        impl->expression = expr;
        for (const auto& child : impl->children) {
            expr->children.emplace_back(populateChildren(child));
        }
        return expr;
    };

    return populateChildren(impl);
}

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

#pragma region Evaluator

std::string LogicExpression::Impl::GetExprErrorContext() const {
    size_t contextLen = 10;
    // Find root
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

// Macro to register a function using its name.
// This macro simplifies the insertion of functions into the functionAdapters map by
// automatically converting the function pointer or lambda into a FunctionAdapter.
// Usage: REGISTER_FUNCTION(functionName)
#define REGISTER_FUNCTION(fn) \
    { #fn, LogicExpression::Impl::RegisterFunction(#fn, fn) }

#define REGISTER_FUNCTION_WITH_DEFAULTS(fn, ...) \
    { #fn, LogicExpression::Impl::RegisterFunctionWithDefaults(#fn, fn, std::make_tuple(__VA_ARGS__)) }

#define REGISTER_LOGIC_FUNCTION(fn) \
    { #fn, LogicExpression::Impl::RegisterLogicFunction(#fn, &Rando::Logic::fn) }

#define REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(fn, ...)                                              \
    {                                                                                               \
#fn, LogicExpression::Impl::RegisterLogicFunctionWithDefaults(#fn, &Rando::Logic::fn,       \
                                                                      std::make_tuple(__VA_ARGS__)) \
    }

#define REGISTER_LOGIC_VARIABLE(var) \
    { #var, LogicExpression::Impl::RegisterLogicVariable(#var, &Rando::Logic::var) }

#pragma region Forwarding Functions
static uint8_t GetOption(const RandomizerSettingKey key) {
    return ctx->GetOption(key).Get();
}

static uint8_t GetTrickOption(const RandomizerTrick trick) {
    return ctx->GetTrickOption(trick).Get();
}

static bool IsDungeonVanilla(const Rando::DungeonKey dungeon) {
    return ctx->GetDungeon(dungeon)->IsVanilla();
}

static bool IsDungeonMQ(const Rando::DungeonKey dungeon) {
    return ctx->GetDungeon(dungeon)->IsMQ();
}

static bool IsTrialSkipped(const TrialKey trial) {
    return ctx->GetTrial(trial)->IsSkipped();
}

static uint8_t TriforcePiecesCollected() {
    return gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected;
}

static bool RegionAgeTimeAccess(const RandomizerRegion region, const RegionAgeTime ageTime) {
    if (ageTime == RegionAgeTime::childDay)
        return RegionTable(region)->childDay;
    if (ageTime == RegionAgeTime::childNight)
        return RegionTable(region)->childNight;
    if (ageTime == RegionAgeTime::adultDay)
        return RegionTable(region)->adultDay;
    if (ageTime == RegionAgeTime::adultNight)
        return RegionTable(region)->adultNight;
}
#pragma endregion

std::unordered_map<std::string, LogicExpression::Impl::FunctionAdapter> LogicExpression::Impl::functionAdapters;
void LogicExpression::Impl::PopulateFunctionAdapters() {
    functionAdapters = {
        REGISTER_FUNCTION(Here),
        REGISTER_FUNCTION_WITH_DEFAULTS(MQSpiritSharedBrokenWallRoom, RandomizerRegion{}, ConditionFn{}, false),
        REGISTER_FUNCTION_WITH_DEFAULTS(MQSpiritSharedStatueRoom, RandomizerRegion{}, ConditionFn{}, false),
        REGISTER_FUNCTION_WITH_DEFAULTS(GetCheckPrice, RC_UNKNOWN_CHECK),
        REGISTER_FUNCTION(GetWalletCapacity),
        REGISTER_FUNCTION(GetOption),
        REGISTER_FUNCTION(GetTrickOption),
        REGISTER_FUNCTION(ChildCanAccess),
        REGISTER_FUNCTION(IsDungeonVanilla),
        REGISTER_FUNCTION(IsDungeonMQ),
        REGISTER_FUNCTION(IsTrialSkipped),
        REGISTER_FUNCTION(TriforcePiecesCollected),
        REGISTER_FUNCTION(RegionAgeTimeAccess),
        REGISTER_FUNCTION(CanPlantBean),

        REGISTER_LOGIC_FUNCTION(CanUse),
        REGISTER_LOGIC_FUNCTION(HasProjectile),
        REGISTER_LOGIC_FUNCTION(HasItem),
        REGISTER_LOGIC_FUNCTION(HasBossSoul),
        REGISTER_LOGIC_FUNCTION(CanOpenOverworldDoor),
        REGISTER_LOGIC_FUNCTION(SmallKeys),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanGroundJump, false),
        REGISTER_LOGIC_FUNCTION(CanOpenUnderwaterChest),
        REGISTER_LOGIC_FUNCTION(CanDoGlitch),
        REGISTER_LOGIC_FUNCTION(CanEquipSwap),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanKillEnemy, RandomizerEnemy{}, ED_CLOSE, true, uint8_t{ 1 }, false,
                                              false),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanPassEnemy, RandomizerEnemy{}, ED_CLOSE, true),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanAvoidEnemy, RandomizerEnemy{}, false, uint8_t{ 1 }),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanGetEnemyDrop, RandomizerEnemy{}, ED_CLOSE, false),
        REGISTER_LOGIC_FUNCTION(CanBreakMudWalls),
        REGISTER_LOGIC_FUNCTION(CanGetDekuBabaSticks),
        REGISTER_LOGIC_FUNCTION(CanGetDekuBabaNuts),
        REGISTER_LOGIC_FUNCTION(CanHitEyeTargets),
        REGISTER_LOGIC_FUNCTION(CanDetonateBombFlowers),
        REGISTER_LOGIC_FUNCTION(CanDetonateUprightBombFlower),
        REGISTER_LOGIC_FUNCTION(MQWaterLevel),
        REGISTER_LOGIC_FUNCTION(BottleCount),
        REGISTER_LOGIC_FUNCTION(OcarinaButtons),
        REGISTER_LOGIC_FUNCTION(HasBottle),
        REGISTER_LOGIC_FUNCTION(CanUseSword),
        REGISTER_LOGIC_FUNCTION(CanJumpslashExceptHammer),
        REGISTER_LOGIC_FUNCTION(CanJumpslash),
        REGISTER_LOGIC_FUNCTION_WITH_DEFAULTS(CanHitSwitch, ED_CLOSE, false),
        REGISTER_LOGIC_FUNCTION(CanDamage),
        REGISTER_LOGIC_FUNCTION(CanAttack),
        REGISTER_LOGIC_FUNCTION(BombchusEnabled),
        REGISTER_LOGIC_FUNCTION(BombchuRefill),
        REGISTER_LOGIC_FUNCTION(HookshotOrBoomerang),
        REGISTER_LOGIC_FUNCTION(ScarecrowsSong),
        REGISTER_LOGIC_FUNCTION(BlueFire),
        REGISTER_LOGIC_FUNCTION(HasExplosives),
        REGISTER_LOGIC_FUNCTION(BlastOrSmash),
        REGISTER_LOGIC_FUNCTION(CanSpawnSoilSkull),
        REGISTER_LOGIC_FUNCTION(CanReflectNuts),
        REGISTER_LOGIC_FUNCTION(CanCutShrubs),
        REGISTER_LOGIC_FUNCTION(CanStunDeku),
        REGISTER_LOGIC_FUNCTION(CallGossipFairy),
        REGISTER_LOGIC_FUNCTION(CallGossipFairyExceptSuns),
        REGISTER_LOGIC_FUNCTION(EffectiveHealth),
        REGISTER_LOGIC_FUNCTION(Hearts),
        REGISTER_LOGIC_FUNCTION(StoneCount),
        REGISTER_LOGIC_FUNCTION(MedallionCount),
        REGISTER_LOGIC_FUNCTION(DungeonCount),
        REGISTER_LOGIC_FUNCTION(FireTimer),
        REGISTER_LOGIC_FUNCTION(WaterTimer),
        REGISTER_LOGIC_FUNCTION(TakeDamage),
        REGISTER_LOGIC_FUNCTION(CanOpenBombGrotto),
        REGISTER_LOGIC_FUNCTION(CanOpenStormsGrotto),
        REGISTER_LOGIC_FUNCTION(CanGetNightTimeGS),
        REGISTER_LOGIC_FUNCTION(CanBreakUpperBeehives),
        REGISTER_LOGIC_FUNCTION(CanBreakLowerBeehives),
        REGISTER_LOGIC_FUNCTION(CanBreakPots),
        REGISTER_LOGIC_FUNCTION(CanBreakCrates),
        REGISTER_LOGIC_FUNCTION(CanBreakSmallCrates),
        REGISTER_LOGIC_FUNCTION(CanBonkTrees),
        REGISTER_LOGIC_FUNCTION(HasFireSource),
        REGISTER_LOGIC_FUNCTION(HasFireSourceWithTorch),
        REGISTER_LOGIC_FUNCTION(TradeQuestStep),
        REGISTER_LOGIC_FUNCTION(CanStandingShield),
        REGISTER_LOGIC_FUNCTION(CanShield),
        REGISTER_LOGIC_FUNCTION(CanUseProjectile),
        REGISTER_LOGIC_FUNCTION(CanBuildRainbowBridge),
        REGISTER_LOGIC_FUNCTION(CanTriggerLACS),
        REGISTER_LOGIC_FUNCTION(IsFireLoopLocked),
        REGISTER_LOGIC_FUNCTION(Get),
        REGISTER_LOGIC_FUNCTION(GetGSCount),
        REGISTER_LOGIC_FUNCTION(CanClearStalagmite),
    };
}

LogicExpression::ValueVariant LogicExpression::Impl::EvaluateFunction(const std::string& path, int depth,
                                                                      const EvaluationCallback& callback) const {
    if (functionAdapters.empty()) {
        PopulateFunctionAdapters();
    }
    try {
        auto it = functionAdapters.find(functionName);
        if (it != functionAdapters.end()) {
            auto result = it->second(children, path, depth, callback);

            // If callback is provided, call it with function info
            if (callback) {
                callback(expression, path, depth, GetTypeString(), result);
            }

            return result;
        }
        throw std::runtime_error("Unknown function: " + functionName + GetExprErrorContext());
    } catch (const std::out_of_range&) {
        throw std::runtime_error("Insufficient arguments for function: " + functionName + GetExprErrorContext());
    }
}

std::unordered_map<std::string, int> LogicExpression::Impl::enumMap;
void LogicExpression::Impl::PopulateEnumMap() {
#define DEFINE_DungeonKey(value) { #value, Rando::value },
#define DEFINE_HintType(value) { #value, value },
#define DEFINE_LogicVal(value) { #value, value },
#define DEFINE_RAND_INF(value) { #value, value },
#define DEFINE_RandomizerArea(value) { #value, value },
#define DEFINE_TrialKey(value) { #value, value },
#define DEFINE_RandomizerCheckType(value) { #value, value },
#define DEFINE_RandomizerCheckQuest(value) { #value, value },
#define DEFINE_RandomizerCheckArea(value) { #value, value },
#define DEFINE_RandomizerCheckStatus(value) { #value, value },
#define DEFINE_RandomizerRegion(value) { #value, value },
#define DEFINE_RandomizerCheck(value) { #value, value },
#define DEFINE_RandomizerTrick(value) { #value, value },
#define DEFINE_RandomizerGet(value) { #value, value },
#define DEFINE_RandomizerHint(value) { #value, value },
#define DEFINE_RandomizerHintTextKey(value) { #value, value },
#define DEFINE_RandomizerSettingGroupKey(value) { #value, value },
#define DEFINE_RandomizerSettingKey(value) { #value, value },
#define DEFINE_RandoOptionGenericOffOn(value) { #value, value },
#define DEFINE_RandoOptionGenericNoYes(value) { #value, value },
#define DEFINE_RandoOptionGenericSkip(value) { #value, value },
#define DEFINE_RandoOptionForest(value) { #value, value },
#define DEFINE_RandoOptionDoorOfTime(value) { #value, value },
#define DEFINE_RandoOptionZorasFountain(value) { #value, value },
#define DEFINE_RandoOptionSleepingWaterfall(value) { #value, value },
#define DEFINE_RandoOptionJabu(value) { #value, value },
#define DEFINE_RandoOptionStartingAge(value) { #value, value },
#define DEFINE_RandoOptionGerudoFortress(value) { #value, value },
#define DEFINE_RandoOptionKakarikoGate(value) { #value, value },
#define DEFINE_RandoOptionRainbowBridge(value) { #value, value },
#define DEFINE_RandoOptionBridgeRewards(value) { #value, value },
#define DEFINE_RandoOptionShopsanity(value) { #value, value },
#define DEFINE_RandoOptionShopsanityCount(value) { #value, value },
#define DEFINE_RandoOptionPrices(value) { #value, value },
#define DEFINE_RandoOptionScrubsanity(value) { #value, value },
#define DEFINE_RandoOptionAmmoDrops(value) { #value, value },
#define DEFINE_RandoOptionBombchuBag(value) { #value, value },
#define DEFINE_RandoOptionBossSouls(value) { #value, value },
#define DEFINE_RandoOptionsFishsanity(value) { #value, value },
#define DEFINE_RandoOptionInfiniteUpgrades(value) { #value, value },
#define DEFINE_RandoOptionDungeonItemLocation(value) { #value, value },
#define DEFINE_RandoOptionDungeonRewards(value) { #value, value },
#define DEFINE_RandoOptionKeyrings(value) { #value, value },
#define DEFINE_RandoOptionKeyringForDungeon(value) { #value, value },
#define DEFINE_RandoOptionGanonsBossKey(value) { #value, value },
#define DEFINE_RandoOptionLACSCondition(value) { #value, value },
#define DEFINE_RandoOptionLACSRewards(value) { #value, value },
#define DEFINE_RandoOptionGanonsTrials(value) { #value, value },
#define DEFINE_RandoOptionDungeonEntranceShuffle(value) { #value, value },
#define DEFINE_RandoOptionBossRoomEntranceShuffle(value) { #value, value },
#define DEFINE_RandoOptionInteriorEntranceShuffle(value) { #value, value },
#define DEFINE_RandoOptionSongShuffle(value) { #value, value },
#define DEFINE_RandoOptionShuffleMerchants(value) { #value, value },
#define DEFINE_RandoOptionStartingOcarina(value) { #value, value },
#define DEFINE_RandoOptionItemPool(value) { #value, value },
#define DEFINE_RandoOptionIceTraps(value) { #value, value },
#define DEFINE_RandoOptionGossipStones(value) { #value, value },
#define DEFINE_RandoOptionHintClarity(value) { #value, value },
#define DEFINE_RandoOptionHintDistribution(value) { #value, value },
#define DEFINE_RandoOptionGerudoKeys(value) { #value, value },
#define DEFINE_RandoOptionTokensanity(value) { #value, value },
#define DEFINE_RandoOptionFreestanding(value) { #value, value },
#define DEFINE_RandoOptionShufflePots(value) { #value, value },
#define DEFINE_RandoOptionShuffleGrass(value) { #value, value },
#define DEFINE_RandoOptionShuffleCrates(value) { #value, value },
#define DEFINE_RandoOptionLinksPocket(value) { #value, value },
#define DEFINE_RandoOptionLogic(value) { #value, value },
#define DEFINE_RandoOptionDamageMultiplier(value) { #value, value },
#define DEFINE_RandoOptionMQDungeons(value) { #value, value },
#define DEFINE_RandoOptionLocationInclusion(value) { #value, value },
#define DEFINE_RandoOptionChestGame(value) { #value, value },
#define DEFINE_RandoOptionMQSet(value) { #value, value },
#define DEFINE_ItemObtainability(value) { #value, value },
#define DEFINE_TrackerWindowType(value) { #value, value },
#define DEFINE_TrackerDisplayType(value) { #value, value },
#define DEFINE_TrackerComboButton(value) { #value, value },
#define DEFINE_TriforceHuntMessages(value) { #value, value },
#define DEFINE_RandomizerEnemy(value) { #value, value },
#define DEFINE_EnemyDistance(value) { #value, value },
#define DEFINE_RandoWaterLevel(value) { #value, value },
#define DEFINE_GrottoEntranceOffsets(value) { #value, value },

    // static const fixes C6262: Excessive stack usage
    struct Pair {
        const char* key;
        int value;
    };
    static const Pair kEnumPairs[] = {
#include "randomizerEnums.h"
        { "HasProjectileAge::Adult", (int)Rando::HasProjectileAge::Adult },
        { "HasProjectileAge::Child", (int)Rando::HasProjectileAge::Child },
        { "HasProjectileAge::Both", (int)Rando::HasProjectileAge::Both },
        { "HasProjectileAge::Either", (int)Rando::HasProjectileAge::Either },
        { "GlitchType::EquipSwapDins", (int)Rando::GlitchType::EquipSwapDins },
        { "GlitchType::EquipSwap", (int)Rando::GlitchType::EquipSwap },
        { "RegionAgeTime::childDay", (int)RegionAgeTime::childDay },
        { "RegionAgeTime::childNight", (int)RegionAgeTime::childNight },
        { "RegionAgeTime::adultDay", (int)RegionAgeTime::adultDay },
        { "RegionAgeTime::adultNight", (int)RegionAgeTime::adultNight },
#define DEFINE_SCENE(_0, _1, value, _3, _4, _5) { #value, value },
#include "tables/scene_table.h"
#undef DEFINE_SCENE
    };

    enumMap.reserve(sizeof(kEnumPairs) / sizeof(Pair));
    for (const auto& p : kEnumPairs) {
        enumMap.emplace(p.key, p.value);
    }
}

LogicExpression::ValueVariant LogicExpression::Impl::EvaluateEnum() const {
    if (enumMap.empty()) {
        PopulateEnumMap();
    }
    auto it = enumMap.find(value);
    if (it != enumMap.end()) {
        return it->second;
    }
    throw std::runtime_error("Unknown enum constant: " + value + GetExprErrorContext());
}

std::unordered_map<std::string, LogicExpression::Impl::FunctionAdapter> LogicExpression::Impl::variableAdapters;
void LogicExpression::Impl::PopulateVariableAdapters() {
    variableAdapters = {
        REGISTER_LOGIC_VARIABLE(Bottles),      REGISTER_LOGIC_VARIABLE(NumBottles),
        REGISTER_LOGIC_VARIABLE(PieceOfHeart), REGISTER_LOGIC_VARIABLE(HeartContainer),
        REGISTER_LOGIC_VARIABLE(IsChild),      REGISTER_LOGIC_VARIABLE(IsAdult),
        REGISTER_LOGIC_VARIABLE(BigPoes),      REGISTER_LOGIC_VARIABLE(BaseHearts),
        REGISTER_LOGIC_VARIABLE(AtDay),        REGISTER_LOGIC_VARIABLE(AtNight),
    };
}

LogicExpression::ValueVariant LogicExpression::Impl::EvaluateVariable() const {
    if (variableAdapters.empty()) {
        PopulateVariableAdapters();
    }

    auto it = variableAdapters.find(value);
    if (it != variableAdapters.end()) {
        std::vector<std::shared_ptr<LogicExpression::Impl>> emptyArgs;
        return it->second(emptyArgs, "var", 0, nullptr); // Call the variable adapter with empty arguments
    }

    throw std::runtime_error("Unknown variable: '" + value + "'" + GetExprErrorContext());
}

// Helper for arithmetic operations to reduce duplication
LogicExpression::ValueVariant LogicExpression::Impl::EvaluateArithmetic(char op, const std::string& path, int depth,
                                                                        const EvaluationCallback& callback) const {
    if (children.empty()) {
        throw std::runtime_error("Arithmetic node has no children" + GetExprErrorContext());
    }

    auto applyOp = [&](auto a, auto b) -> ValueVariant {
        using A = std::decay_t<decltype(a)>;
        using B = std::decay_t<decltype(b)>;

        if constexpr (std::is_integral_v<A> && std::is_integral_v<B>) {
            // Normalize to signed for '-', '/' and mixed signedness; keep unsigned for '+' and '*' when both unsigned
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
            } else if (op == '-' || op == '/') {
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
            } else {
                throw std::runtime_error("Unknown arithmetic op" + GetExprErrorContext());
            }
        } else {
            throw std::runtime_error("Invalid types for arithmetic (must be integral, not bool)" +
                                     GetExprErrorContext());
        }
    };

    // Evaluate first child to initialize accumulator
    ValueVariant accum = children[0]->Evaluate(path + ".0", depth + 1, callback);

    // Fold across remaining children left-associatively
    for (size_t i = 1; i < children.size(); ++i) {
        auto nextVal = children[i]->Evaluate(path + "." + std::to_string(i), depth + 1, callback);
        try {
            accum = std::visit(applyOp, accum, nextVal);
        } catch (const std::bad_variant_access&) {
            throw std::runtime_error("Invalid variant access in arithmetic" + GetExprErrorContext());
        }
    }

    // Callback for the whole arithmetic node
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

    // Get the expression string
    std::string exprText;
    if (expressionString) {
        // This is the root node with the full expression
        exprText = *expressionString;
    } else if (parent) {
        // Find root to get expression string
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
            result = !GetValue<bool>(childResult);
            if (callback) {
                callback(expression, path, depth, GetTypeString(), result);
            }
            return result;
        }

        case Type::And: {
            // Short-circuit evaluation over all children
            bool accum = true;
            for (size_t i = 0; i < children.size(); ++i) {
                auto childResult = children[i]->Evaluate(path + "." + std::to_string(i), depth + 1, callback);
                if (!GetValue<bool>(childResult)) {
                    accum = false;
                    break;
                }
            }
            result = accum;
            if (callback) {
                callback(expression, path, depth, GetTypeString() + (GetValue<bool>(result) ? "" : " (short-circuit)"),
                         result);
            }
            return result;
        }

        case Type::Or: {
            // Short-circuit evaluation over all children
            bool accum = false;
            for (size_t i = 0; i < children.size(); ++i) {
                auto childResult = children[i]->Evaluate(path + "." + std::to_string(i), depth + 1, callback);
                if (GetValue<bool>(childResult)) {
                    accum = true;
                    break;
                }
            }
            result = accum;
            if (callback) {
                callback(expression, path, depth, GetTypeString() + (GetValue<bool>(result) ? " (short-circuit)" : ""),
                         result);
            }
            return result;
        }

        case Type::Comparison: {
            auto leftResult = children[0]->Evaluate(path + ".0", depth + 1, callback);
            auto rightResult = children[1]->Evaluate(path + ".1", depth + 1, callback);

            auto compare = [&](auto a, auto b) -> bool {
                using A = decltype(a);
                using B = decltype(b);

                // Compare booleans only for equality/inequality.
                if constexpr (std::is_same_v<A, bool> && std::is_same_v<B, bool>) {
                    if (operation == "==")
                        return a == b;
                    if (operation == "!=")
                        return a != b;
                    throw std::runtime_error("Unsupported operator for booleans: " + operation + GetExprErrorContext());
                }
                // Compare any integral types (int and uint8_t included) by promoting to int.
                else if constexpr (std::is_integral_v<A> && std::is_integral_v<B>) {
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
            bool cond = GetValue<bool>(condResult);

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

    return false;
}

ExpressionEvaluation EvaluateExpression(std::string condition) {
    return EvaluateExpression(LogicExpression::Parse(condition));
}

ExpressionEvaluation EvaluateExpression(std::shared_ptr<LogicExpression> expression) {
    // Create a vector to store the evaluation sequence
    std::vector<
        std::tuple<std::shared_ptr<LogicExpression>, std::string, int, std::string, LogicExpression::ValueVariant>>
        evaluationSequence;

    // Define a callback that records each evaluation step
    auto recordCallback = [&evaluationSequence](const std::shared_ptr<LogicExpression>& expr, const std::string& path,
                                                int depth, const std::string& type,
                                                const LogicExpression::ValueVariant& result) {
        evaluationSequence.emplace_back(expr, path, depth, type, result);
    };

    // Evaluate the expression with the callback
    auto finalResult = expression->Evaluate<LogicExpression::ValueVariant>(recordCallback);

    // Helper function to convert path string to a vector of integers for sorting
    auto pathToVector = [](const std::string& path) {
        std::vector<int> result;
        std::stringstream ss(path);
        std::string segment;

        while (std::getline(ss, segment, '.')) {
            try {
                result.push_back(std::stoi(segment));
            } catch (const std::exception&) {
                // If it's not a valid integer, just skip it
                result.push_back(0);
            }
        }

        return result;
    };

    // Sort the evaluation sequence by path
    std::sort(evaluationSequence.begin(), evaluationSequence.end(), [&pathToVector](const auto& a, const auto& b) {
        const auto& pathA = std::get<1>(a);
        const auto& pathB = std::get<1>(b);

        auto vecA = pathToVector(pathA);
        auto vecB = pathToVector(pathB);

        // Compare each component of the path
        size_t i = 0;
        while (i < vecA.size() && i < vecB.size()) {
            if (vecA[i] != vecB[i]) {
                return vecA[i] < vecB[i];
            }
            i++;
        }

        // If one path is a prefix of the other, the shorter one comes first
        return vecA.size() < vecB.size();
    });

    ExpressionEvaluation evaluation;
    evaluation.Expression = std::get<0>(evaluationSequence[0]);
    evaluation.Depth = std::get<2>(evaluationSequence[0]);
    evaluation.Type = std::get<3>(evaluationSequence[0]);
    evaluation.Result = std::get<4>(evaluationSequence[0]);

    // Stack to keep track of parent nodes at each depth
    std::stack<ExpressionEvaluation*> parentStack;
    parentStack.push(&evaluation);

    // Process remaining evaluations to build the tree
    for (size_t i = 1; i < evaluationSequence.size(); ++i) {
        ExpressionEvaluation child;
        child.Expression = std::get<0>(evaluationSequence[i]);
        child.Depth = std::get<2>(evaluationSequence[i]);
        child.Type = std::get<3>(evaluationSequence[i]);
        child.Result = std::get<4>(evaluationSequence[i]);

        // Pop parents from stack if we're at a shallower depth
        while (!parentStack.empty() && parentStack.top()->Depth >= child.Depth) {
            parentStack.pop();
        }

        // Add child to current parent
        if (!parentStack.empty()) {
            parentStack.top()->Children.push_back(std::move(child));
            // If this child might have children, push it onto the stack
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
    } else {
        return "unknown";
    }
}
