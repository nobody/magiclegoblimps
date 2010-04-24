
#include "../DataFile.h"

int main() {
    DataFile df("robot.dat", DataFile::ROBOT);

    Vector_ts<Robot*>* robots = new Vector_ts<Robot*>;

    boost::asio::ip::tcp::endpoint e;
    Robot* rob = new Robot(e, 65535);
    rob->setXCord(7);
    rob->setYCord(15);
    rob->setVideoURL("10.10.1.100");

    Robot* rob2 = new Robot(e, 32767);
    rob2->setXCord(31);
    rob2->setYCord(3);
    rob2->setVideoURL("10.10.1.101");

    robots->push_back(rob);
    robots->push_back(rob2);

    df.write(robots);

    delete rob;
    delete rob2;

    delete robots;


    robots = (Vector_ts<Robot*>*)df.read();


    Vector_ts<Robot*>::iterator it;
    Vector_ts<Robot*>::iterator it_end = robots->end();
    
    for (it = robots->begin(); it < it_end; ++it) {
        std::cout << "Got Robot:\n";
        std::cout << "  RID: " << (*it)->getRID() << "\n";
        std::cout << "  X:   " << (*it)->getXCord() << "\n";
        std::cout << "  Y:   " << (*it)->getYCord() << "\n";
        std::cout << "  URL: " << (*it)->getVideoURL() << "\n";
        std::cout << "\n";

        delete (*it);
    }

    delete robots;


    DataFile dfo("object.dat", DataFile::OBJECT);

    Vector_ts<Object*>* objs = new Vector_ts<Object*>;

    char tmp1[] = "Construct a module that blocks the reception of ICMP echo requests (ping) after 10 such packets have been received. The module will use the netfilter framework in the Linux Kernel. Netfilter provides five hooks. All packets, including incoming, outgoing and forwarded, traverse these five hooks. You as a developer can register your own function to listen for packets on any or all of the hooks. After the registration, whenever a packet traverses a hook for which you have registered your own function, that fun";
    char tmp2[] = "Construct a module that blocks the reception of ICMP echo requests (ping) after 10 such packets have been received. The module will use the netfilter framework in the Linux Kernel. Netfilter provides five hooks. All packets, including incoming, outgoing and forwarded, traverse these five hooks. You as a developer can register your own function to listen for packets on any or all of the hooks. After the registration, whenever a packet traverses a hook for which you have registered your own function, that fun";
    char tmp3[] = "Construct a module that blocks the reception of ICMP echo requests (ping) after 10 such packets have been received. The module will use the netfilter framework in the Linux Kernel. Netfilter provides five hooks. All packets, including incoming, outgoing and forwarded, traverse these five hooks. You as a developer can register your own function to listen for packets on any or all of the hooks. After the registration, whenever a packet traverses a hook for which you have registered your own function, that fun";
    char tmp4[] = "Construct a module that blocks the reception of ICMP echo requests (ping) after 10 such packets have been received. The module will use the netfilter framework in the Linux Kernel. Netfilter provides five hooks. All packets, including incoming, outgoing and forwarded, traverse these five hooks. You as a developer can register your own function to listen for packets on any or all of the hooks. After the registration, whenever a packet traverses a hook for which you have registered your own function, that fun";
    char tmp5[] = "Construct a module that blocks the reception of ICMP echo requests (ping) after 10 such packets have been received. The module will use the netfilter framework in the Linux Kernel. Netfilter provides five hooks. All packets, including incoming, outgoing and forwarded, traverse these five hooks. You as a developer can register your own function to listen for packets on any or all of the hooks. After the registration, whenever a packet traverses a hook for which you have registered your own function, that fun";
    char tmp6[] = "Construct a module that blocks the reception of ICMP echo requests (ping) after 10 such packets have been received. The module will use the netfilter framework in the Linux Kernel. Netfilter provides five hooks. All packets, including incoming, outgoing and forwarded, traverse these five hooks. You as a developer can register your own function to listen for packets on any or all of the hooks. After the registration, whenever a packet traverses a hook for which you have registered your own function, that fun";
    char box1[] = "a";
    char box2[] = "b";
    char box3[] = "c";
    char box4[] = "d";
    char box5[] = "e";
    char box6[] = "f";
    Object* obj1 = new Object(1, "Object 1", tmp1, 512, box1, 1);
    Object* obj2 = new Object(2, "Object 2", tmp2, 512, box2, 1);
    Object* obj3 = new Object(3, "Object 3", tmp3, 512, box3, 1);
    Object* obj4 = new Object(4, "Object 4", tmp4, 512, box4, 1);
    Object* obj5 = new Object(5, "Object 5", tmp5, 512, box5, 1);
    Object* obj6 = new Object(6, "Object 6", tmp6, 512, box6, 1);

    objs->push_back(obj1);
    objs->push_back(obj2);
    objs->push_back(obj3);
    objs->push_back(obj4);
    objs->push_back(obj5);
    objs->push_back(obj6);

    dfo.write(objs);

    delete obj1;

    delete objs;

    objs = (Vector_ts<Object*>*)dfo.read();

    Vector_ts<Object*>::iterator ito; 
    Vector_ts<Object*>::iterator ito_end = objs->end(); 

    for (ito = objs->begin(); ito < ito_end; ++ito) {
        std::cout << "Got Object:\n"
                  << "  OID:   " << (*ito)->getOID() << "\n"
                  << "  name:  " << (*ito)->getName() << "\n";

        long size = (*ito)->getColorsize();
        std::cout << "  color_s: " << size << "\n";

        char* color = (*ito)->getColor();

        std::cout << "  color: " << std::hex;

        for (long i = 0; i < size; ++i)
            std::cout << (short)color[i];

        std::cout << std::dec << "\n";

        delete (*ito);

    }

    delete objs;

    return 0;
}
