#include <iostream>
#include <fmt/format.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char **argv)
{
    po::options_description desc("Opções permitidas");
    desc.add_options()("ajuda,a", "Exibe esta mensagem de ajuda");
    desc.add_options()("arquivos-origem,ao", po::value<std::string>(), "Arquivos texto de origem");
    desc.add_options()("arquivo-destino,ad", po::value<std::string>(), "Arquivo texto de destino");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm, true);
    po::notify(vm);

    if (vm.count("ajuda") || vm.empty())
    {
        std::ostringstream os;
        os << desc;
        fmt::print("{}\n", os.str());
        return 0;
    }
    return 0;
}
