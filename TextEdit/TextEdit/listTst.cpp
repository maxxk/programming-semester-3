#include <iostream>
#include <string.h>
#include "L2List.h"

using namespace std;

static void printHelp();

class ListElement: public L2ListHeader {
public:
    int value;
};

int main() {
    cout << "Test of class L2List\n";

    L2List list;
    int x;

    char line[256];
    while (true) {
        cout << "Input command (addBefore, removeAfter, moveForw, moveBack, init, show, quit):\n" ;
        cin >> line;
        try {
            if (strcmp(line, "addBefore") == 0) {
                cin >> x;
                ListElement* e = new ListElement();
                e->value = x;
                list.addBefore(e);
            } else if (strcmp(line, "removeAfter") == 0) {
                list.removeAfter();
            } else if (strcmp(line, "moveForw") == 0) {
                list.moveForward();
            } else if (strcmp(line, "moveBack") == 0) {
                list.moveBack();
            } else if (strcmp(line, "init") == 0) {
                list.removeAll();
            } else if (strcmp(line, "show") == 0) {
                int s = list.size();
                cout << "size = " << s << endl;
                L2List::const_iterator b0 = list.begin();
                L2List::const_iterator b1 = list.endBefore();
                L2List::const_iterator e0 = list.beginAfter();
                L2List::const_iterator e1 = list.end();

                L2List::const_iterator i = b0;
                while (i != b1) {
                    cout << ((const ListElement&)*i).value << ' ';
                    ++i;
                }
                cout << "_ ";

                i = e0;
                while (i != e1) {
                    cout << ((const ListElement&)*i).value << ' ';
                    ++i;
                }

                if (s > 0)
                    cout << "\n----\n";
            } else if (line[0] == 'q' || line[0] == 0) {
                break;
            } else {
                cout << "Illegal command.\n";
                printHelp();
            }
        } catch (L2ListException& e) {
            cout << "L2ListException: " << e.reason << endl;
        }
    }
    return 0;
}

static void printHelp() {
    cout <<
        "Test of L2List:\n"
        "Commands:\n"
        "\taddBefore n\n"
        "\tremoveAfter\n"
        "\tmoveForw\n"
        "\tmoveBack\n"
        "\tinit\n"
        "\tshow\n";
}
