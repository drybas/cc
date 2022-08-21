#pragma once

#include "Ast.h"
#include "Error.h"

class Codegen {

public:
    void generate_intro(size_t local_var_counts) {
        std::cout << ".intel_syntax noprefix\n";
        std::cout << ".global main\n";
        std::cout << " main:\n";
        std::cout << " push rbp\n";
        std::cout << " mov rbp, rsp\n";
        std::cout << " sub rsp, "<< local_var_counts * 8 << "\n";
    }

    [[nodiscard]] auto generate(const Ast::Node& node) -> bool {
        std::visit(*this, node.var);
        std::cout << " pop rax \n";
        return m_continue;
    }

    void generate_outro() {
        if (m_comments)
            std::cout << " ; outro \n";

        std::cout << " mov rsp, rbp\n";
        std::cout << " pop rbp\n";
        std::cout << " ret \n";
    }

    void operator()(const Ast::Operation& obj) {
        std::visit(*this, obj.left->var);
        std::visit(*this, obj.right->var);

        if (m_comments)
            std::cout << " ; operation \n";

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
        if (m_comments)
            std::cout << " ; handle number \n";

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

    void operator()(const Ast::LVar& lvar) {
        if (m_comments)
            std::cout << " ; load local variable \n";

        get_addr(lvar);
        std::cout << " pop rax \n";
        std::cout << " mov rax, [rax]\n";
        std::cout << " push rax\n";
    }

    void operator()(const Ast::Assign& assign) {
        if (m_comments)
            std::cout << " ; assign for " << std::get_if<Ast::LVar>(&assign.left->var)->name << "= \n";

        get_addr(assign.left->var);
        std::visit(*this, assign.right->var);

        if (m_comments)
            std::cout << " ; assign next \n";

        std::cout << " pop rdi\n";
        std::cout << " pop rax\n";
        std::cout << " mov [rax], rdi\n";
        std::cout << " push rdi\n";

        if (m_comments)
            std::cout << " ; assign done \n";
    }

    void operator()(const Ast::Return& ret) {
        std::visit(*this, ret.left->var);
        m_continue = false;
    }

private:
    void get_addr(const Ast::NodeVar& var) {

        if (const Ast::LVar* lvar = std::get_if<Ast::LVar>(&var)) {
            std::cout << " lea rax, [rbp - " << lvar->index * 8 << "]\n";
            std::cout << " push rax \n";
        } else {
            Error::raise(Error::ExpectLvalue);
        }
    }

private:
    bool m_comments = false;
    bool m_continue = true;
};

