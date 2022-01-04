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
private:
    void putIntro() {
        std::cout << ".intel_syntax noprefix\n";
        std::cout << ".global main\n";
        std::cout << " main:\n";
   }
};

