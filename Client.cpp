/**
 * @file Client.cpp
 * @brief Файл взаимодействие с сервером
 */

#include "md5.h"
#include "Client.h"

/**
* @brief Взаимодействие с сервером
* @param str1 - адрес сервера
* @param str2 - порт сервера
* @throw client_error класс вызываемый при возникновении ошибки
*/

int Client::Server(string str1, string str2)
{
    if(str2 == "") {
        str2 = "33333";
    }

    if(autf_file == "") {
        autf_file = "./config/vclient.conf";
    }

    // Открытие файла для аутентификации
    ifstream fautf(autf_file);

    if(!fautf.is_open()) {
        throw client_error(string("fun:Server, param:autf_file.\nОшибка отрытия файла для аутентификация"));
    }
    if(fautf.peek() == EOF) {
        fautf.close();
        throw client_error(string("fun:Server, param:autf_file.\nФайл для аутентификация пуст"));
    }

    getline(fautf, line);
    int k = line.find(" ");
    username = line.substr(0, k);
    pswd = line.erase(0, k+1);

    // Открытие файла для чтения векторов
    ifstream fvector(vector_file);

    if(!fvector.is_open()) {
        fvector.close();
        throw client_error(string("fun:Server, param:vector_file.\nОшибка отрытия исходного файла с векторами"));
    }
    if(fvector.peek() == EOF) {
        fvector.close();
        throw client_error(string("fun:Server, param:vector_file.\nФайл с векторами пуст"));
    }
    // Открытие файла для записи суммы
    ofstream fresultat(resultat_file);

    if(!fresultat.is_open()) {
        fresultat.close();
        throw client_error(string("fun:Server, param:resultat_file.\nОшибка отрытия файла для результатов"));
    }

    char buf[255];

    try {
        
        strcpy(buf,str1.c_str());
    } catch (...) {
        throw client_error(std::string("fun:Server, param:buf.\nНевозможно получить адрес"));
    }

    try {

        port = stoi(str2);
    } catch (...) {
        throw client_error(string("fun:Server, param:port.\nНевозможно получить порт"));
    }

    //Соединение клиента с сервером
    
    
    sockaddr_in * selfAddr = new (sockaddr_in);
    selfAddr->sin_family = AF_INET;
    selfAddr->sin_port = 0;
    selfAddr->sin_addr.s_addr = 0;

    sockaddr_in * remoteAddr = new (sockaddr_in);
    remoteAddr->sin_family = AF_INET;
    remoteAddr->sin_port = htons(port);
    remoteAddr->sin_addr.s_addr = inet_addr(buf);

    int mySocket = socket(AF_INET, SOCK_STREAM, 0);
    if(mySocket == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:mySocket.\nОшибка создания сокета"));
    }

    int rc = bind(mySocket,(const sockaddr *) selfAddr, sizeof(sockaddr_in));
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:selfAddr.\nОшибка привязки сокета с локальным адресом"));
    }

    rc = connect(mySocket, (const sockaddr*) remoteAddr, sizeof(sockaddr_in));
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:remoteAddr.\nОшибка подключения сокета к удаленному серверу"));
    }

    char *buffer = new char[4096];
    strcpy(buffer,username.c_str());
    int msgLen = strlen(buffer);

    rc = send(mySocket, buffer, msgLen, 0);
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:buffer.\nОшибка оправки логина"));
    }
    cout << "Мы отправляем логин: " << buffer << endl;

    rc = recv(mySocket, buffer, 4096, 0);
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:buffer.\nОшибка получения ответа"));
    }
    string s1 = string(buffer);
    buffer[rc] = '\0';
    cout << "Мы получаем соль: " << buffer << endl;

    string hsh = s1 + pswd;
    msg = MD5_hash(hsh);

    strcpy(buffer,msg.c_str());
    rc = send(mySocket, buffer, msg.length(), 0);
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:msg.\nОшибка оправки хэша"));
    }
    cout << "Мы отправляем хэш: " << buffer << endl;

    rc = recv(mySocket, buffer, sizeof(buffer), 0);
    buffer[rc] = '\0';
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:buffer.\nОшибка получения ответа об аунтефикации"));
    }
    cout << "Мы получаем ответ: " << buffer << endl;

    getline(fvector, line);
    int len = stoi(line);

    rc = send(mySocket, &len, sizeof(len), 0);
    if (rc == -1) {
        close(mySocket);
        throw client_error(string("fun:Server, param:buffer.\nОшибка оправки кол-ва векторов"));
    }
    cout << "Мы отправляем кол-во векторов: " << len << endl;


    for(int l = 0; l < len; l++) {
        getline(fvector, line);

        // Получение вектора(сначала длина, потом его значений)
        
        int r=0;
        int strl = line.length();
        for (int k=0; k<strl; k++)
            if (line[k]==' ') r++;
        int size = r+1;

        uint32_t array[size] = {0};
        for(int j = 0; j < size; j++) {
            string a;
            int i = line.find(" ");
            a = line.substr(0, i); 
            line = line.erase(0, i+1);
            array[j] = stod(a);
        }

        //Отправка векторов
        
        
        rc = send(mySocket, &size, sizeof(size), 0);
        if (rc == -1) {
            close(mySocket);
            throw client_error(string("fun:Server, param:buffer.\nОшибка оправки размера векторов"));
        }
        cout << "Мы отправляем размер вектора: " << size << endl;

        rc = send(mySocket, &array, sizeof(array), 0);
        if (rc == -1) {
            close(mySocket);
            throw client_error(string("fun:Server, param:buffer.\nОшибка оправки самих векторов"));
        }
        cout << "Мы отправляем сам вектор: " << array << endl;

        // Получение результата
        rc = recv(mySocket, &sum, sizeof(sum), 0);
        if (rc == -1) {
            close(mySocket);
            throw client_error(string("fun:Server, param:buffer.\nОшибка получения ответа в виде суммы"));
        }
        cout << "Мы получаем результат вычисления: " << sum << endl;
        FILE *y;
        const char *h = resultat_file.c_str();
        y=fopen(h, "r+b");
        fwrite(&sum, sizeof(uint32_t), 1, y);
    }

    close(mySocket);

    delete selfAddr;
    delete remoteAddr;
    delete[] buffer;
    return 0;
}
