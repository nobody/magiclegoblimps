
#include <iostream>
#include <deque>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>


using boost::asio::ip::tcp;

void work(boost::asio::io_service &io_){
    std::cout << "Worker thread calling io_.run()\n";
    io_.run();
}

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

class AdminClient {
    public:
        AdminClient(tcp::socket&);
        ~AdminClient();

        void write(message &m);
        void close();

    private:
        tcp::socket &sock_;
        message read_message_;
        std::deque<message> write_queue_;

        void read_handler(const boost::system::error_code&, size_t);
        void internal_write(message msg);
        void write_handler(const boost::system::error_code& error);

};

AdminClient::AdminClient(tcp::socket &socket)
    : sock_(socket)
{

    std::cout << "Calling async_read()\n";
    boost::asio::async_read_until(sock_,
        read_message_.buffer(), '\n',
        boost::bind(&AdminClient::read_handler, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

AdminClient::~AdminClient(){
}


void AdminClient::read_handler(const boost::system::error_code& error, std::size_t bytes) {
    std::cout << "Recieved " << bytes << " bytes.\n";

    // The extra parens around the first argument here are necessary 
    // to avoid g++ interpreting this as a function declaration
    //std::string s((std::istreambuf_iterator<char>(&read_buffer_)), std::istreambuf_iterator<char>());

    if (error == boost::asio::error::eof)
        close();
    else if (error == boost::asio::error::operation_aborted)
        return;
    else if (error){
        std::cout << "Throwing an error.\n";
        throw boost::system::system_error(error); // Some other error.
    }

    std::string s(read_message_.data(bytes));
    std::cout << "Built string.\n";

    read_message_.consume(bytes);
    std::cout << read_message_.buffer().size() << " bytes remaining in buffer.\n";

    std::cout << s << "\n-----------------------\n";

    boost::system::error_code err;
    boost::asio::async_read_until(sock_, read_message_.buffer(), '\n', 
        boost::bind(&AdminClient::read_handler, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void AdminClient::write(message &m) {
    std::cout << "Called write() with \"" << m.data() << "\" as the message\n";

    message msg(m);
    sock_.get_io_service().post(boost::bind(&AdminClient::internal_write, this, msg));
}

void AdminClient::internal_write(message msg) {

    bool writing = !write_queue_.empty();

    std::cout << "Adding message \"" << msg.data() << "\" to queue\n";
    write_queue_.push_back(msg);

    if (!writing){
        boost::asio::async_write(sock_, boost::asio::buffer(write_queue_.front().data()), 
                boost::bind(&AdminClient::write_handler, this, 
                    boost::asio::placeholders::error));
    }
}


void AdminClient::write_handler(const boost::system::error_code& error) {
    if (!error) {
        message msg(write_queue_.front());
        std::cout << "Successfully wrote \"" << msg.data() << "\" to the socket.\n";

        write_queue_.pop_front();
        while(!write_queue_.empty()){
        boost::asio::async_write(sock_, boost::asio::buffer(write_queue_.front().data()), 
                boost::bind(&AdminClient::write_handler, this, 
                    boost::asio::placeholders::error));
            
        }
    } else {
        close();
    }
}

void AdminClient::close() {
    std::cout << "Closing socket.\n";
    sock_.close();
}


int main(int argc, char* argv[]) {
    std::string hostname;
    if (argc == 1) {
        hostname = "localhost";
    } else if (argc != 2) {
        std::cerr << "Usage: AdminClient <host>" << std::endl;
            return 1;
    } else {
        hostname = argv[1];
    }

    boost::asio::io_service io;

    tcp::socket socket(io);

    try {
        tcp::resolver resolver(io);
        tcp::resolver::query query(hostname, "10000");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoint_iterator != end) {
            socket.close();
            socket.connect(*endpoint_iterator++, error);
        }
        if (error)
            throw boost::system::system_error(error);

        std::cout << "Socket connected to " 
            << socket.remote_endpoint().address().to_string() 
            << ":" << socket.remote_endpoint().port() 
            << std::endl;

    } catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    AdminClient ac(socket);

    boost::thread worker(work, boost::ref(io));

    char line[message::max_length + 1];
    while(std::cin.getline(line, message::max_length + 1)) {
        message msg;
        msg.from_string(line, std::strlen(line));
        ac.write(msg);
    }

    ac.close();
    worker.join();
    std::cout << "Worker.join() completed.\n";

}
