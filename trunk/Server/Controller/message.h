/*
 * message.h
 *
 * Modified on:    $Date$
 * Last Edited By: $Author$
 * Revision:       $Revision$
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

// This is a simple wrapper for the streambuf class that I find easier to use.
class message {
    public:
        enum { max_length = 512 };
        message() { sbuf_.prepare(max_length); }

        // copy constructor
        // since streambuf is noncopyable, we must make a new one and manually copy the data
        message(const message& msg) {
            const boost::asio::streambuf& sb(msg.buffer());

            boost::asio::streambuf::const_buffers_type data = sb.data();

            std::string s(boost::asio::buffers_begin(data), boost::asio::buffers_end(data));

            from_string(s.c_str(), s.length()); 
        }

        ~message() {}

        // data(b) will return a string containing the first b bytes of data in the streambuf
        // but will NOT consume the data
        std::string data(size_t bytes) {
            boost::asio::streambuf::const_buffers_type data = sbuf_.data();

            std::string s(boost::asio::buffers_begin(data), boost::asio::buffers_begin(data) + bytes);

            return s;
        }

        // data() will return the entire contents of the buffer
        // but will NOT consume the data
        std::string data() {
            boost::asio::streambuf::const_buffers_type data = sbuf_.data();

            std::string s(boost::asio::buffers_begin(data), boost::asio::buffers_end(data));

            return s;
        }

        // returns a reference to the underlying buffer
        boost::asio::streambuf& buffer() {
            boost::asio::streambuf& ret = sbuf_;
            return ret;
        }

        // const version of buffer() is necessary for the copy constructor
        const boost::asio::streambuf& buffer() const {
            const boost::asio::streambuf& ret = sbuf_;
            return ret;
        }

        // removes bytes from the beginning of the buffer
        void consume(size_t bytes) {
            if (bytes > sbuf_.size())
                bytes = sbuf_.size();
            sbuf_.consume(bytes);
        }

        // puts the string on the end of the buffer
        void from_string(const char* cstr, size_t len) {
            if (len > max_length)
                len = max_length;
            sbuf_.sputn(cstr, len);
        }

    private:
        boost::asio::streambuf sbuf_;
};

#endif /* MESSAGE_H_ */


/* vi: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */
