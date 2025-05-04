#include "../include/tabular.hpp"

int main() {
    using namespace tabular;

    Table table;
    table.addRow({"Anas", "Hamdane", "very very very very loooooooooooooooooooooooooooooooooooooooooooooooooooooooong texto"});
    table.addRow({"   HELLO", "      How are u", "       Good"});

    table.printTable();
    return 0;
}