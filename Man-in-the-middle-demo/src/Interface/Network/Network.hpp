#include<iostream>

#include<asio.hpp>
#include<asio/ssl.hpp>

#include<thread>

namespace Network
{
    class Socket
    {
        public:
            Socket(std::string_view Ip, unsigned int Port) : EndPoint(asio::ip::make_address(Ip), Port), AsioSocket(IO_Context)
            {

            }

            void AsyncConnect()
            {
                if(this->AsioSocket.is_open())
                {
                    std::clog << "Warning: Socket is open" << std::endl;
                    return;
                }

                AsioSocket.async_connect(EndPoint, [&](asio::error_code ErrorCode){
                    if(!ErrorCode)
                    {
                        return;
                    }
                    std::cerr << ErrorCode.message() << std::endl;
                });
            }

            void AsyncHost()
            {
                asio::ip::tcp::acceptor Acceptor(this->IO_Context, this->EndPoint);
                Acceptor.listen();
                Acceptor.async_accept(AsioSocket, [&](asio::error_code ErrorCode){
                    if(!ErrorCode)
                    {
                        return;
                    }
                    std::cerr << ErrorCode.message() << std::endl;
                });
            }

            void AsyncWrite(std::string_view Data)
            {
                AsioSocket.async_send(asio::buffer(Data), [&](asio::error_code ErrorCode, size_t BytesTransferred){
                    if(!ErrorCode)
                    {
                        return;
                    }
                    std::cerr << ErrorCode.message() << std::endl;
                });
            }

            void AsyncRead(void*(Handler)(std::string* Data, asio::error_code ErrorCode, size_t BytesGot))
            {
                asio::streambuf TempBuffer;

                asio::async_read_until(this->AsioSocket, TempBuffer, '\0', [&](asio::error_code ErrorCode, size_t BytesGot){
                    std::istream InputStream(&TempBuffer);
                    std::string Message;

                    if(!ErrorCode)
                    {
                        std::getline(InputStream, Message, '\0');

                        Handler(&Message, ErrorCode, BytesGot);

                        return;
                    }
                    std::cerr << ErrorCode.message() << std::endl;
                });
            }
            
            void Connect()
            {
                if(this->AsioSocket.is_open())
                {
                    std::clog << "Warning: Socket is open" << std::endl;
                    return;
                }

                AsioSocket.connect(this->EndPoint);
            }

            void Host()
            {
                if(this->AsioSocket.is_open())
                {
                    std::clog << "Warning: Socket is open" << std::endl;
                    return;
                }
                asio::ip::tcp::acceptor Acceptor(this->IO_Context, this->EndPoint);
                
                Acceptor.listen();
                Acceptor.accept(AsioSocket);
            }

            void Write(std::string_view Data)
            {
                std::string StringData = Data.data();

                StringData+='\0';

                AsioSocket.send(asio::buffer(StringData));
            }

            std::string Read()
            {
                asio::streambuf StreamBuffer;
                asio::read_until(AsioSocket, StreamBuffer, '\0');

                std::istream InputStream(&StreamBuffer);

                std::string Message;

                std::getline(InputStream, Message, '\0');

                return Message;
            }

            void Close()
            {
                this->AsioSocket.close();
            }
            
        private:
            asio::io_context IO_Context;

            asio::ip::tcp::socket AsioSocket;
            asio::ip::tcp::endpoint EndPoint;
    };
}