extern "C" {
#include "ast.h"
}

#include <variant>
#include <sstream>
#include <string>

#include "cxxopts.h"

extern void codegen(module_t* module);

int main(int argc, char* argv[])
{
    cxxopts::Options options("nxsc", "nxsc compile a .nxs file");
    options.add_options()("f,file", "File name", cxxopts::value<std::string>())(
        "v,verbose", "Verbose output",
        cxxopts::value<bool>()->default_value("false"));

    auto result = options.parse(argc, argv);
    auto filename = result["file"].as<std::string>();

    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        fprintf(stderr, "open %s fail\n", filename.c_str());
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    auto size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::vector<char> buffer;
    buffer.resize(size);
    fread(buffer.data(), 1, size, fp);
    fclose(fp);
    module_t* module = nxs_parse((char*)buffer.data());
    // // print_ast_root(block, std::cout);

    codegen(module);

    destroy_module(module);
    return 0;
}