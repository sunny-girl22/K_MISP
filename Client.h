/**
* @file Client.h
* @author  Фейгина Е.А.
* @version 1.0
* @date 27.02.2023
* @copyright ИБСТ ПГУ
* @brief Заголовочный файл для модуля Client
*/
#include <iostream>
#include <fstream>
#include <unistd.h> //close()
#include <arpa/inet.h>

using namespace std;

/**
* @brief Класс для подключения к серверу
* @param port - порт
* @param sum - сумма
* @param msg - отправляемый хэш
* @param vector_file - имя файла для считывания векторов
* @param resultat_file - имя файла для результата
* @param autf_file - файл с логином и паролем клиента
* @param username - логин клиента
* @param line - строка из файла
*/

class Client
{
public:
    int Server(string str1, string str2);
    int port;
    int32_t sum;
    string msg;
    string vector_file;
    string resultat_file;
    string autf_file;
    string username;
    string pswd;
    string line;
};

/** @brief Класс обработки ошибок client_error
* @details Класс выводящий сообщения об ошибках
*/

class client_error: public invalid_argument
{
public:
    explicit client_error (const string& what_arg):
        invalid_argument(what_arg) {}
    explicit client_error (const char* what_arg):
        invalid_argument(what_arg) {}
};
