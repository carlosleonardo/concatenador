#include <iostream>
#include <fmt/format.h>
#include <boost/program_options.hpp>
#include <string>
#include <fstream>

namespace po = boost::program_options;

bool arquivoValido(const std::string &filename)
{
    std::ifstream f(filename.c_str());
    return f.good();
}

bool verificarExistencia(const std::vector<std::string> &arquivos)
{
    for (const auto &arquivo : arquivos)
    {
        fmt::print("Verificando existência do arquivo {}\n", arquivo);
        if (!arquivoValido(arquivo))
        {
            fmt::print("Arquivo {} não existe.\n", arquivo);
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv)
{
    po::options_description desc("Opções permitidas");
    desc.add_options()("ajuda,a", "Exibe esta mensagem de ajuda");
    desc.add_options()("arquivos-origem,O", po::value<std::vector<std::string>>()->multitoken(), "Arquivos texto de origem");
    desc.add_options()("arquivo-destino,D", po::value<std::string>(), "Arquivo texto de destino");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("ajuda") || vm.empty())
    {
        std::ostringstream os;
        os << desc;
        fmt::print("{}\n", os.str());
        return 0;
    }
    if (vm.count("arquivos-origem"))
    {
        const auto arquivosOrigem = vm["arquivos-origem"].as<std::vector<std::string>>();

        // Verifica se os arquivos existem
        if (!verificarExistencia(arquivosOrigem))
        {
            return -1;
        }
        if (vm.count("arquivo-destino"))
        {
            const auto arquivoDestino = vm["arquivo-destino"].as<std::string>();
            std::ofstream arqDestino(arquivoDestino);
            for (const auto &arquivo : arquivosOrigem)
            {
                std::ifstream arquivoOrigem(arquivo);
                std::string linha;
                while (std::getline(arquivoOrigem, linha))
                {
                    arqDestino << linha << std::endl;
                }
            }
        }
        else
        {
            fmt::print("Nenhum arquivo de destino foi especificado.\n");
            return -2;
        }
    }

    return 0;
}
