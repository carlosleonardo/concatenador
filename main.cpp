#include <iostream>
#include <fmt/format.h>
#include <boost/program_options.hpp>
#include <string>
#include <fstream>
#include <regex>
#include <filesystem>
#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#endif

namespace po = boost::program_options;

/// @brief Verifica se o arquivo é válido e existe
/// @param filename nome do arquivo
/// @return true se o arquivo é válido e existe
bool arquivoValido(const std::string &filename)
{
    std::ifstream f(filename.c_str());
    return f.good();
}

/// @brief Verifica se o arquivo é válido e existe
/// @param arquivos vetor de arquivos
/// @return true se todos os arquivos são válidos e existem
bool verificarExistencia(const std::vector<std::string> &arquivos)
{
    return std::all_of(arquivos.begin(), arquivos.end(), [](const std::string &arquivo)
                       {
        fmt::print("Verificando existência de arquivo {}\n", arquivo);                        
        if(!arquivoValido(arquivo)) {
            fmt::print("Arquivo {} não existe ou é inválido.\n", arquivo);
            return false;
        }
        return true; });
}

/// @brief Copia os arquivos de origem para o arquivo de destino
/// @param arquivosOrigem vetor de arquivos de origem
/// @param arquivoDestino arquivo de destino
/// @return
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

/// @brief Verifica se o tamanho do comando é válido
/// @param argc contagem de argumentos
/// @param argv array de argumentos
/// @return false se o tamanho do comando é inválido ou não for suportado pelo sistema operacional
bool verificarTamanhoComando(int argc, char **argv)
{
#ifdef _WIN32
    const size_t tamanhoMaximo = 32767;
#elif defined(__linux__) || defined(__APPLE__)
    const size_t maxArg = sysconf(_SC_ARG_MAX);
    if (maxArg == -1)
    {
        return false;
    }
    size_t tamanhoMaximo = maxArg - 1024;
#else
    return false;
#endif
    std::string comando = "";

    for (int i = 0; i < argc; i++)
    {
        comando += argv[i];
        if (i < argc - 1)
        {
            comando += " ";
        }
    }
    if (comando.size() > tamanhoMaximo)
    {
        return false;
    }
    return true;
}

std::vector<std::string> obterArquivosDaMascara(const std::string &mascara)
{
    std::vector<std::string> arquivos{};
    const auto caminho = std::filesystem::path(mascara);
    const auto diretorio = caminho.has_parent_path() ? caminho.parent_path() : std::filesystem::current_path();

    std::string padrao = mascara;
    padrao = std::regex_replace(padrao, std::regex("\\*"), ".*");
    padrao = std::regex_replace(padrao, std::regex("\\?"), ".");
    padrao = std::regex_replace(padrao, std::regex("\\."), "\\.");

    // Extrai todos os arquivos da pasta que combinam com a máscara
    for (const auto &arquivo : std::filesystem::directory_iterator(diretorio))
    {
        // Verifica se o arquivo é um arquivo regular e se o nome do arquivo combina com a máscara
        // fmt::print("Verificando arquivo {}\n", arquivo.path().string());
        if (arquivo.is_regular_file())
            if (std::regex_match(arquivo.path().filename().string(), std::regex(padrao)))
            {
                fmt::print("Arquivo {} é válido\n", arquivo.path().string());
                arquivos.push_back(arquivo.path().string());
            }
    }
    return arquivos;
}

int main(int argc, char **argv)
{
    po::options_description desc("Opções permitidas");
    desc.add_options()("ajuda,a", "Exibe esta mensagem de ajuda");
    desc.add_options()("arquivos-origem,O", po::value<std::vector<std::string>>()->multitoken(), "Arquivos texto de origem");
    desc.add_options()("arquivo-destino,D", po::value<std::string>(), "Arquivo texto de destino");

    if (verificarTamanhoComando(argc, argv) == false)
    {
        fmt::print("Comando excede o máximo na linha.\n");
        return -6;
    }
    try
    {

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
            // Obtém os arquivos de origem
            auto arquivosOrigem = vm["arquivos-origem"].as<std::vector<std::string>>();
            std::vector<std::string> arquivosFinal;

            // Process each input argument - could be a wildcard or regular file
            for (const auto &entrada : arquivosOrigem)
            {
                auto arquivosFiltrados = obterArquivosDaMascara(entrada);
                if (!arquivosFiltrados.empty())
                {
                    // If wildcard matched files, add them
                    arquivosFinal.insert(arquivosFinal.end(),
                                         arquivosFiltrados.begin(),
                                         arquivosFiltrados.end());
                }
                else if (arquivoValido(entrada))
                {
                    // If it's a regular file that exists, add it
                    arquivosFinal.push_back(entrada);
                }
            }

            if (arquivosFinal.empty())
            {
                fmt::print("Nenhum arquivo válido encontrado nos argumentos fornecidos.\n");
                return -7;
            }

            if (!verificarExistencia(arquivosFinal))
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
    }
    catch (const std::exception &e)
    {
        fmt::print("Erro ao executar o programa: {}\n", e.what());
        return -1;
    }
    catch (...)
    {
        fmt::print("Erro desconhecido ao executar o programa.\n");
        return -1;
    }

    return 0;
}
