#include <iostream.h>
#include <string.h>
#include "Text.h"

int main() {
    Text text;
    cout << "Test of class Text\n";
    while (true) {
        char command[256];
        char filePath[1024];
        cout << "Give command\n"
                "(load file_name, save file_name, getString n, show, quit):\n";
        cin >> command;
        if (command[0] == 'q') {
            break;
        } else if (strcmp(command, "load") == 0) {
            cin >> filePath;
            bool res = text.load(filePath);
            if (res)
                cout << "File loaded successfully.\n";
            else
                cout << "Falure in file loading.\n";
        } else if (strcmp(command, "show") == 0) {
            Text::const_iterator i = text.begin();
            Text::const_iterator e = text.end();
            while (i != e) {
                cout << i->getString() << endl;
                i++;
            }
        } else if (strcmp(command, "getString") == 0) {
            int n;
            cin >> n;
            cout << text.getString(n) << endl;
        } else if (strcmp(command, "save") == 0) {
            cin >> filePath;
            bool res = text.save(filePath);
            if (res)
                cout << "File saved successfully.\n";
            else
                cout << "Falure in file saving.\n";
        }
    }
    return 0;
}
