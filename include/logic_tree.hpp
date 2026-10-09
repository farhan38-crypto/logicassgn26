/**
 * @file logic_tree.hpp
 * @brief Parse-tree algorithms for fully parenthesized propositional logic.
 */
#ifndef LOGIC_TREE_HPP
#define LOGIC_TREE_HPP

#include <algorithm>
#include <cctype>
#include <fstream>
#include <functional>
#include <memory>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace logic {

/** A node in the rooted parse tree.  A negation node uses only left. */
struct Node {
    std::string value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    explicit Node(std::string node_value) : value(std::move(node_value)) {}
    bool is_operator() const { return value == "~" || value == "+" || value == "*" || value == ">"; }
    bool is_unary() const { return value == "~"; }
};

/** Holds the source text and cursor while parsing a fully parenthesized formula. */
class InfixParser {
public:
    explicit InfixParser(const std::string& input) : input_(input) {}

    std::unique_ptr<Node> parse() {
        auto root = parse_expression();
        skip_space();
        if (position_ != input_.size()) {
            fail("unexpected text after a complete formula");
        }
        return root;
    }

private:
    const std::string& input_;
    std::size_t position_ = 0;

    void skip_space() {
        while (position_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[position_]))) ++position_;
    }

    char peek() {
        skip_space();
        return position_ < input_.size() ? input_[position_] : '\0';
    }

    [[noreturn]] void fail(const std::string& message) const {
        throw std::invalid_argument("Parse error at character " + std::to_string(position_ + 1) + ": " + message);
    }

    std::unique_ptr<Node> parse_expression() {
        const char current = peek();
        if (current == '~') {
            ++position_;
            auto node = std::make_unique<Node>("~");
            node->left = parse_expression();
            return node;
        }
        if (current == '(') {
            ++position_;
            if (peek() == '~') { // also accept (~P), which is common in fully parenthesized input
                ++position_;
                auto node = std::make_unique<Node>("~");
                node->left = parse_expression();
                if (peek() != ')') fail("expected ')' after negation");
                ++position_;
                return node;
            }
            auto left = parse_expression();
            const char op = peek();
            if (op != '+' && op != '*' && op != '>') fail("expected binary connective (+, *, >)");
            ++position_;
            auto right = parse_expression();
            if (peek() != ')') fail("expected ')' after binary expression");
            ++position_;
            auto node = std::make_unique<Node>(std::string(1, op));
            node->left = std::move(left);
            node->right = std::move(right);
            return node;
        }
        if (std::isalnum(static_cast<unsigned char>(current)) || current == '_') {
            const std::size_t start = position_;
            while (position_ < input_.size() &&
                   (std::isalnum(static_cast<unsigned char>(input_[position_])) || input_[position_] == '_')) ++position_;
            return std::make_unique<Node>(input_.substr(start, position_ - start));
        }
        fail("expected an atom, '~', or '('");
    }
};

/** Converts a formula into whitespace-separated prefix notation (Task 1). */
inline std::string infix_to_prefix(const std::string& infix) {
    auto tree = InfixParser(infix).parse();
    std::vector<std::string> tokens;
    std::function<void(const Node*)> visit = [&](const Node* node) {
        tokens.push_back(node->value);
        if (node->left) visit(node->left.get());
        if (node->right) visit(node->right.get());
    };
    visit(tree.get());
    std::ostringstream out;
    for (std::size_t i = 0; i < tokens.size(); ++i) out << (i ? " " : "") << tokens[i];
    return out.str();
}

/** Builds a parse tree from whitespace-separated prefix notation (Task 2). */
inline std::unique_ptr<Node> prefix_to_tree(const std::string& prefix) {
    std::istringstream input(prefix);
    std::vector<std::string> tokens;
    for (std::string token; input >> token;) tokens.push_back(token);
    if (tokens.empty()) throw std::invalid_argument("Prefix expression is empty");
    std::size_t index = 0;
    std::function<std::unique_ptr<Node>()> build = [&]() -> std::unique_ptr<Node> {
        if (index == tokens.size()) throw std::invalid_argument("Prefix expression ended before all operands were supplied");
        auto node = std::make_unique<Node>(tokens[index++]);
        if (node->value == "~") node->left = build();
        else if (node->value == "+" || node->value == "*" || node->value == ">") {
            node->left = build();
            node->right = build();
        }
        return node;
    };
    auto root = build();
    if (index != tokens.size()) throw std::invalid_argument("Prefix expression has extra tokens");
    return root;
}

/** Returns the fully parenthesized infix formula by recursive inorder traversal. */
inline std::string inorder_recursive(const Node* node) {
    if (!node->is_operator()) return node->value;
    if (node->is_unary()) return "(~" + inorder_recursive(node->left.get()) + ")";
    return "(" + inorder_recursive(node->left.get()) + node->value + inorder_recursive(node->right.get()) + ")";
}

/** Returns the fully parenthesized infix formula by iterative inorder traversal. */
inline std::string inorder_iterative(const Node* root) {
    struct Frame { const Node* node; int stage; };
    std::stack<Frame> work;
    work.push({root, 0});
    std::string result;
    while (!work.empty()) {
        Frame& frame = work.top();
        const Node* node = frame.node;
        if (!node->is_operator()) { result += node->value; work.pop(); continue; }
        if (node->is_unary()) {
            if (frame.stage++ == 0) { result += "(~"; work.push({node->left.get(), 0}); }
            else { result += ")"; work.pop(); }
            continue;
        }
        if (frame.stage == 0) { result += "("; frame.stage = 1; work.push({node->left.get(), 0}); }
        else if (frame.stage == 1) { result += node->value; frame.stage = 2; work.push({node->right.get(), 0}); }
        else { result += ")"; work.pop(); }
    }
    return result;
}

inline void preorder_recursive_impl(const Node* node, std::vector<std::string>& output) {
    output.push_back(node->value);
    if (node->left) preorder_recursive_impl(node->left.get(), output);
    if (node->right) preorder_recursive_impl(node->right.get(), output);
}

/** Returns a pre-order traversal recursively. */
inline std::vector<std::string> preorder_recursive(const Node* root) {
    std::vector<std::string> output; preorder_recursive_impl(root, output); return output;
}

/** Returns a pre-order traversal iteratively. */
inline std::vector<std::string> preorder_iterative(const Node* root) {
    std::vector<std::string> output; std::stack<const Node*> work; work.push(root);
    while (!work.empty()) {
        const Node* node = work.top();
        work.pop();
        output.push_back(node->value);
        if (node->right) work.push(node->right.get());
        if (node->left) work.push(node->left.get());
    }
    return output;
}

inline void postorder_recursive_impl(const Node* node, std::vector<std::string>& output) {
    if (node->left) postorder_recursive_impl(node->left.get(), output);
    if (node->right) postorder_recursive_impl(node->right.get(), output);
    output.push_back(node->value);
}

/** Returns a post-order traversal recursively. */
inline std::vector<std::string> postorder_recursive(const Node* root) {
    std::vector<std::string> output; postorder_recursive_impl(root, output); return output;
}

/** Returns a post-order traversal iteratively, using visited-state frames. */
inline std::vector<std::string> postorder_iterative(const Node* root) {
    std::vector<std::string> output; std::stack<std::pair<const Node*, bool>> work; work.push({root, false});
    while (!work.empty()) { auto [node, visited] = work.top(); work.pop();
        if (visited) output.push_back(node->value);
        else { work.push({node, true}); if (node->right) work.push({node->right.get(), false}); if (node->left) work.push({node->left.get(), false}); } }
    return output;
}

/** Computes the number of nodes on the longest root-to-leaf path recursively. */
inline int height_recursive(const Node* node) {
    if (!node) return 0;
    return 1 + std::max(height_recursive(node->left.get()), height_recursive(node->right.get()));
}

/** Computes parse-tree height iteratively with depth-first traversal. */
inline int height_iterative(const Node* root) {
    int height = 0; std::stack<std::pair<const Node*, int>> work; work.push({root, 1});
    while (!work.empty()) {
        auto [node, depth] = work.top();
        work.pop();
        height = std::max(height, depth);
        if (node->left) work.push({node->left.get(), depth + 1});
        if (node->right) work.push({node->right.get(), depth + 1});
    }
    return height;
}

