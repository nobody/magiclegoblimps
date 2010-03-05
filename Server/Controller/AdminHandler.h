/*
 * AdminSocket.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef ADMINHANDLER_H_
#define ADMINHANDLER_H_

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include "TcpServer.h"

class AdminHandler : public TcpServer::ConnHandler {
    public:
        AdminHandler(); 
        virtual ~AdminHandler();

        typedef std::map<boost::asio::ip::tcp::endpoint, TcpServer::TcpConnection::pointer> conn_map;

        virtual void onConnect(TcpServer::TcpConnection::pointer tcp_connection);

    private:
        //void threaded_on_connect(int connIndex);
        void threaded_on_connect(boost::asio::ip::tcp::endpoint);

        static conn_map connections;

        class message {
            public:
                enum { max_length = 512 };
                message() { sbuf_.prepare(max_length); }

                message(const message& msg) {
                    const boost::asio::streambuf& sb(msg.buffer());

                    boost::asio::streambuf::const_buffers_type data = sb.data();

                    std::string s(boost::asio::buffers_begin(data), boost::asio::buffers_end(data));

                    from_string(s.c_str(), s.length()); 
                }
                ~message() {}

                std::string data(size_t bytes) {
                    boost::asio::streambuf::const_buffers_type data = sbuf_.data();

                    std::string s(boost::asio::buffers_begin(data), boost::asio::buffers_begin(data) + bytes);

                    return s;
                }

                std::string data() {
                    boost::asio::streambuf::const_buffers_type data = sbuf_.data();

                    std::string s(boost::asio::buffers_begin(data), boost::asio::buffers_end(data));

                    return s;
                }

                const boost::asio::streambuf& buffer() const {
                    const boost::asio::streambuf& ret = sbuf_;
                    return ret;
                }

                boost::asio::streambuf& buffer() {
                    boost::asio::streambuf& ret = sbuf_;
                    return ret;
                }

                void consume(size_t bytes) {
                    if (bytes > sbuf_.size())
                        bytes = sbuf_.size();
                    sbuf_.consume(bytes);
                }

                void from_string(const char* cstr, size_t len) {
                    if (len > max_length)
                        len = max_length;
                    sbuf_.sputn(cstr, len);
                }

            private:
                boost::asio::streambuf sbuf_;
        };
        class session {
            public:
                session(TcpServer::TcpConnection::pointer);
                ~session();

                void start();

            private:
                TcpServer::TcpConnection::pointer conn_;
                message read_message_;

                void write_handler(const boost::system::error_code& error,  std::size_t bytes_transferred);
                void read_handler(const boost::system::error_code& error,  std::size_t bytes_transferred);
                void close();
                void do_close();

        };

        std::vector<boost::shared_ptr<session> > sessions;
};


#endif /* ADMINHANDLER_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
