#include "Request.hpp"
#include <iostream>

int main(void)
{
    Request req;
    //tests
   // _buffer = "";

    // duplicate field name but with appending characteristics
    // _buffer = "GET / HTTP/1.1\r\nHost: Leila : John\r\nHost: 123123, 12313d\r\n\r\n";
    std::string buffer;
    
    /*  test inputs */
    buffer = "GET / HTTP/1.1\r\nHost:Maira\r\nContent-Length: 2\r\nTE: You Know\r\nAccept-Encoding: raw\r\nTE: oi\r\n\r\nOi";

    // chunked
    //_buffer = "GET / HTTP/1.1\r\nHost: Maira\r\nTransfer-Encoding: chunked\r\n\r\n2\r\nOi\r\n1\r\n!\r\n0\r\n\r\n";
    // bigger chunk
    //_buffer = "GET / HTTP/1.1\r\nHost: example.com\r\nTransfer-Encoding: chunked\r\n\r\n2A\r\nThis chunk has 42 characters incl alphanum\r\n1\r\n!\r\n0\r\n\r\n";

    //  _buffer = "GET / HTTP/1.1\r\nHost:Maira\r\nContent-Length: 12\r\nTransfer-Encoding: chunked\r\nAccept: enconding\r\n\r\nMozilla i am here\nSomething written\nDeveloper Network";
    // _buffer = "B\r\n \r\n E";
    //_buffer = "\r\n Before there was a LWS to be remove\r\n\tand here too\r\n \r\n \r\n\t\r\n \r\n\tbefore this there were 5x them";

    /*  Parse Request Function */
    req.parse_request(buffer.c_str());

    /*  Print results */
    std::cout << std::endl;
    std::cout << "PARSED :" << std::endl;
    std::cout << "method   <" << req.get_method() << ">" << std::endl;
    std::cout << "uri      <" << req.get_uri() << ">" << std::endl;
    std::cout << "protocol <" << req.get_protocol() << ">" << std::endl;
    std::cout << "version  <" << req.get_version() << ">" << std::endl;

    std::cout << "-------- Headers --------" << std::endl;
    req.print_headers_map();

    std::cout << "---------GET HEADER VALUE-------" << std::endl;
    std::cout << "'Content-Length' value is : <" << req.get_header_per_key("Content-Length") << ">" << std::endl;
    std::cout << "'Transfer-Encoding' value is : <" << req.get_header_per_key("Transfer-Encoding") << ">" << std::endl;

    std::cout << "Existing Key 'Host' value is : <" << req.get_header_per_key("Host") << ">" << std::endl;
    std::cout << "Non-Existing Key 'Lasagna' value is : <" << req.get_header_per_key("Lasagna") << ">" << std::endl;

    std::cout << "---------  BODY  -------" << std::endl;
    std::cout << "Body <" << req.get_body() << ">" << std::endl; 

    return (0);
}