/** Evaluates a tree bottom-up. Every atom in the formula must have an assignment. */
inline bool evaluate_bottom_up(const Node* root, const std::unordered_map<std::string, bool>& assignment) {
    std::stack<std::pair<const Node*, bool>> order; std::vector<const Node*> postorder; order.push({root, false});
    while (!order.empty()) { auto [node, seen] = order.top(); order.pop(); if (seen) postorder.push_back(node);
        else { order.push({node, true}); if (node->right) order.push({node->right.get(), false}); if (node->left) order.push({node->left.get(), false}); } }
    std::unordered_map<const Node*, bool> value;
    for (const Node* node : postorder) {
        if (!node->is_operator()) { auto found = assignment.find(node->value); if (found == assignment.end()) throw std::invalid_argument("No truth value supplied for atom '" + node->value + "'"); value[node] = found->second; }
        else if (node->value == "~") value[node] = !value.at(node->left.get());
        else if (node->value == "+") value[node] = value.at(node->left.get()) || value.at(node->right.get());
        else if (node->value == "*") value[node] = value.at(node->left.get()) && value.at(node->right.get());
        else value[node] = !value.at(node->left.get()) || value.at(node->right.get());
    }
    return value.at(root);
}

/** Collects distinct atomic propositions in lexical order. */
inline std::vector<std::string> atoms(const Node* root) {
    std::set<std::string> names; std::stack<const Node*> work; work.push(root);
    while (!work.empty()) { const Node* node = work.top(); work.pop(); if (!node->is_operator()) names.insert(node->value); else { if (node->left) work.push(node->left.get()); if (node->right) work.push(node->right.get()); } }
    return {names.begin(), names.end()};
}

/** Represents one truth-table assignment and its formula result. */
struct TruthRow { std::vector<bool> values; bool result; };

/** Generates all 2^n valuations, in lexical atom order, and evaluates each bottom-up. */
inline std::vector<TruthRow> truth_table(const Node* root, const std::vector<std::string>& names) {
    if (names.size() >= sizeof(std::size_t) * 8) throw std::invalid_argument("Too many atoms for an exhaustive truth table");
    const std::size_t count = std::size_t{1} << names.size(); std::vector<TruthRow> rows; rows.reserve(count);
    for (std::size_t mask = 0; mask < count; ++mask) { std::unordered_map<std::string, bool> assignment; std::vector<bool> values;
        for (std::size_t i = 0; i < names.size(); ++i) { const bool bit = (mask & (std::size_t{1} << (names.size() - 1 - i))) == 0; assignment[names[i]] = bit; values.push_back(bit); }
        rows.push_back({values, evaluate_bottom_up(root, assignment)}); }
    return rows;
}

/** Translates a DIMACS CNF file into the assignment's fully parenthesized infix notation. */
inline std::string dimacs_to_infix(const std::string& filename) {
    std::ifstream file(filename); if (!file) throw std::runtime_error("Could not open CNF file: " + filename);
    std::vector<std::vector<int>> clauses; std::vector<int> clause; std::string word;
    while (file >> word) { if (word == "c") { std::string ignored; std::getline(file, ignored); }
        else if (word == "p") { std::string format; int variables, clause_count; if (!(file >> format >> variables >> clause_count) || format != "cnf") throw std::invalid_argument("Invalid DIMACS problem line"); }
        else { int literal; try { literal = std::stoi(word); } catch (...) { throw std::invalid_argument("Invalid DIMACS token: " + word); }
            if (literal == 0) { clauses.push_back(clause); clause.clear(); } else clause.push_back(literal); } }
    if (!clause.empty()) throw std::invalid_argument("DIMACS clause is missing its terminating 0");
    if (clauses.empty()) throw std::invalid_argument("DIMACS file contains no clauses");
    auto join = [](const std::vector<std::string>& pieces, const std::string& connective) { std::string result = pieces.front(); for (std::size_t i = 1; i < pieces.size(); ++i) result = "(" + result + connective + pieces[i] + ")"; return result; };
    std::vector<std::string> rendered_clauses;
    for (const auto& input_clause : clauses) { if (input_clause.empty()) throw std::invalid_argument("Empty DIMACS clauses are not representable without a false constant"); std::vector<std::string> literals;
        for (int literal : input_clause) literals.push_back(literal < 0 ? "(~x" + std::to_string(-literal) + ")" : "x" + std::to_string(literal));
        rendered_clauses.push_back(join(literals, "+")); }
    return join(rendered_clauses, "*");
}

inline std::string join_tokens(const std::vector<std::string>& tokens) { std::ostringstream out; for (std::size_t i = 0; i < tokens.size(); ++i) out << (i ? " " : "") << tokens[i]; return out.str(); }

} // namespace logic
#endif
