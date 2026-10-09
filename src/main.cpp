/**
 * @file main.cpp
 * @brief Command-line front end for the Logic in CS Assignment 1 tool.
 */
#include "logic_tree.hpp"
#include <iostream>

namespace {
void usage(const char* program) {
    std::cout << "Usage:\n  " << program << " --formula \"((p*q)>r)\"\n  " << program << " --cnf benchmark.cnf\n"
              << "Connectives: ~ negation, + OR, * AND, > implication.\n";
}

void print_truth_table(const logic::Node* root, const std::string& formula) {
    const auto names = logic::atoms(root); const auto rows = logic::truth_table(root, names);
    std::cout << "\nTruth table\n";
    for (const auto& name : names) std::cout << name << '\t';
    std::cout << formula << "\n";
    for (const auto& row : rows) { for (bool value : row.values) std::cout << (value ? 'T' : 'F') << '\t'; std::cout << (row.result ? 'T' : 'F') << '\n'; }
}

void report(const std::string& formula) {
    const std::string prefix = logic::infix_to_prefix(formula);
    auto tree = logic::prefix_to_tree(prefix);
    std::cout << "Input infix:               " << formula << "\n"
              << "Task 1 - prefix:           " << prefix << "\n"
              << "Task 2 - tree root:        " << tree->value << "\n"
              << "Task 3 - inorder recursive:" << logic::inorder_recursive(tree.get()) << "\n"
              << "Task 3 - inorder iterative:" << logic::inorder_iterative(tree.get()) << "\n"
              << "Task 3 - preorder recursive: " << logic::join_tokens(logic::preorder_recursive(tree.get())) << "\n"
              << "Task 3 - preorder iterative: " << logic::join_tokens(logic::preorder_iterative(tree.get())) << "\n"
              << "Task 3 - postorder recursive:" << logic::join_tokens(logic::postorder_recursive(tree.get())) << "\n"
              << "Task 3 - postorder iterative:" << logic::join_tokens(logic::postorder_iterative(tree.get())) << "\n"
              << "Task 4 - height recursive: " << logic::height_recursive(tree.get()) << "\n"
              << "Task 4 - height iterative: " << logic::height_iterative(tree.get()) << "\n";
    print_truth_table(tree.get(), logic::inorder_recursive(tree.get()));
}
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) { usage(argv[0]); return 1; }
        if (std::string(argv[1]) == "--formula") report(argv[2]);
        else if (std::string(argv[1]) == "--cnf") { const std::string formula = logic::dimacs_to_infix(argv[2]); std::cout << "DIMACS converted to: " << formula << "\n"; report(formula); }
        else { usage(argv[0]); return 1; }
        return 0;
    } catch (const std::exception& error) { std::cerr << "Error: " << error.what() << '\n'; return 2; }
}
