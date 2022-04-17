extern "C" {
#include "ast.h"
}

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <fstream>

std::unique_ptr<llvm::LLVMContext> llvm_context;
std::unique_ptr<llvm::IRBuilder<>> llvm_builder;

static void visit(llvm::Module* llvm_module, stat_t* self)
{
    if (self->type == stat_type_print) {

        auto* F = llvm_module->getFunction("printf");

        auto* value
            = llvm::ConstantDataArray::getString(*llvm_context, self->sz, true);
        auto* ptr = llvm_builder->CreateAlloca(value->getType(),
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(*llvm_context), 1),
            "param");
        // auto *value =
        // llvm::ConstantInt::get(llvm::Type::getInt8Ty(*llvm_context), 97);
        llvm_builder->CreateStore(value, ptr);
        std::vector<llvm::Value*> emptyArgs;
        emptyArgs.push_back(ptr);
        llvm_builder->CreateCall(F, llvm::makeArrayRef(emptyArgs));
    }
}

static void visit(llvm::Module* llvm_module, statlist_t* self)
{
    if (self->statlist) {
        visit(llvm_module, self->statlist);
    }

    if (self->stat) {
        visit(llvm_module, self->stat);
    }
}

static void visit(llvm::Module* llvm_module, block_t* self)
{
    visit(llvm_module, self->statlist);
}

static void visit(module_t* self)
{
    auto llvm_module
        = std::make_unique<llvm::Module>("my cool jit", *llvm_context);

    {
        auto* FT = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(*llvm_context), true);

        auto* F = llvm::Function::Create(
            FT, llvm::Function::ExternalLinkage, "printf", llvm_module.get());
    }

    auto* FT
        = llvm::FunctionType::get(llvm::Type::getVoidTy(*llvm_context), false);

    auto* F = llvm::Function::Create(
        FT, llvm::Function::ExternalLinkage, "main", llvm_module.get());

    auto* BB = llvm::BasicBlock::Create(*llvm_context, "entry", F);
    llvm_builder->SetInsertPoint(BB);

    visit(llvm_module.get(), self->block);

    llvm_builder->CreateRetVoid();

    llvm_module->print(llvm::outs(), nullptr);
}

extern void codegen(module_t* module)
{
    llvm_context = std::make_unique<llvm::LLVMContext>();
    llvm_builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
    visit(module);
    llvm_builder = nullptr;
    llvm_context = nullptr;
}