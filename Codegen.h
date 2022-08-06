#pragma once

#include "Ast.h"

class Codegen {

public:
   void generate(const Ast::Node& node) {
       putIntro();
       std::visit(*this, node.var);
       std::cout << " pop rax \n";
       std::cout << " ret \n";
   }

   void operator()(const Ast::Operation& obj) {
       std::visit(*this, obj.left->var);
       std::visit(*this, obj.right->var);

       std::cout << " pop rdi \n";
       std::cout << " pop rax \n";

       switch (obj.kind) {
           case Ast::Operation::Add:
               std::cout << " add rax, rdi \n";
               break;

           case Ast::Operation::Sub:
               std::cout << " sub rax, rdi \n";
               break;

           case Ast::Operation::Mul:
               std::cout << " imul rax, rdi \n";
               break;
           case Ast::Operation::Div:
               std::cout << " cqo\n";
               std::cout << " idiv rdi \n";
               break;
       }

       std::cout << " push rax\n";
   }

    void operator()(const Ast::Num& num) {
       std::cout << " push " << num.value << "\n";
   }

    void operator()(const Ast::Condition& cond) {
        std::visit(*this, cond.left->var);
        std::visit(*this, cond.right->var);

        std::cout << " pop rdi \n";
        std::cout << " pop rax \n";
        std::cout << " cmp rax, rdi \n";
        switch (cond.kind) {
            case Ast::Condition::Eq:
                std::cout << " sete al \n";
                break;
            case Ast::Condition::NotEq:
                std::cout << " setne al \n";
                break;
            case Ast::Condition::Less:
                std::cout << " setl al \n";
                break;
            case Ast::Condition::Greater:
                std::cout << " setg al \n";
                break;
            case Ast::Condition::LessEq:
                std::cout << " setle al \n";
                break;
            case Ast::Condition::GreaterEq:
                std::cout << " setge al \n";
                break;
        }
        std::cout << " movzb rax, al \n";
        std::cout << " push rax \n";
    }
private:
    void putIntro() {
        std::cout << ".intel_syntax noprefix\n";
        std::cout << ".global main\n";
        std::cout << " main:\n";
   }
};

