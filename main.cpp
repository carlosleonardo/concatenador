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
            fmt::print("Arquivo {} não existe ou é inválido.\n", arquivo);
            return false;
        }
    }
    return true;
}

int copiarArquivos(const std::vector<std::string> &arquivosOrigem, const std::string &arquivoDestino)
{
    std::ofstream arqDestino(arquivoDestino);
    if (!arqDestino.is_open())
    {
        fmt::print("Não foi possível abrir o arquivo de destino {} para gravação.\n", arquivoDestino);
        return -3;
    }
    for (const auto &arquivo : arquivosOrigem)
    {
        std::ifstream arquivoOrigem(arquivo);
        if (!arquivoOrigem.is_open())
        {
            fmt::print("Não foi possível abrir o arquivo de origem {}.\n", arquivo);
            return -4;
        }
        try
        {
            std::string linha;
            // Copia o conteúdo do arquivo de origem para o arquivo de destino
            arqDestino.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            while (std::getline(arquivoOrigem, linha))
            {
                arqDestino << linha << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            arquivoOrigem.close();
            arqDestino.close();
            fmt::print("Erro ao copiar o arquivo {}: {}\n", arquivo, e.what());
            return -5;
        }
    }
    return 0;
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
            return copiarArquivos(arquivosOrigem, arquivoDestino);
        }
        else
        {
            fmt::print("Nenhum arquivo de destino foi especificado.\n");
            return -2;
        }
    }

    return 0;
}
